#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MeatballBone.h"
#include "MeatballChunk.generated.h"

/**
 *   describes the actual mesh and any other information for a given meatball chunk.
 */
UCLASS(Blueprintable)
class VESTIGO_API UMeatballChunk : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMeatballBone* bone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* staticMesh;

};