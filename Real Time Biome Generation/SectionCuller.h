#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "SectionCuller.generated.h"

UCLASS(Blueprintable, BlueprintType)
class BLACKOCEAN_API ASectionCuller : public AActor
{
	GENERATED_BODY()

public:

	ASectionCuller();
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type reason) override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* GroundMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CullSolidInstances = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CullUnsolidInstances = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Bounds;

	FVector LastLocation;
	FVector LastBounds;

	UPROPERTY(EditAnywhere)
	UMaterialInstance* GroundMaterial;

	bool Dirty = false;

private:

	bool wasCullSolid = false;
	bool wasCullUnsolid;

	UMaterialInstanceDynamic* DynamicMaterial;
	
};