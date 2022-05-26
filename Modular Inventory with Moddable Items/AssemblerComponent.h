#pragma once

#include "CoreMinimal.h"
#include "../BlackOcean.h"
#include "BaseStructure.h"
#include "Components/ActorComponent.h"
#include "../Items/Recipe.h"
#include "../Items/Item.h"
#include "../Items/ContainerComponent.h"
#include "AssemblerComponent.generated.h"

#define MAX_QUEUE_UNIQUE_RECIPE 10


UENUM(BlueprintType)
enum class ERecipeQueueMove : uint8
{
	MoveUpOne		UMETA(DisplayName = "MoveUpOne"),
	MoveDownOne		UMETA(DisplayName = "MoveDownOne"),
	MoveUpAll		UMETA(DisplayName = "MoveUpAll"),
	MoveDownAll		UMETA(DisplayName = "MoveDownAll")
};


class UConnectorComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateQueuedRecipes);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCompletedQueuedRecipe, TSubclassOf<URecipe>, CompletedRecipe);

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class BLACKOCEAN_API UAssemblerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UAssemblerComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<URecipe>> DefaultRecipes;

	UPROPERTY(BlueprintAssignable)
	FUpdateQueuedRecipes OnUpdateQueuedRecipes;

	UPROPERTY(BlueprintAssignable)
	FCompletedQueuedRecipe OnCompletedQueuedRecipe;

	UFUNCTION(BlueprintPure)
	TArray<TSubclassOf<URecipe>> GetSortedRecipes();

	UFUNCTION(BlueprintCallable)
	void TryCreateRecipeQueue(TSubclassOf<URecipe> RecipeClass, int count=1);

	UFUNCTION(BlueprintCallable)
	void TryCancelRecipeQueue(int index);

	UFUNCTION(BlueprintCallable)
	void TryMoveRecipeQueue(int index, ERecipeQueueMove MoveOperation);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UContainerComponent* InputContainer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UContainerComponent* OutputContainer;

protected:

	UPROPERTY(Replicated)
	TArray<TSubclassOf<URecipe>> Recipes;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_QueuedRecipes)
	TArray<FRecipeQueued> QueuedRecipes;

	UFUNCTION()
	void OnRep_QueuedRecipes();

	UFUNCTION(NetMulticast, Unreliable)
	void CLIENTS_OnCompletedQueuedRecipe(TSubclassOf<URecipe> CompletedRecipe);
};