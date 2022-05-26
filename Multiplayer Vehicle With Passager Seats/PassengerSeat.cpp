#include "PassengerSeat.h"

APassengerSeat::APassengerSeat()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystem");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);
}

void APassengerSeat::SetInputAbility(TSubclassOf<UGameplayAbility> ability, EAbilityInputID input)
{
	if (AbilitySystemComponent && HasAuthority()) {
		FGameplayAbilitySpec* spec = AbilitySystemComponent->FindAbilitySpecFromInputID((int32)input);
		if (spec) {
			AbilitySystemComponent->SetRemoveAbilityOnEnd(spec->Handle);
		}

		if (ability) {
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(ability, 1, (int32)input));
		}
	}
}

void APassengerSeat::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent) {
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
	SetOwner(NewController);
}

void APassengerSeat::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	AbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"), FString("CancelTarget"), FString("EAbilityInputID"), static_cast<int32>(EAbilityInputID::Confirm), static_cast<int32>(EAbilityInputID::Cancel)));
}
