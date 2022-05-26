#pragma once

#include "CoreMinimal.h"
#include "Characteristic.h"
#include "AmmoCharacteristic.generated.h"

UCLASS(Blueprintable)
class BLACKOCEAN_API UAmmoCharacteristic : public UCharacteristic
{
	GENERATED_BODY()

protected:

	UAmmoCharacteristic() {
		Name = FName("AMMO");
		Type = ECharacteristicType::Ammo;
		Maximum = 0.0f;
		Priority = 20001;
	}
};