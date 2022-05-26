#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MeatballBone.generated.h"

/**
 *  Defines a reference between a meatball and a skeletal bone by name.
 */
UCLASS(Blueprintable)
class VESTIGO_API UMeatballBone : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName targetBone;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector offset;
};