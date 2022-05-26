#include "CarriableSocket.h"
#include "Net/UnrealNetwork.h"

ACarriableSocket::ACarriableSocket()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	SocketComponent = CreateDefaultSubobject<USceneComponent>("Socket");
	SocketComponent->SetupAttachment(RootComponent);
	SocketComponent->bEditableWhenInherited = true;

	CarrierComponent = CreateDefaultSubobject<UCarrierComponent>("CarrierComponent");

	bReplicates = true;
}

void ACarriableSocket::Destroyed()
{
	Super::Destroyed();
	if (CarrierComponent->CurrentCarried) {
		CarrierComponent->HandleDropCarriable(CarrierComponent->CurrentCarried);
	}
}

bool ACarriableSocket::CanInteract_Implementation(UPrimitiveComponent* InteractionComponent, AActor* InteractorActor)
{
	ICarrierInterface* CI = Cast<ICarrierInterface>(InteractorActor);
	if (!CI) return false;
	UCarrierComponent* InteractorCarrierComponent = ICarrierInterface::Execute_GetCarrierComponent(InteractorActor);
	if (!InteractorCarrierComponent) return false;
	
	if (CarrierComponent->CurrentCarried && InteractorCarrierComponent->CurrentCarried) return false; // already carrying something.
	if (!CarrierComponent->CurrentCarried && InteractorCarrierComponent->CurrentCarried) { // has item to maybe insert?
		for (int i = 0; i < AllowedCarriableClasses.Num(); i++) {
			if (InteractorCarrierComponent->CurrentCarried->GetClass() == AllowedCarriableClasses[i]) {
				return true;
			}
			if (InteractorCarrierComponent->CurrentCarried->GetClass()->IsChildOf(AllowedCarriableClasses[i])) {
				return true;
			}
		}
	}

	if (CarrierComponent->CurrentCarried && !InteractorCarrierComponent->CurrentCarried) {
		return true; // empty hand, can eject.
	}

	return false;
}

FGameplayTag ACarriableSocket::GetInteractTag_Implementation(UPrimitiveComponent* interactedComponent, AActor* InteractorActor)
{
	return FGameplayTag::RequestGameplayTag(FName(CarrierComponent->CurrentCarried ? "Interact.EjectItem" : "Interact.DepositItem"));
}

FText ACarriableSocket::GetInteractText_Implementation(UPrimitiveComponent* InteractionComponent)
{
	return FText::FromString(CarrierComponent->CurrentCarried ? "Eject" : "Insert");
}

FText ACarriableSocket::GetInteractTitle_Implementation(UPrimitiveComponent* InteractionComponent)
{
	return FText::FromString("Socket");
}

void ACarriableSocket::SetInputCarriableAbility_Implementation(TSubclassOf<UCharacterAbility> ability)
{
	// Sockets dont use abilities.
}

void ACarriableSocket::ClearInputCarriableAbility_Implementation(EAbilityInputID input)
{
	// Sockets dont use abilities.
}

UCarrierComponent* ACarriableSocket::GetCarrierComponent_Implementation()
{
	return CarrierComponent;
}

USceneComponent* ACarriableSocket::GetAttachmentComponent_Implementation()
{
	return SocketComponent;
}

FName ACarriableSocket::GetAttachmentSocket_Implementation()
{
	return NAME_None;
}
