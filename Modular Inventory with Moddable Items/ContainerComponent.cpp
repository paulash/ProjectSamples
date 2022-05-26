#include "ContainerComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Characteristics/WeightCharacteristic.h"
#include "Net/UnrealNetwork.h"

void UContainerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UContainerComponent, Items);
	DOREPLIFETIME(UContainerComponent, Equipped);
}

UContainerComponent::UContainerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	SetIsReplicatedByDefault(true);
}

void UContainerComponent::BeginPlay()
{
	if (GetWorld()->IsServer()) {
		TArray<AItem*> OldItems = Items;

		for (int i = 0; i < DefaultItems.Num(); i++) {
			if (!DefaultItems[i].ItemClass) continue;
			AItem* newItem = GetWorld()->SpawnActor<AItem>(DefaultItems[i].ItemClass, GetOwner()->GetActorTransform());

			bool didStack = false;
			for (int x = 0; x < Items.Num(); x++) {
				if (Items[x]->IsStackable(newItem)) {
					didStack = true;
					Items[x]->SetCount(Items[x]->GetCount() + DefaultItems[i].Count);

					newItem->Destroy();

					Items = Items;
					OnRep_Items(OldItems);
					break;
				}
			}
			if (didStack) continue;

			newItem->SetCount(DefaultItems[i].Count);
			PickupItem(newItem);
		}
	}

	APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn && Pawn->IsLocallyControlled()) {
		//Spawn capture2d
		RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 512, 512);

		PreviewArm = NewObject<USceneComponent>(GetOwner());
		PreviewArm->RegisterComponent();
		PreviewArm->SetWorldLocation(FVector(0, 0, 0));

		ItemPreviewCapture = NewObject<USceneCaptureComponent2D>(GetOwner());
		ItemPreviewCapture->RegisterComponent();
		ItemPreviewCapture->FOVAngle = 100;
		ItemPreviewCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
		ItemPreviewCapture->ShowFlags.Fog = false;
		ItemPreviewCapture->ShowFlags.VolumetricFog = false;
		ItemPreviewCapture->PostProcessSettings.bOverride_IndirectLightingColor = true;
		ItemPreviewCapture->PostProcessSettings.bOverride_IndirectLightingIntensity = true;
		ItemPreviewCapture->PostProcessSettings.IndirectLightingColor = FColor::White;
		ItemPreviewCapture->PostProcessSettings.IndirectLightingIntensity = 20;
		ItemPreviewCapture->PostProcessSettings.AddBlendable(ItemPreviewFX, 1);
		ItemPreviewCapture->TextureTarget = RenderTarget;

		ItemPreviewCapture->SetWorldLocation(FVector(-50, 0, ITEM_HIDE_DEPTH));
		ItemPreviewCapture->AttachToComponent(PreviewArm, FAttachmentTransformRules::KeepWorldTransform);
	}


	Super::BeginPlay();
}

void UContainerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (PreviewArm) {
		PreviewArm->AddRelativeRotation(FRotator(0, DeltaTime * 12, 0));
	}
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UContainerComponent::EndPlay(EEndPlayReason::Type reason)
{
	if (GetWorld()->IsServer()) {
		for (int i = 0; i < Items.Num(); i++) {
			if (!Items[i]) continue;
			Items[i]->Destroy();
		}
	}
	Super::EndPlay(reason);
}

bool UContainerComponent::ContainsItem(AItem* Item)
{
	if (!Item) return false;
	return Items.Contains(Item);
}

float UContainerComponent::GetTotalWeight()
{
	float weight = 0;
	for (int i = 0; i < Items.Num(); i++) {
		if (Equipped.Contains(Items[i])) continue; // equipped things have no weight.

		float itemWeight = 0;
		if (Items[i]->GetCharacteristicValue(UWeightCharacteristic::StaticClass(), itemWeight)) {
			weight += itemWeight * Items[i]->GetCount();
		}
	}
	return weight;
}

AItem* UContainerComponent::GetItem(TSubclassOf<AItem> ItemClass)
{
	for (int i = 0; i < Items.Num(); i++) {
		if (Items[i]->GetClass() == ItemClass) {
			return Items[i];
		}
	}
	return NULL;
}

int UContainerComponent::GetItemCount(TSubclassOf<AItem> ItemClass)
{
	AItem* ItemActor = GetItem(ItemClass);
	if (!ItemActor) return 0;
	return ItemActor->GetCount();
}

