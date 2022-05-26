#include "Item.h"
#include "ContainerComponent.h"
#include "Characteristics/WeightCharacteristic.h"
#include "Characteristics/ConditionCharacteristic.h"
#include "ItemModification.h"
#include "Net/UnrealNetwork.h"

void AItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItem, Count);
	DOREPLIFETIME(AItem, Characteristic);
	DOREPLIFETIME(AItem, Modifications);
}

AItem::AItem()
{
	DisplayName = FText::FromString("[undefined item]");
	Category = FName("Misc"); // default item category is 'misc'

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	ItemPreviewContainer = CreateDefaultSubobject<USceneComponent>("ItemPreviewContainer");
	ItemPreviewContainer->SetupAttachment(RootComponent);
	ItemPreviewContainer->SetAbsolute(true, true, true);

	Characteristic.Add(FCharacteristicInstance(UConditionCharacteristic::StaticClass(), 1));
	Characteristic.Add(FCharacteristicInstance(UWeightCharacteristic::StaticClass(), 1));

	bReplicates = true;
	SetReplicatingMovement(true);
	bNetUseOwnerRelevancy = true;
}

void AItem::BeginPlay()
{
	GetRootComponent()->SetVisibility(false, true);
	ItemPreviewContainer->SetVisibility(true, true);

	SetActorLocation(FVector(0, 0, ITEM_HIDE_DEPTH));
	ItemPreviewContainer->SetWorldLocation(FVector(0, 0, ITEM_HIDE_DEPTH));

	if (IsPreviewOnly) {
		SetReplicates(false);
	}

	if (GetWorld()->IsServer() || IsPreviewOnly) {
		for (int i = 0; i < DefaultModifications.Num(); i++) {
			AItemModification* newItemMod = GetWorld()->SpawnActor<AItemModification>(DefaultModifications[i], GetActorTransform());
			AddItemModification(newItemMod);
		}
	}
	Super::BeginPlay();
}

void AItem::EndPlay(EEndPlayReason::Type reason)
{
	for (int i = 0; i < Modifications.Num(); i++) {
		Modifications[i]->Destroy();
	}
	Super::EndPlay(reason);
}

bool AItem::IsStackable_Implementation(AItem* OtherItem)
{
	return GetClass() == OtherItem->GetClass();
}

void AItem::SetCount(int _count)
{
	if (!GetWorld()->IsServer()) return;
	Count = _count;
	OnRep_Update();
}

bool AItem::GetCharacteristicValue(TSubclassOf<UCharacteristic> CharacteristicClass, float& Value)
{
	for (int i = 0; i < Characteristic.Num(); i++) {
		if (Characteristic[i].CharacteristicClass == CharacteristicClass) {
			Value = Characteristic[i].Value;
			return true;
		}
	}
	return false;
}

bool AItem::AddOrUpdateCharacteristicValue(TSubclassOf<UCharacteristic> CharacteristicClass, float Value)
{
	if (!GetWorld()->IsServer() && !IsPreviewOnly) return false;
	for (int i = 0; i < Characteristic.Num(); i++) {
		if (Characteristic[i].CharacteristicClass == CharacteristicClass) {
			Characteristic[i].Value = Value;
			return true;
		}
	}
	Characteristic.Add(FCharacteristicInstance(CharacteristicClass, Value));
	Characteristic = Characteristic;
	OnRep_Update();
	return true;
}

void AItem::AddItemModification(AItemModification* ItemModification)
{
	if (!GetWorld()->IsServer() && !IsPreviewOnly) return;

	ItemModification->SetOwner(this);
	Modifications.AddUnique(ItemModification);
	Modifications = Modifications;
	OnRep_Update();
}

void AItem::RemoveItemModification(AItemModification* ItemModification)
{
	if (!GetWorld()->IsServer() && !IsPreviewOnly) return;

	ItemModification->SetOwner(NULL);
	Modifications.Remove(ItemModification);
	OnRep_Update();
}

void AItem::OnRep_Update()
{
	OnUpdateItem.Broadcast();
}

TArray<FCharacteristicInstance> AItem::GetSortedCharacteristics()
{
	TArray<FCharacteristicInstance> sortedCharacteristic = TArray<FCharacteristicInstance>(Characteristic);
	for (int i = 0; i < Modifications.Num(); i++) {
		sortedCharacteristic.Append(Modifications[i]->Characteristic);
	}

	sortedCharacteristic.Sort([](const FCharacteristicInstance& ip1, const FCharacteristicInstance& ip2) {
		UCharacteristic* default1 = Cast<UCharacteristic>(ip1.CharacteristicClass->GetDefaultObject());
		UCharacteristic* default2 = Cast<UCharacteristic>(ip2.CharacteristicClass->GetDefaultObject());

		return default1->GetPriority() > default2->GetPriority();
	});
	return sortedCharacteristic;
}

void AItem::OnRep_Owner()
{
	OnUpdateItem.Broadcast();
}

void AItem::OnTransfered()
{
	OnUpdateItem.Broadcast();
}
