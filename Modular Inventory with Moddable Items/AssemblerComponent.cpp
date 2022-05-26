#include "AssemblerComponent.h"
#include "ConnectorComponent.h"
#include "Net/UnrealNetwork.h"

void UAssemblerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAssemblerComponent, QueuedRecipes);
}

UAssemblerComponent::UAssemblerComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
}

void UAssemblerComponent::BeginPlay()
{
	for (int i = 0; i < DefaultRecipes.Num(); i++) {
		Recipes.Add(DefaultRecipes[i]);
	}

	Super::BeginPlay();
}

void UAssemblerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// Must be the server, and have stuff queued, and have an output connected.
	if (GetWorld()->IsServer() && QueuedRecipes.Num() > 0) {
		int ActiveIndex = 0;

		if (InputContainer && OutputContainer) {
			while (QueuedRecipes.IsValidIndex(ActiveIndex)) {
				if (QueuedRecipes[ActiveIndex].RequirementsMet) break;

				QueuedRecipes[ActiveIndex].RequirementsMet = false;
				QueuedRecipes[ActiveIndex].RequirementImpossible = false;

				URecipe* RecipeDefault = Cast<URecipe>(QueuedRecipes[ActiveIndex].RecipeClass->GetDefaultObject());
				bool requirementMet = false;
				for (int i = 0; i < RecipeDefault->Materials.Num(); i++) {
					FMaterialRequirement requirement = RecipeDefault->Materials[i];
					if (requirement.Count > InputContainer->GetItemCount(requirement.ItemClass)) {
						break;
					}
					requirementMet = true;
				}

				if (requirementMet) {
					for (int i = 0; i < RecipeDefault->Materials.Num(); i++) {
						FMaterialRequirement requirement = RecipeDefault->Materials[i];
						InputContainer->RemoveItem(InputContainer->GetItem(requirement.ItemClass), requirement.Count);
					}

					QueuedRecipes[ActiveIndex].RequirementsMet = true;
					break;
				}
				else {
					QueuedRecipes[ActiveIndex].RequirementImpossible = true;
				}
				ActiveIndex++;
			}

			if (QueuedRecipes.IsValidIndex(ActiveIndex)) {
				QueuedRecipes[ActiveIndex].TimeRemaining -= DeltaTime;
				if (QueuedRecipes[ActiveIndex].TimeRemaining <= 0) {
					URecipe* RecipeDefault = Cast<URecipe>(QueuedRecipes[ActiveIndex].RecipeClass->GetDefaultObject());
					AItem* newItem = GetWorld()->SpawnActor<AItem>(RecipeDefault->ResultItemClass);
					OutputContainer->PickupItem(newItem);
					CLIENTS_OnCompletedQueuedRecipe(QueuedRecipes[ActiveIndex].RecipeClass);

					if (QueuedRecipes[ActiveIndex].Count == 1) {
						QueuedRecipes.RemoveAt(ActiveIndex);
					}
					else {
						QueuedRecipes[ActiveIndex].Count--;
						QueuedRecipes[ActiveIndex].TimeRemaining = RecipeDefault->TimeRequired;
						QueuedRecipes[ActiveIndex].RequirementsMet = false;
					}
				}
			}
		}
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

TArray<TSubclassOf<URecipe>> UAssemblerComponent::GetSortedRecipes()
{
	TArray<TSubclassOf<URecipe>> sortedRecipes = TArray<TSubclassOf<URecipe>>(Recipes);
	sortedRecipes.Sort([](const TSubclassOf<URecipe>& ip1, const TSubclassOf<URecipe>& ip2) {
		if (!ip1) return false;
		if (!ip2) return true;
		URecipe* Recipe1 = Cast<URecipe>(ip1->GetDefaultObject());
		URecipe* Recipe2 = Cast<URecipe>(ip2->GetDefaultObject());

		if (!Recipe1->ResultItemClass) return false;
		if (!Recipe2->ResultItemClass) return true;

		AItem* Item1 = Cast<AItem>(Recipe1->ResultItemClass->GetDefaultObject());
		AItem* Item2 = Cast<AItem>(Recipe2->ResultItemClass->GetDefaultObject());

		return Item1->DisplayName.CompareToCaseIgnored(Item2->DisplayName) < 0;
	});
	return sortedRecipes;
}

void UAssemblerComponent::TryCreateRecipeQueue(TSubclassOf<URecipe> RecipeClass, int count/*=1*/)
{
	if (QueuedRecipes.Num() >= MAX_QUEUE_UNIQUE_RECIPE) return;
	if (!Recipes.Contains(RecipeClass)) return;
	if (!RecipeClass) return;
	if (count <= 0) return;

	if (QueuedRecipes.Num() > 0 && QueuedRecipes[QueuedRecipes.Num()-1].RecipeClass == RecipeClass) {
		QueuedRecipes[QueuedRecipes.Num()-1].Count += count;
		return;
	}

	URecipe* RecipeDefault = Cast<URecipe>(RecipeClass->GetDefaultObject());

	FRecipeQueued RecipeQueued;
	RecipeQueued.RecipeClass = RecipeClass;
	RecipeQueued.TimeRemaining = RecipeDefault->TimeRequired;
	RecipeQueued.Count = count;

	QueuedRecipes.Add(RecipeQueued);
	QueuedRecipes = QueuedRecipes;
	OnRep_QueuedRecipes();
}

void UAssemblerComponent::TryCancelRecipeQueue(int index)
{
	if (!QueuedRecipes.IsValidIndex(index)) return;

	if (QueuedRecipes[index].RequirementsMet) {
		if (InputContainer) {
			URecipe* RecipeDefault = Cast<URecipe>(QueuedRecipes[index].RecipeClass->GetDefaultObject());
			for (int i = 0; i < RecipeDefault->Materials.Num(); i++) {
				AItem* MaterialItem = GetWorld()->SpawnActor<AItem>(RecipeDefault->Materials[i].ItemClass);
				MaterialItem->SetCount(RecipeDefault->Materials[i].Count);
				InputContainer->PickupItem(MaterialItem);
			}
		}
	}

	QueuedRecipes.RemoveAt(index);
	QueuedRecipes = QueuedRecipes;
	OnRep_QueuedRecipes();
}

void UAssemblerComponent::TryMoveRecipeQueue(int index, ERecipeQueueMove MoveOperation)
{
	if (!QueuedRecipes.IsValidIndex(index)) return;

	switch (MoveOperation) {

	case ERecipeQueueMove::MoveUpOne:
		if (index == 0) {
			return;
		}
		break;
	case ERecipeQueueMove::MoveDownOne:
		if (index + 1 == QueuedRecipes.Num()) {
			return;
		}
		break;
	}

	FRecipeQueued QueuedCopy = QueuedRecipes[index];
	QueuedRecipes.RemoveAt(index);

	switch (MoveOperation) {

	case ERecipeQueueMove::MoveUpOne:
		QueuedRecipes.Insert(QueuedCopy, index - 1);
		break;
	case ERecipeQueueMove::MoveDownOne:
		QueuedRecipes.Insert(QueuedCopy, index + 1);
		break;
	case ERecipeQueueMove::MoveUpAll:
		QueuedRecipes.Insert(QueuedCopy, 0);
		break;
	case ERecipeQueueMove::MoveDownAll:
		QueuedRecipes.Add(QueuedCopy);
		break;
	}

	QueuedRecipes = QueuedRecipes;
	OnRep_QueuedRecipes();
}

void UAssemblerComponent::OnRep_QueuedRecipes()
{
	OnUpdateQueuedRecipes.Broadcast();
}

void UAssemblerComponent::CLIENTS_OnCompletedQueuedRecipe_Implementation(TSubclassOf<URecipe> CompletedRecipe)
{
	OnCompletedQueuedRecipe.Broadcast(CompletedRecipe);
}
