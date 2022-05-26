#include "Rover.h"
#include "WheeledVehicleMovementComponent.h"
#include "BlackOcean.h"
#include "BOPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

ARover::ARover()
{
	CockpitMountComponent = CreateDefaultSubobject<USceneComponent>("CockpitMount");
	CockpitMountComponent->bEditableWhenInherited = true;
	CockpitMountComponent->SetupAttachment(GetMesh());

	CockpitExitComponent = CreateDefaultSubobject<USceneComponent>("CockpitExit");
	CockpitExitComponent->bEditableWhenInherited = true;
	CockpitExitComponent->SetupAttachment(GetMesh());

	PassengerExitComponent = CreateDefaultSubobject<USceneComponent>("PassengerExit");
	PassengerExitComponent->bEditableWhenInherited = true;
	PassengerExitComponent->SetupAttachment(GetMesh());

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystem");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);

	AttributeSet = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("AttributeSet"));

	PassengerMountComponent1 = CreateDefaultSubobject<USceneComponent>("PassengerMount1");
	PassengerMountComponent1->bEditableWhenInherited = true;
	PassengerMountComponent1->SetupAttachment(GetMesh());
	PassengerMountsComponent.Add(PassengerMountComponent1);

	PassengerMountComponent2 = CreateDefaultSubobject<USceneComponent>("PassengerMount2");
	PassengerMountComponent2->bEditableWhenInherited = true;
	PassengerMountComponent2->SetupAttachment(GetMesh());
	PassengerMountsComponent.Add(PassengerMountComponent2);

	PassengerMountComponent3 = CreateDefaultSubobject<USceneComponent>("PassengerMount3");
	PassengerMountComponent3->bEditableWhenInherited = true;
	PassengerMountComponent3->SetupAttachment(GetMesh());
	PassengerMountsComponent.Add(PassengerMountComponent3);

	PassengerMountComponent4 = CreateDefaultSubobject<USceneComponent>("PassengerMount4");
	PassengerMountComponent4->bEditableWhenInherited = true;
	PassengerMountComponent4->SetupAttachment(GetMesh());
	PassengerMountsComponent.Add(PassengerMountComponent4);

}

void ARover::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->RefreshAbilityActorInfo();

	for (int i = 0; i < PassengerSeats; i++) {
		APassengerSeat* newSeat = GetWorld()->SpawnActor<APassengerSeat>(PassengerSeatClass);
		newSeat->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		seats.Add(newSeat);
	}
}

void ARover::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	AttributeSet = NewObject<UBaseAttributeSet>(this, TEXT("AttributeSet"));
}

void ARover::SetInputAbility(TSubclassOf<UGameplayAbility> ability, EAbilityInputID input)
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

void ARover::MoveForward(float axis)
{
	GetVehicleMovementComponent()->SetThrottleInput(axis);
}

void ARover::MoveRight(float axis)
{
	GetVehicleMovementComponent()->SetSteeringInput(axis);
}

float ARover::GetHealth() const
{
	if (IsValid(AttributeSet))
	{
		return AttributeSet->GetHealth();
	}
	return 0.0f;
}

float ARover::GetMaxHealth() const
{
	if (IsValid(AttributeSet))
	{
		return AttributeSet->GetMaxHealth();
	}
	return 0.0f;
}

void ARover::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ARover::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ARover::MoveRight);

	AbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"), FString("CancelTarget"), FString("EAbilityInputID"), static_cast<int32>(EAbilityInputID::Confirm), static_cast<int32>(EAbilityInputID::Cancel)));
}

bool ARover::IsAlive_Implementation()
{
	return GetHealth() > 0.0f;
}

ETeamAttitude::Type ARover::GetAttitudeTowards_Implementation(const AActor* other) const
{
	return ETeamAttitude::Neutral;
}

void ARover::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent) {
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
	SetOwner(NewController);
}

void ARover::StartPilot(ABaseCharacter* newPilot)
{
	if (IsPlayerControlled()) return;

	ABOPlayerController* playerController = Cast<ABOPlayerController>(newPilot->GetController());
	if (!playerController) return;

	playerController->Possess(this);
	playerController->SetViewTargetWithBlend(newPilot);
	playerController->SetViewTargetWithBlend(this, 0.2f);

	newPilot->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	newPilot->SetActorEnableCollision(false);
	newPilot->AttachToComponent(CockpitMountComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	newPilot->MeshHidden = true;
	newPilot->OnRep_MeshHidden();
}

void ARover::StartPassenger(ABaseCharacter* newPassenger)
{
	int emptySeatIndex = INVALID_INDEX;
	for (int i = 0; i < PassengerSeats; i++) {
		if (!seats[i]->IsPlayerControlled()) {
			emptySeatIndex = i;
			break;
		}
	}
	if (emptySeatIndex == INVALID_INDEX) {
		return;
	}

	ABOPlayerController* playerController = Cast<ABOPlayerController>(newPassenger->GetController());
	if (!playerController) return;

	playerController->Possess(seats[emptySeatIndex]);
	playerController->SetViewTargetWithBlend(newPassenger);
	playerController->SetViewTargetWithBlend(this, 0.2f);

	newPassenger->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	newPassenger->SetActorEnableCollision(false);
	newPassenger->AttachToComponent(PassengerMountsComponent[emptySeatIndex], FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	newPassenger->MeshHidden = true;
	newPassenger->OnRep_MeshHidden();
}

void ARover::StopPilot()
{
	ABOPlayerController* playerController = Cast<ABOPlayerController>(GetController());
	if (!playerController) return;
	if (!playerController->PlayerCharacter) return;

	APlayerCharacter* player = playerController->PlayerCharacter;
	player->SetActorEnableCollision(true);
	player->MeshHidden = false;
	player->OnRep_MeshHidden();
	player->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	player->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	playerController->Possess(player);

	SetPlayerState(NULL);

	FTransform cockpitExit = CockpitExitComponent->GetComponentTransform();
	player->TeleportTo(cockpitExit.GetLocation(), cockpitExit.GetRotation().Rotator());
}

void ARover::StopPassenger(APassengerSeat* seat)
{
	ABOPlayerController* playerController = Cast<ABOPlayerController>(seat->GetController());
	if (!playerController) return;
	if (!playerController->PlayerCharacter) return;

	APlayerCharacter* player = playerController->PlayerCharacter;
	player->SetActorEnableCollision(true);
	player->MeshHidden = false;
	player->OnRep_MeshHidden();
	player->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);	
	player->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	playerController->Possess(player);

	seat->SetPlayerState(NULL);

	FTransform passengerExit = PassengerExitComponent->GetComponentTransform();
	player->TeleportTo(passengerExit.GetLocation(), passengerExit.GetRotation().Rotator());
}

void ARover::OnDeath_Implementation(AActor* killer)
{
	if (IsPlayerControlled()) {
		StopPilot();
	}
	for (int i = 0; i < PassengerSeats; i++) {
		if (seats[i]->IsPlayerControlled()) {
			StopPassenger(seats[i]);
		}
	}
}

UAbilitySystemComponent* ARover::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
