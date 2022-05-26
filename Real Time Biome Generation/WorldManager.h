#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "SectionBiome.h"
#include "SectionCuller.h"
#include "WorldManager.generated.h"


#define SECTION_BUCKET_SIZE 1024
#define SECTION_UNIT_SIZE 64
#define WORLD_SIZE 102400
#define SECTION_UPDATES_PER_FRAME 8
#define SECTION_UPDATES_FIRST_BATCH 256
#define SECTION_BIOME_NOISE  256

//102400

class AWorldManager;


USTRUCT(BlueprintType)
struct FSectionBucket
{
	GENERATED_BODY()

public:

	bool Dirty = false;
	FIntPoint BucketPoint;
	TMap<TSubclassOf<USectionInstance>, UInstancedStaticMeshComponent*> ISMs;
	TMap<TSubclassOf<USectionInstance>, TArray<FTransform>> InstanceTransforms;

	UStaticMeshComponent* GroundMeshComponent;

	USectionBiome* SectionBiome;
	int BiomeIndex = -1;

	TArray<TSubclassOf<USectionInstance>> QueuedUpdates;

	AWorldManager* WorldManager;

	FSectionBucket() {
		BucketPoint = FIntPoint(0, 0);
		WorldManager = NULL;
		GroundMeshComponent = NULL;
	}

	FSectionBucket(FIntPoint _BucketPoint, AWorldManager* _WorldManager);

	UInstancedStaticMeshComponent* GetISM(TSubclassOf<USectionInstance> SectionClass);

	void CalculateInstanceTransforms();
	bool Update();
	bool RenderInstanceTransforms(TArray<ASectionCuller*> OverlappingSectionCullers);
	void DirtyInstances();
};

UCLASS(Blueprintable, BlueprintType)
class BLACKOCEAN_API AWorldManager : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* GroundMesh;

	UPROPERTY(EditDefaultsOnly)
	int Seed = 789534952;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<USectionBiome>> SectionBiomes;

	AWorldManager();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void AddSectionBucket(FIntPoint point);

	void RegisterSectionCuller(ASectionCuller* NewCuller);
	void UnregisterSectionCuller(ASectionCuller* OldCuller);
	void UpdateSectionCuller(ASectionCuller* UpdatedCuller);
	TArray<ASectionCuller*> GetOverlappingSectionCullers(FIntPoint SectionBucketPoint);

	UInstancedStaticMeshComponent* CreateISM(TSubclassOf<USectionInstance> SectionInstanceClass);
	UStaticMeshComponent* CreateGroundMesh();

	UFUNCTION(BlueprintPure)
	bool LoadingFirstBatch() {
		return FirstBatch;
	}

	UFUNCTION(BlueprintPure)
	USectionBiome* GetBiomeAtLocation(FVector Location);
	
private:

	void AddSectionCullerToBucket(ASectionCuller* Culler);
	void RemoveSectionCullerToBucket(ASectionCuller* Culler, bool UseLastLocation = false);

	UPROPERTY()
	TMap<FIntPoint, FSectionBucket> Sections;

	TMap<TSubclassOf<USectionBiome>, UInstancedStaticMeshComponent*> BiomeGroundISMs;

	TMap<FIntPoint, TArray<ASectionCuller*>> SectionCullerBucket;
	TSet<ASectionCuller*> SectionCullers;
	TArray<ASectionCuller*> UpdatedSectionCullers;
	TQueue<FIntPoint> DirtySections;

	bool FirstBatch = true;
};