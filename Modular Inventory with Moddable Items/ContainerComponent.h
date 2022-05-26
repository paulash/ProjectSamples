
#pragma once

#include "CoreMinimal.h"
#include "../BlackOcean.h"
#include "Components/ActorComponent.h"
#include "Item.h"
#include "ItemGear.h"
#include "../Structures/BaseStructure.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ContainerComponent.generated.h"

#define ITEM_HIDE_DEPTH -10000

USTRUCT(BlueprintType)
struct FItemSetup
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AItem> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int Count = 1;

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUpdateInventory, const TArray<AItem*>&, AddedItems, const TArray<AItem*>&, RemovedItems);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAddItem, AItem*, Item, int, change);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquippedItem, AItemGear*, ItemGear);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnequippedItem, AItemGear*, ItemGear);

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class BLACKOCEAN_API UContainerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	UMaterialInstance* ItemPreviewFX;

	USceneComponent* PreviewArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneCaptureComponent2D* ItemPreviewCapture;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTextureRenderTarget2D* RenderTarget;

	UPROPERTY(BlueprintAssignable)
	FUpdateInventory OnUpdateInventory;

	UPROPERTY(BlueprintAssignable)
	FAddItem OnAddItem;

	UPROPERTY(BlueprintAssignable)
	FEquippedItem OnEquippedItem;

	UPROPERTY(BlueprintAssignable)
	FUnequippedItem OnUnequippedItem;

	UPROPERTY(EditAnywhere)
	TArray<FItemSetup> DefaultItems;

	UPROPERTY(EditDefaultsOnly)
	TArray<FName> EquipmentSockets;

	UPROPERTY(ReplicatedUsing=OnRep_Items, BlueprintReadOnly)
	TArray<AItem*> Items;

	UPROPERTY(ReplicatedUsing=OnRep_Equipped, BlueprintReadOnly)
	TArray<AItemGear*> Equipped; // use an array, and just write functions to find occupied sockets.

	UContainerComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(EEndPlayReason::Type reason) override;

	UFUNCTION(BlueprintPure)
	bool ContainsItem(AItem* Item);

	UFUNCTION(BlueprintPure)
	float GetTotalWeight();

	UFUNCTION(BlueprintPure)
	AItem* GetItem(TSubclassOf<AItem> ItemClass);

	UFUNCTION(BlueprintPure)
	int GetItemCount(TSubclassOf<AItem> ItemClass);

	UFUNCTION(BlueprintCallable)
	void PickupItem(AItem* Item);

	UFUNCTION(BlueprintCallable)
	bool DropItem(AItem* Item, int Count=-1);

	UFUNCTION(BlueprintCallable)
	bool RemoveItem(AItem* Item, int Count=-1);

	bool TransferItem(UContainerComponent* TargetContainer, AItem* Item, int Count=-1);

	UFUNCTION(BlueprintCallable)
	void SetItemPreview(AItem* Item);

	UFUNCTION(BlueprintPure)
	TArray<AItem*> GetSortedItems();

	UFUNCTION(BlueprintPure)
	TArray<AItem*> GetSortedItemsWithinCategory(FName Category);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void Request_TransferItem(UContainerComponent* SourceContainer, UContainerComponent* TargetContainer, AItem* Item, int Count);

	UFUNCTION(Client, Reliable)
	void OWNER_EventAddItem(AItem* Item, int Change);

	UFUNCTION(BlueprintCallable)
	void SendAddItemEvent(AItem* Item, int Change);

	UFUNCTION(BlueprintPure)
	bool IsEquipped(AItemGear* ItemGear);

	UFUNCTION(BlueprintCallable)
	void Equip(AItemGear* ItemGear);

	UFUNCTION(BlueprintCallable)
	void Unequip(AItemGear* ItemGear);

	UFUNCTION(BlueprintCallable)
	bool CanEquip(AItemGear* ItemGear);

	UFUNCTION(BlueprintPure)
	AItemGear* GetEquippedBySocket(FName socket);

	UFUNCTION(BlueprintPure)
	float GetMaxWeight() { return 500; };

	UFUNCTION(BlueprintPure)
	int GetMaxAllowedTransfer(AItem* item);

private:

	UFUNCTION()
	void OnRep_Items(TArray<AItem*> OldItems);

	UFUNCTION()
	void OnRep_Equipped(TArray<AItemGear*> OldEquipped);
};