#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "../BlackOcean.h"
#include "../BaseCharacter.h"
#include "ItemGear.generated.h"

class UContainerComponent;

UCLASS()
class BLACKOCEAN_API AItemGear : public AItem
{
	GENERATED_BODY()

public:
	AItemGear();

	UPROPERTY(EditDefaultsOnly)
	TArray<FName> Sockets;

	UFUNCTION(BlueprintCallable)
	void Equip();

	UFUNCTION(BlueprintCallable)
	void Unequip();

	UFUNCTION(BlueprintPure)
	ABaseCharacter* GetEquipper(); // pull container off owner, check if in equipped.

	UFUNCTION(BlueprintPure)
	bool IsEquipped();

	UFUNCTION(BlueprintNativeEvent)
	void OnEquipped(UContainerComponent* Equipper);
	virtual void OnEquipped_Implementation(UContainerComponent* Equipper);

	UFUNCTION(BlueprintNativeEvent)
	void OnUnequip(UContainerComponent* Unequipper);
	virtual void OnUnequip_Implementation(UContainerComponent* Unequipper);

	virtual void OnTransfered() override;

};