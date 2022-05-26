#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GenericTeamAgentInterface.h"
#include "NPCController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateAggroTargetDelegate, AActor*, AggroTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBecomeAggressiveDelegate);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLACKOCEAN_API ANPCController : public AAIController
{
	GENERATED_BODY()


public:

	//ANPCController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	ANPCController(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FOnBecomeAggressiveDelegate OnBecomeAggressive;

	UPROPERTY(BlueprintAssignable)
	FOnUpdateAggroTargetDelegate OnUpdateAggroTarget;

	UFUNCTION(BlueprintCallable)
	void AddThreat(AActor* actor, int amount);

	UFUNCTION(BlueprintCallable)
	void RemoveThreat(AActor* actor, int amount);

	UFUNCTION(BlueprintCallable)
	void ClearThreat(AActor* actor);

	UFUNCTION(BlueprintCallable)
	void ClearAllThreat();

	UFUNCTION(BlueprintPure)
	AActor* GetHighestThreatActor();

protected:

	UAIPerceptionComponent* PerceptionComponent;
	UAISenseConfig_Sight* sightConfig;
	TMap<AActor*, int> threatMap;

	UPROPERTY(BlueprintReadOnly)
	AActor* currentAggroTarget;

private:

	UFUNCTION()
	void OnTargetDestroyed(AActor* destroyedActor);

	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

};