void UContainerComponent::PickupItem(AItem* Item)
{
	if (!GetWorld()->IsServer()) return;
	if (!Item) return;
	if (Items.Contains(Item)) return;

	float weight = 0;
	if (Item->GetCharacteristicValue(UWeightCharacteristic::StaticClass(), weight)) {
		float totalWeight = weight * Item->GetCount();
		if (totalWeight + GetTotalWeight() > GetMaxWeight()) {
			return; // to heavy.
		}
	}

	TArray<AItem*> OldItems = Items;

	for (int i = 0; i < Items.Num(); i++) {
		if (Items[i]->IsStackable(Item)) {
			Items[i]->SetCount(Items[i]->GetCount() + Item->GetCount());
			Item->Destroy();

			Items = Items;
			OnRep_Items(OldItems);
			return;
		}
	}
	Items.Add(Item);
	Items = Items;
	OnRep_Items(OldItems);

	Item->SetOwner(GetOwner());
}

bool UContainerComponent::DropItem(AItem* Item, int Count/*=-1*/)
{
	return false;
}

bool UContainerComponent::RemoveItem(AItem* Item, int Count/*=-1*/)
{
	if (!GetWorld()->IsServer()) return false;
	if (!Item) return false;
	if (Count == -1) {
		Count = Item->GetCount();
	}
	if (!Items.Contains(Item)) return false;

	TArray<AItem*> OldItems = Items;

	if (Item->GetCount() >= Count) {
		Item->SetCount(Item->GetCount() - Count);
		if (Item->GetCount() == 0) {
			Items.Remove(Item);
			Item->Destroy();

			Items = Items;
			OnRep_Items(OldItems);
		}
		return true;
	}
	return false;
}

bool UContainerComponent::TransferItem(UContainerComponent* TargetContainer, AItem* Item, int Count/*=-1*/)
{
	if (!GetWorld()->IsServer()) return false;
	if (!Item) return false;
	if (Item->Untransferable) return false;
	if (!ContainsItem(Item)) {
		return false;
	}
	if (Item->GetCount() < Count) {
		return false;
	}

	TArray<AItem*> OldItems = Items;

	// TODO: add a way to check how much is allowed, and use that in the transfer count UI as the 'max'
	float weight = 0;
	if (Item->GetCharacteristicValue(UWeightCharacteristic::StaticClass(), weight)) {
		float totalWeight = weight * Count;
		if (totalWeight + GetTotalWeight() > GetMaxWeight()) {
			return false; // to heavy.
		}
	}

	if (Count == -1 || Item->GetCount() == Count) {
		Items.Remove(Item);
		Items = Items;

		Item->OnTransfered();
		OnRep_Items(OldItems);
		TargetContainer->PickupItem(Item);
	}
	else {
		Item->SetCount(Item->GetCount() - Count);

		AItem* newItem = GetWorld()->SpawnActor<AItem>(Item->GetClass(), FTransform::Identity);
		newItem->SetCount(Count);

		OnRep_Items(OldItems);
		TargetContainer->PickupItem(newItem);
	}
	return true;
}

void UContainerComponent::SetItemPreview(AItem* Item)
{
	if (!ItemPreviewCapture) return;
	if (!Item) return;
	
	ItemPreviewCapture->ShowOnlyActors.Empty();
	ItemPreviewCapture->ShowOnlyActors.Add(Item);
	ItemPreviewCapture->CaptureScene();
}

TArray<AItem*> UContainerComponent::GetSortedItems()
{
	TArray<AItem*> sortedItems = TArray<AItem*>(Items);
	sortedItems.Sort([](const AItem& ip1, const AItem& ip2) {
		return ip1.DisplayName.CompareToCaseIgnored(ip2.DisplayName) < 0;
	});
	return sortedItems;
}

TArray<AItem*> UContainerComponent::GetSortedItemsWithinCategory(FName Category)
{
	TArray<AItem*> categoryItems;
	TArray<AItem*> sortedItems = GetSortedItems();
	for (int i = 0; i < sortedItems.Num(); i++) {
		if (sortedItems[i]->Category == Category) {
			categoryItems.Add(sortedItems[i]);
		}
	}
	return categoryItems;
}

void UContainerComponent::SendAddItemEvent(AItem* Item, int Change)
{
	OWNER_EventAddItem(Item, Change);
}

