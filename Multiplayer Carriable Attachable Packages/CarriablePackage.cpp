#include "CarriablePackage.h"
#include "ICarrierInterface.h"

ACarriablePackage::ACarriablePackage()
{
	bReplicates = true;
	SetReplicateMovement(true);
}

TSubclassOf<UCharacterAbility> ACarriablePackage::GetAbility_Implementation()
{
	return PackageAbility;
}

void ACarriablePackage::OnPickedUp_Implementation(AActor* Carrier, bool wasInitial)
{
}

void ACarriablePackage::OnDrop_Implementation(AActor* Carrier)
{
}

float ACarriablePackage::GetDropHeight_Implementation()
{
	return 0;
}

bool ACarriablePackage::CanPickup_Implementation(AActor* Carrier)
{
	ICarrierInterface* CI = Cast<ICarrierInterface>(Carrier);
	if (!CI) return false;
	UCarrierComponent* InteractorCarrierComponent = ICarrierInterface::Execute_GetCarrierComponent(Carrier);
	if (!InteractorCarrierComponent) return false;

	return InteractorCarrierComponent->CurrentCarried == NULL;
}

bool ACarriablePackage::CanInteract_Implementation(UPrimitiveComponent* InteractionComponent, AActor* InteractorActor)
{
	ICarrierInterface* CI = Cast<ICarrierInterface>(InteractorActor);
	if (!CI) return false;
	UCarrierComponent* InteractorCarrierComponent = ICarrierInterface::Execute_GetCarrierComponent(InteractorActor);
	if (!InteractorCarrierComponent) return false;

	return InteractorCarrierComponent->CurrentCarried == NULL;
}

FGameplayTag ACarriablePackage::GetInteractTag_Implementation(UPrimitiveComponent* interactedComponent, AActor* InteractorActor)
{
	return FGameplayTag::RequestGameplayTag(FName("Interact.Pickup"));
}

FText ACarriablePackage::GetInteractText_Implementation(UPrimitiveComponent* InteractionComponent)
{
	return FText::FromString(TEXT("Pickup"));
}

FText ACarriablePackage::GetInteractTitle_Implementation(UPrimitiveComponent* InteractionComponent)
{
	return Title;
}

