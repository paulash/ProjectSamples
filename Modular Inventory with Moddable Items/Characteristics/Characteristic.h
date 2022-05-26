#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Characteristic.generated.h"

UENUM(BlueprintType)
enum class ECharacteristicType : uint8
{
	Tag,
	Float,
	Integer,
	Progress,
	Damage,
	Ammo,
	Hidden
};

UCLASS(Blueprintable)
class BLACKOCEAN_API UCharacteristic : public UObject
{
	GENERATED_BODY()

public:

	FName GetCharacteristicName() {
		return Name;
	}

	float GetPriority() {
		return Priority;
	}

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* Icon = NULL;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ECharacteristicType Type = ECharacteristicType::Tag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Minimum;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Maximum;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Priority;
};