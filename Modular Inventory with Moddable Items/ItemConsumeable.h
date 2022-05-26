#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "ItemConsumeable.generated.h"

UCLASS()
class BLACKOCEAN_API AItemConsumeable : public AItem
{
	GENERATED_BODY()

protected:

	AItemConsumeable() {
		Category = FName("Misc");
		UseTag = FGameplayTag::RequestGameplayTag(FName("Item.Trigger.Consume"));
	}

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> ConsumeEffect;
};