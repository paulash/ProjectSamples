#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "../CharacterAbility.h"
#include "ItemGear.h"
#include "ItemWeapon.generated.h"


class APlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponAmmoChangedDelegate, int32, OldValue, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateFireAbility, bool, primary);

UCLASS(Blueprintable, BlueprintType)
class BLACKOCEAN_API AItemWeapon : public AItemGear
{
	GENERATED_BODY()

public:

	AItemWeapon();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<AItem> AmmoItemClass;

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	USkeletalMeshComponent* MeshComponent;

	UFUNCTION()
	void OnRep_Ammo(int oldValue);

	UPROPERTY(BlueprintAssignable)
	FWeaponAmmoChangedDelegate OnAmmoChanged;

	UPROPERTY(BlueprintAssignable)
	FUpdateFireAbility OnUpdateFireAbility;

	UFUNCTION(BlueprintCallable)
	bool ConsumeAmmo();

	UFUNCTION(BlueprintCallable)
	void ReloadAmmo();

	UFUNCTION(BlueprintPure)
	bool HasAmmo() {
		return Ammo != 0;
	}

	UFUNCTION(BlueprintPure)
	bool CanReload() {
		return Ammo != MaxAmmo;
	}

	UFUNCTION(BlueprintPure)
	int GetAmmoCount() {
		return Ammo;
	}

	UFUNCTION(BlueprintPure)
	int GetMaxAmmoCount() {
		return MaxAmmo;
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing=OnRep_UpdateFireAbilityClass)
	TSubclassOf<UCharacterAbility> FireAbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing=OnRep_UpdateTacticalAbilityClass)
	TSubclassOf<UCharacterAbility> TacticalAbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AnimationMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer AssignedHolderTags;

	bool IsStackable_Implementation(AItem* OtherItem) override;

	virtual void OnEquipped_Implementation(UContainerComponent* Equipper) override;
	virtual void OnUnequip_Implementation(UContainerComponent* Unequipper) override;

	UFUNCTION(BlueprintCallable)
	void UpdateFireAbility(bool primary, TSubclassOf<UCharacterAbility> AbilityClass);

protected:

	UFUNCTION()
	void OnRep_UpdateFireAbilityClass();

	UFUNCTION()
	void OnRep_UpdateTacticalAbilityClass();

	UPROPERTY(ReplicatedUsing=OnRep_Ammo, BlueprintReadOnly)
	int Ammo = 0;

	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly)
	int MaxAmmo = 0;

};