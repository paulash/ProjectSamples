#pragma once

#include "CoreMinimal.h"
#include "ItemGear.h"
#include "../CharacterAbility.h"
#include "ItemImplant.generated.h"

UCLASS()
class BLACKOCEAN_API AItemImplant : public AItemGear
{
	GENERATED_BODY()

public:

	AItemImplant();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UCharacterAbility> Ability1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UCharacterAbility> Ability2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UCharacterAbility> Ability3;

	virtual void OnEquipped_Implementation(UContainerComponent* Equipper) override;
	virtual void OnUnequip_Implementation(UContainerComponent* Unequipper) override;

};