bool UContainerComponent::IsEquipped(AItemGear* ItemGear)
{
	return Equipped.Contains(ItemGear);
}

void UContainerComponent::Equip(AItemGear* ItemGear)
{
	if (!GetWorld()->IsServer()) return;
	if (!ContainsItem(ItemGear)) return;
	if (!CanEquip(ItemGear)) return;

	TArray<AItemGear*> OldEquipped = Equipped;
	TArray<AItemGear*> gearInSockets;
	for (int i = 0; i < Equipped.Num(); i++) {

		for (int s = 0; s < Equipped[i]->Sockets.Num(); s++) {
			if (ItemGear->Sockets.Contains(Equipped[i]->Sockets[s])) {
				gearInSockets.Add(Equipped[i]);
			}
		}
	}
	for (int i = 0; i < gearInSockets.Num(); i++) {
		Equipped.Remove(gearInSockets[i]);
	}

	Equipped.Add(ItemGear);
	Equipped = Equipped; // force rep.
	OnRep_Equipped(OldEquipped);
}

void UContainerComponent::Unequip(AItemGear* ItemGear)
{
	if (!GetWorld()->IsServer()) return;
	//if (!ContainsItem(ItemGear)) return;
	if (!IsEquipped(ItemGear)) return;

	TArray<AItemGear*> OldEquipped = Equipped;

	Equipped.Remove(ItemGear);

	Equipped = Equipped; // force rep.
	OnRep_Equipped(OldEquipped);
}

bool UContainerComponent::CanEquip(AItemGear* ItemGear)
{
	for (int s = 0; s < ItemGear->Sockets.Num(); s++) {
		if (!EquipmentSockets.Contains(ItemGear->Sockets[s])) {
			return false;
		}
	}
	return true;
}

AItemGear* UContainerComponent::GetEquippedBySocket(FName socket)
{
	for (int i = 0; i < Equipped.Num(); i++) {
		if (Equipped[i]->Sockets.Contains(socket)) {
			return Equipped[i];
		}
	}
	return NULL;
}

int UContainerComponent::GetMaxAllowedTransfer(AItem* Item)
{
	int count = 0;
	float weight = 0;
	if (Item->GetCharacteristicValue(UWeightCharacteristic::StaticClass(), weight)) {
		for (int i = 0; i < Item->GetCount(); i++) {
			float totalWeight = weight * i;
			if (totalWeight > GetMaxWeight()) {
				break;
			}
			count++;
		}
	}
	return count;
}

void UContainerComponent::OWNER_EventAddItem_Implementation(AItem* Item, int Change)
{
	OnAddItem.Broadcast(Item, Change);
}

void UContainerComponent::Request_TransferItem_Implementation(UContainerComponent* SourceContainer, UContainerComponent* TargetContainer, AItem* Item, int Count)
{
	SourceContainer->TransferItem(TargetContainer, Item, Count);
}

bool UContainerComponent::Request_TransferItem_Validate(UContainerComponent* SourceContainer, UContainerComponent* TargetContainer, AItem* Item, int Count)
{
	return SourceContainer->ContainsItem(Item);
}

void UContainerComponent::OnRep_Items(TArray<AItem*> OldItems)
{
	TArray<AItem*> AddedItems;
	TArray<AItem*> RemovedItems;

	for (int i = 0; i < OldItems.Num(); i++) {
		if (Items.Contains(OldItems[i])) continue;
		RemovedItems.Add(OldItems[i]);
	}
	for (int i = 0; i < Items.Num(); i++) {
		if (OldItems.Contains(Items[i])) continue;
		AddedItems.Add(Items[i]);
	}

	OnUpdateInventory.Broadcast(AddedItems, RemovedItems);
}

void UContainerComponent::OnRep_Equipped(TArray<AItemGear*> OldEquipped)
{
	for (int i = 0; i < OldEquipped.Num(); i++) {
		if (Equipped.Contains(OldEquipped[i])) continue;
		OnUnequippedItem.Broadcast(OldEquipped[i]);
		OldEquipped[i]->OnUnequip(this);
	}

	for (int i = 0; i < Equipped.Num(); i++) {
		if (OldEquipped.Contains(Equipped[i])) continue;
		OnEquippedItem.Broadcast(Equipped[i]);
		Equipped[i]->OnEquipped(this);
	}
}
