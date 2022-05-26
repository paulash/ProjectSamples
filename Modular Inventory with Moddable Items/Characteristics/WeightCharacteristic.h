#pragma once

#include "CoreMinimal.h"
#include "Characteristic.h"
#include "WeightCharacteristic.generated.h"


UCLASS(Blueprintable)
class BLACKOCEAN_API UWeightCharacteristic : public UCharacteristic
{
	GENERATED_BODY()

protected:

	UWeightCharacteristic() {
		Name = FName("WEIGHT");
		Type = ECharacteristicType::Float;
		Maximum = 1.0f;
		Priority = -1;
	}
};