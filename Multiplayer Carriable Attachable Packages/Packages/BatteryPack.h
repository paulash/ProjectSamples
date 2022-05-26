#pragma once

#include "CoreMinimal.h"
#include "../CarriablePackage.h"
#include "BatteryPack.generated.h"


UCLASS(Blueprintable, BlueprintType)
class BLACKOCEAN_API ABatteryPack : public ACarriablePackage
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	float MaxEnergy = 100;

	UPROPERTY(BlueprintReadOnly, Replicated)
	float Energy = 0;

	UFUNCTION()
	void TickCharge(float rate);

	UFUNCTION()
	void TickDischarge(float rate);

	UFUNCTION()
	void Discharge(float amount);

};