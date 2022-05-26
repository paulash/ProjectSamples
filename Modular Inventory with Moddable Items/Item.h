#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Characteristics/Characteristic.h"
#include "../CharacterAbility.h"
#include "Item.generated.h"

class AItemModification;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateItem);


USTRUCT(BlueprintType)
struct FCharacteristicInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UCharacteristic> CharacteristicClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Value;

	FCharacteristicInstance() {
		CharacteristicClass = NULL;
		Value = 0;
	}

	FCharacteristicInstance(TSubclassOf<UCharacteristic> _CharacteristicClass, float _Value) {
		CharacteristicClass = _CharacteristicClass;
		Value = _Value;
	}

};

UCLASS()
class BLACKOCEAN_API AItem : public AActor
{
	GENERATED_BODY()


protected: 

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Update)
	int Count = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing=OnRep_Update)
	TArray<FCharacteristicInstance> Characteristic;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Update)
	TArray<AItemModification*> Modifications;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AItemModification>> DefaultModifications;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn = "true"))
	bool IsPreviewOnly = false;

	UPROPERTY(BlueprintAssignable)
	FUpdateItem OnUpdateItem;

	AItem();
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type reason) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* Icon = NULL;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName Category;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag UseTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool Untransferable = false;

	UFUNCTION(BlueprintNativeEvent)
	bool IsStackable(AItem* OtherItem);
	virtual bool IsStackable_Implementation(AItem* OtherItem);

	UFUNCTION(BlueprintCallable)
	void SetCount(int _count);

	UFUNCTION(BlueprintPure)
	int GetCount() { return Count; }

	UFUNCTION(BlueprintPure)
	bool GetCharacteristicValue(TSubclassOf<UCharacteristic> CharacteristicClass, float& Value);

	UFUNCTION(BlueprintCallable)
	bool AddOrUpdateCharacteristicValue(TSubclassOf<UCharacteristic> CharacteristicClass, float Value);

	UFUNCTION(BlueprintCallable)
	void AddItemModification(AItemModification* ItemModification);

	UFUNCTION(BlueprintCallable)
	void RemoveItemModification(AItemModification* ItemModification);

	UPROPERTY(BlueprintReadOnly)
	USceneComponent* ItemPreviewContainer;

	UFUNCTION(BlueprintPure)
	TArray<FCharacteristicInstance> GetSortedCharacteristics();

	virtual void OnRep_Owner() override;

	virtual void OnTransfered();

private:

	UFUNCTION()
	void OnRep_Update();
};