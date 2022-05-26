#pragma once

#include "CoreMinimal.h"
#include "../CarriableSocket.h"
#include "Components/SphereComponent.h"
#include "BatterySocket.generated.h"

class ABatteryPack;

UCLASS(Blueprintable, BlueprintType)
class BLACKOCEAN_API ABatterySocket : public ACarriableSocket
{
	GENERATED_BODY()

public:

	ABatterySocket();

	UPROPERTY(EditAnywhere)
	int Priority = 0;

	UPROPERTY(EditAnywhere)
	USphereComponent* SphereComponent;

	UFUNCTION(BlueprintPure)
	ABatteryPack* GetSocketedBattery();

	virtual void OnInsert_Implementation(AActor* Carriable, bool wasInitial) override;
	virtual void OnEject_Implementation(AActor* Carriable) override;
	virtual FText GetInteractTitle_Implementation(UPrimitiveComponent* InteractionComponent) override;
};