#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MeatballChunk.h"
#include "MeatballBone.h"
#include "MeatballMap.generated.h"

/**
 * creates an override mapping for meatball bones, to chunks.
 */
UCLASS(Blueprintable)
class VESTIGO_API UMeatballMap : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UMeatballChunk*> mapping;
};