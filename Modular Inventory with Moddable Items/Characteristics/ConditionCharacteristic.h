#pragma once

#include "CoreMinimal.h"
#include "Characteristic.h"
#include "ConditionCharacteristic.generated.h"


UCLASS(Blueprintable)
class BLACKOCEAN_API UConditionCharacteristic : public UCharacteristic
{
	GENERATED_BODY()

protected:

	UConditionCharacteristic() {
		Name = FName("CONDITION");
		Type = ECharacteristicType::Progress;
		Maximum = 1.0f;
		Priority = 20000;
	}
};