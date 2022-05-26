#include "CarrierComponent.h"
#include "ICarriable.h"
#include "ICarrierInterface.h"
#include "../BlackOcean.h"
#include "Net/UnrealNetwork.h"

void UCarrierComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UCarrierComponent, CurrentCarried, COND_InitialOnly);
}

UCarrierComponent::UCarrierComponent()
{
	SetIsReplicatedByDefault(true);
}

void UCarrierComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCarrierComponent::PickUp(AActor* newCarriable)
{
	if (!GetWorld()->IsServer()) return;
	if (CurrentCarried) return;

	CLIENTS_PickUp(newCarriable);
}

void UCarrierComponent::Drop()
{
	if (!GetWorld()->IsServer()) return;
	if (!CurrentCarried) return;

	CLIENTS_Drop(CurrentCarried);
}

void UCarrierComponent::Transfer(AActor* newCarrier)
{	
	if (!GetWorld()->IsServer()) return;
	if (!CurrentCarried) return;
	if (!newCarrier) return;
	ICarrierInterface* CI = Cast<ICarrierInterface>(newCarrier);
	if (!CI) return;

	CLIENTS_Transfer(newCarrier);
}

void UCarrierComponent::CLIENTS_PickUp_Implementation(AActor* NewCarriable)
{
	if (CurrentCarried) return;

	HandleAttachCarriable(NewCarriable, false);
	CurrentCarried = NewCarriable;
}

void UCarrierComponent::CLIENTS_Drop_Implementation(AActor* DropCarriable)
{
	if (DropCarriable != CurrentCarried) return;
	HandleDropCarriable(DropCarriable);
	CurrentCarried = NULL;
}

void UCarrierComponent::CLIENTS_Transfer_Implementation(AActor* NewCarrier)
{
	if (!CurrentCarried) return;
	if (!NewCarrier) return;

	HandleTransferCarriable(NewCarrier);
	CurrentCarried = NULL;
}

void UCarrierComponent::OnRep_CurrentCarried()
{
	if (CurrentCarried) {
		HandleAttachCarriable(CurrentCarried, true);
	}
}

void UCarrierComponent::OnDestroyedCarriable(AActor* DestroyedActor)
{
	if (DestroyedActor != CurrentCarried) return;
	CurrentCarried = NULL;
	HandleDetachCarriable(DestroyedActor);
}

void UCarrierComponent::HandleAttachCarriable(AActor* NewCarried, bool wasInitial)
{
	ICarrierInterface::Execute_SetInputCarriableAbility(GetOwner(), ICarriable::Execute_GetAbility(NewCarried));
	NewCarried->OnDestroyed.AddDynamic(this, &UCarrierComponent::OnDestroyedCarriable);
	NewCarried->SetActorEnableCollision(false);

	NewCarried->AttachToComponent(
		ICarrierInterface::Execute_GetAttachmentComponent(GetOwner()), 
		FAttachmentTransformRules::SnapToTargetNotIncludingScale, 
		ICarrierInterface::Execute_GetAttachmentSocket(GetOwner())
	);
	ICarriable::Execute_OnPickedUp(NewCarried, GetOwner(), wasInitial);
	ICarrierInterface::Execute_OnInsert(GetOwner(), NewCarried, wasInitial);

	OnUpdateCarriable.Broadcast(NewCarried);
}

void UCarrierComponent::HandleDetachCarriable(AActor* DroppedCarriable)
{
	if (!DroppedCarriable) return;

	for (int i = 0; i < CARRIABLE_ABILITY_MAX_COUNT; i++) {
		EAbilityInputID input = (EAbilityInputID)((int)EAbilityInputID::Ability1 + i);
		ICarrierInterface::Execute_ClearInputCarriableAbility(GetOwner(), input);
	}
	DroppedCarriable->OnDestroyed.RemoveDynamic(this, &UCarrierComponent::OnDestroyedCarriable);

	ICarrierInterface::Execute_OnEject(GetOwner(), DroppedCarriable);
	OnUpdateCarriable.Broadcast(NULL);
	// assumes the actual detachment will occur elsewhere, by result of drop or transfer.
}

void UCarrierComponent::HandleDropCarriable(AActor* DroppedCarriable)
{
	if (!DroppedCarriable) return;
	HandleDetachCarriable(DroppedCarriable);

	// detach from carrier.
	DroppedCarriable->SetOwner(NULL);
	DroppedCarriable->SetActorEnableCollision(true);
	DroppedCarriable->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	FVector targetPoint = GetOwner()->GetActorLocation() + (GetOwner()->GetActorForwardVector() * 100);
	/*
	targetPoint.Z = ICarriable::Execute_GetDropHeight(DroppedCarriable);
	if (targetPoint.Z == 0) {
		FVector Origin, Extends;
		DroppedCarriable->GetActorBounds(true, Origin, Extends);
		targetPoint.Z = Extends.Z / 2;
	}
	*/
	targetPoint.Z = 0;
	DroppedCarriable->TeleportTo(targetPoint, GetOwner()->GetActorRotation());

	ICarriable::Execute_OnDrop(DroppedCarriable, GetOwner());
}

void UCarrierComponent::HandleTransferCarriable(AActor* newCarrier)
{
	UCarrierComponent* newCarrierComponent = ICarrierInterface::Execute_GetCarrierComponent(newCarrier);
	if (!newCarrierComponent) return;

	HandleDetachCarriable(CurrentCarried);
	newCarrierComponent->HandleAttachCarriable(CurrentCarried, false);
	newCarrierComponent->CurrentCarried = CurrentCarried;
}