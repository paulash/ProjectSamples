#pragma once

#include "CoreMinimal.h"
#include "../CarriablePackage.h"
#include "ResourcePack.generated.h"


UCLASS(Blueprintable, BlueprintType)
class BLACKOCEAN_API AResourcePack : public ACarriablePackage
{
	GENERATED_BODY()

protected:

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Meta = (ExposeOnSpawn = true))
	FName ResourceUniqueName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Meta = (ExposeOnSpawn = true))
	int MaxResourceUnits;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Meta = (ExposeOnSpawn = true))
	int CurrentResourceUnits;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	bool AddResource(FName _ResourceUniqueName, float amount);

	UFUNCTION(BlueprintCallable)
	bool RemovedResource(FName _ResourceUniqueName, float amount);

	UFUNCTION(BlueprintPure)
	int GetCurrentResourceUnits() {
		return CurrentResourceUnits;
	}

	UFUNCTION(BlueprintPure)
	int GetMaxResourceUnits() {
		return MaxResourceUnits;
	}

	UFUNCTION(BlueprintPure)
	FName GetResourceUniqueName() {
		return ResourceUniqueName;
	}

};