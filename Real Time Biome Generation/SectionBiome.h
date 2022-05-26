#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SectionInstance.h"
#include "SectionBiome.generated.h"

/**
 *
 */
UCLASS(Blueprintable)
class BLACKOCEAN_API USectionBiome : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	FText BiomeName;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInstance* GroundMaterial;

	UPROPERTY(EditDefaultsOnly)
	float Priority = 0;

	UPROPERTY(EditDefaultsOnly)
	FVector2D MostureRange;

	UPROPERTY(EditDefaultsOnly)
	FVector2D ElevationRange;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<USectionInstance>> SectionInstances;

};