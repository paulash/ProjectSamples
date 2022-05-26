#include "NPCController.h"
#include "BaseCharacter.h"
#include "ICombatant.h"
#include "Navigation/CrowdFollowingComponent.h"

ANPCController::ANPCController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent"))) {

	// Setup the perception component
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception Component"));
	sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));

	sightConfig->SightRadius = 800;
	sightConfig->LoseSightRadius = sightConfig->SightRadius + 100;
	sightConfig->PeripheralVisionAngleDegrees = 360.0f;

	sightConfig->DetectionByAffiliation.bDetectEnemies = true;
	sightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	sightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	PerceptionComponent->ConfigureSense(*sightConfig);
	PerceptionComponent->SetDominantSense(sightConfig->GetSenseImplementation());
}

void ANPCController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AActor* highestThreat = GetHighestThreatActor();
	if (!highestThreat) {
		TArray<AActor*> seenActors;
		PerceptionComponent->GetCurrentlyPerceivedActors(UAISenseConfig_Sight::StaticClass(), seenActors);
		if (seenActors.Num() == 0) {
			return;
		}
		// add aggro for any seen actors, if we have no highest threat.
		AddThreat(seenActors[0], 1);
		OnBecomeAggressive.Broadcast();
	}
	if (currentAggroTarget == highestThreat) {
		return; // already aggroing this target.
	}

	currentAggroTarget = highestThreat;
	OnUpdateAggroTarget.Broadcast(currentAggroTarget);
}

void ANPCController::AddThreat(AActor* actor, int amount)
{
	if (!GetPawn()) return;
	if (ICombatant::Execute_GetAttitudeTowards(GetPawn(), actor) != ETeamAttitude::Hostile) {
		return;
	}

	if (threatMap.Contains(actor)) {
		threatMap[actor] += amount;
	}
	else {
		threatMap.Add(actor, amount);
		actor->OnDestroyed.AddDynamic(this, &ANPCController::OnTargetDestroyed);
	}
}

void ANPCController::RemoveThreat(AActor* actor, int amount)
{
	if (threatMap.Contains(actor)) {

		if (threatMap[actor] - amount <= 0) {
			threatMap.Remove(actor);
			actor->OnDestroyed.RemoveDynamic(this, &ANPCController::OnTargetDestroyed);
		}
		else {
			threatMap[actor] -= amount;
		}
	}
}

void ANPCController::ClearThreat(AActor* actor)
{
	if (threatMap.Contains(actor)) {
		threatMap.Remove(actor);
	}
}

void ANPCController::ClearAllThreat()
{
	threatMap.Empty();
}

AActor* ANPCController::GetHighestThreatActor()
{
	AActor* highestThreatActor = NULL;
	int highestThreatAmount = 0;

	for (const TPair<AActor*, int> pair : threatMap) {
		if (pair.Value > highestThreatAmount) {
			highestThreatAmount = pair.Value;
			highestThreatActor = pair.Key;
		}
	}

	return highestThreatActor;
}

void ANPCController::OnTargetDestroyed(AActor* destroyedActor)
{
	ClearThreat(destroyedActor);
}

ETeamAttitude::Type ANPCController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (!GetPawn()) return ETeamAttitude::Neutral;
	return ICombatant::Execute_GetAttitudeTowards(GetPawn(), &Other);
}