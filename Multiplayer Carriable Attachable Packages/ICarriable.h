#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../CharacterAbility.h"
#include "ICarriable.generated.h"

#define CARRIABLE_ABILITY_MAX_COUNT 3

UINTERFACE(Blueprintable)
class BLACKOCEAN_API UCarriable : public UInterface
{
	GENERATED_BODY()
};

class BLACKOCEAN_API ICarriable
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TSubclassOf<UCharacterAbility> GetAbility();

	UFUNCTION(BlueprintNativeEvent)
	void OnPickedUp(AActor* Carrier, bool wasInitial);

	UFUNCTION(BlueprintNativeEvent)
	void OnDrop(AActor* Carrier);

	UFUNCTION(BlueprintNativeEvent)
	float GetDropHeight();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanPickup(AActor* Carrier);
	virtual bool CanPickup_Implementation(AActor* Carrier);
};