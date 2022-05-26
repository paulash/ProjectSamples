#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CarrierComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateCarriable, AActor*, Carriable);

UCLASS(Blueprintable, BlueprintType)
class BLACKOCEAN_API UCarrierComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UCarrierComponent();
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentCarried, BlueprintReadOnly)
	AActor* CurrentCarried;

	UPROPERTY(BlueprintAssignable)
	FUpdateCarriable OnUpdateCarriable;

	UFUNCTION(BlueprintCallable)
	void PickUp(AActor* newCarriable);

	UFUNCTION(BlueprintCallable)
	void Drop();

	UFUNCTION(BlueprintCallable)
	void Transfer(AActor* newCarrier);

protected:

	UFUNCTION(NetMulticast, Reliable)
	void CLIENTS_PickUp(AActor* NewCarriable);

	UFUNCTION(NetMulticast, Reliable)
	void CLIENTS_Drop(AActor* DropCarriable);

	UFUNCTION(NetMulticast, Reliable)
	void CLIENTS_Transfer(AActor* NewCarrier);

	UFUNCTION()
	void OnRep_CurrentCarried();

	UFUNCTION()
	void OnDestroyedCarriable(AActor* DestroyedActor);

	void HandleAttachCarriable(AActor* NewCarried, bool wasInitial);
	void HandleDetachCarriable(AActor* DroppedCarriable);
	void HandleTransferCarriable(AActor* newCarrier);

public:

	void HandleDropCarriable(AActor* DroppedCarriable);
};