#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "ICombatant.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Abilities/BaseAttributeSet.h"
#include "PlayerCharacter.h"
#include "PassengerSeat.h"
#include "Rover.generated.h"

/**
 *
 */
UCLASS()
class BLACKOCEAN_API ARover : public AWheeledVehicle, public ICombatant, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	int PassengerSeats = 0;


	ARover();
	virtual void BeginPlay() override;
	void PreInitializeComponents();

	UFUNCTION(BlueprintCallable)
	void SetInputAbility(TSubclassOf<UGameplayAbility> ability, EAbilityInputID input);

	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Combatant stuff
	bool IsAlive_Implementation() override;
	ETeamAttitude::Type GetAttitudeTowards_Implementation(const AActor* other) const override;

	void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintCallable)
	void StartPilot(ABaseCharacter* newPilot);

	UFUNCTION(BlueprintCallable)
	void StartPassenger(ABaseCharacter* newPassenger);

	UFUNCTION(BlueprintCallable)
	void StopPilot();

	UFUNCTION(BlueprintCallable)
	void StopPassenger(APassengerSeat* seat);

	virtual void OnDeath_Implementation(AActor* killer) override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<APassengerSeat> PassengerSeatClass;

	UPROPERTY(BlueprintReadOnly)
	TArray<APassengerSeat*> seats;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly)
	UBaseAttributeSet* AttributeSet;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* CockpitMountComponent;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* CockpitExitComponent;

	UPROPERTY(EditDefaultsOnly)
	TArray<USceneComponent*> PassengerMountsComponent;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* PassengerMountComponent1;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* PassengerMountComponent2;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* PassengerMountComponent3;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* PassengerMountComponent4;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* PassengerExitComponent;


	UFUNCTION()
	void MoveForward(float axis);

	UFUNCTION()
	void MoveRight(float axis);

	UFUNCTION(BlueprintPure)
	float GetHealth() const;

	UFUNCTION(BlueprintPure)
	float GetMaxHealth() const;

	

private:

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

};