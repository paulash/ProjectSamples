#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Misc/Guid.h"
#include "Recipe.generated.h"

class AItem;
class URecipe;

USTRUCT(BlueprintType)
struct FMaterialRequirement
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AItem> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int Count = 1;

};

USTRUCT(BlueprintType)
struct FRecipeQueued
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<URecipe> RecipeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TimeRemaining = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int Count = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool RequirementsMet = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool RequirementImpossible = false;
};

UCLASS(Blueprintable)
class BLACKOCEAN_API URecipe : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName Category; // use dots to break down list. Ranged.Rifle

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TimeRequired = 30.0f; // in seconds, using server time.

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FMaterialRequirement> Materials;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AItem> ResultItemClass;
};