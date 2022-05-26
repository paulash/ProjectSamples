#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BlackOcean.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Abilities/BaseAttributeSet.h"
#include "PassengerSeat.generated.h"

/**
 *
 */
UCLASS()
class BLACKOCEAN_API APassengerSeat : public APawn
{
	GENERATED_BODY()

public:

	APassengerSeat();

	UFUNCTION(BlueprintCallable)
	void SetInputAbility(TSubclassOf<UGameplayAbility> ability, EAbilityInputID input);

	void PossessedBy(AController* NewController) override;
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAbilitySystemComponent* AbilitySystemComponent;
};