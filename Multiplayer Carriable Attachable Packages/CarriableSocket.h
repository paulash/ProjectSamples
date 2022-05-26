#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ICarrierInterface.h"
#include "../IInteractable.h"
#include "CarriableSocket.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCarriableSocketUpdated, AActor*, OldCarriable, AActor*, NewCarriable);

UCLASS(Blueprintable, BlueprintType)
class BLACKOCEAN_API ACarriableSocket : public AActor, public IInteractable, public ICarrierInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly)
	UCarrierComponent* CarrierComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	USceneComponent* SocketComponent;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AActor>> AllowedCarriableClasses;

public:

	ACarriableSocket();
	virtual void Destroyed() override;

	// Interactable stuff.
	bool CanInteract_Implementation(UPrimitiveComponent* InteractionComponent, AActor* InteractorActor) override;
	FGameplayTag GetInteractTag_Implementation(UPrimitiveComponent* interactedComponent, AActor* InteractorActor) override;
	FText GetInteractText_Implementation(UPrimitiveComponent* InteractionComponent) override;
	virtual FText GetInteractTitle_Implementation(UPrimitiveComponent* InteractionComponent) override;


	// Carrier stuff.
	void SetInputCarriableAbility_Implementation(TSubclassOf<UCharacterAbility> ability) override;
	void ClearInputCarriableAbility_Implementation(EAbilityInputID input) override;
	UCarrierComponent* GetCarrierComponent_Implementation() override;
	USceneComponent* GetAttachmentComponent_Implementation() override;
	FName GetAttachmentSocket_Implementation() override;

};