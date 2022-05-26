#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SectionInstance.generated.h"

/**
 *
 */
UCLASS(Blueprintable)
class BLACKOCEAN_API USectionInstance : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* Mesh;

	UPROPERTY(EditDefaultsOnly)
	bool Collision = true;

	UPROPERTY(EditDefaultsOnly)
	FVector2D ScaleRange = FVector2D(1, 1); // randomized scale between x-y

	UPROPERTY(EditDefaultsOnly)
	FVector2D OffsetRange; // offset from center of grid point.

	UPROPERTY(EditDefaultsOnly)
	FVector2D OffsetZRange; // offset from ground of grid point.

	UPROPERTY(EditDefaultsOnly)
	FVector2D SampleWeightRequired = FVector2D(-1, 1); // amount the sample of noise has to be above to spawn this instance.

	UPROPERTY(EditDefaultsOnly)
	int GridSpace = 1;

	UPROPERTY(EditDefaultsOnly)
	float rarity = -1.0f; // rarity required.
};