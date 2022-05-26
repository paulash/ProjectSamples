#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ICarriable.h"
#include "../IInteractable.h"
#include "Components/StaticMeshComponent.h"
#include "CarriablePackage.generated.h"


UCLASS(Blueprintable, BlueprintType)
class BLACKOCEAN_API ACarriablePackage : public AActor, public ICarriable, public IInteractable
{
	GENERATED_BODY()

public:

	ACarriablePackage();

	UPROPERTY(EditDefaultsOnly)
	FText Title;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UCharacterAbility> PackageAbility;

	// Carriable stuff.
	TSubclassOf<UCharacterAbility> GetAbility_Implementation() override;
	virtual void OnPickedUp_Implementation(AActor* Carrier, bool wasInitial) override;
	virtual void OnDrop_Implementation(AActor* Carrier) override;
	virtual float GetDropHeight_Implementation() override;
	bool CanPickup_Implementation(AActor* Carrier) override;

	// Interactable stuff.
	bool CanInteract_Implementation(UPrimitiveComponent* InteractionComponent, AActor* InteractorActor) override;
	FGameplayTag GetInteractTag_Implementation(UPrimitiveComponent* interactedComponent, AActor* InteractorActor) override;
	FText GetInteractText_Implementation(UPrimitiveComponent* InteractionComponent) override;
	FText GetInteractTitle_Implementation(UPrimitiveComponent* InteractionComponent) override;
};