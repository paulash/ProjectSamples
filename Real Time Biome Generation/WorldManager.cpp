#include "WorldManager.h"
#include "NoisePlugin/Public/FastNoise.h"
#include "DrawDebugHelpers.h"

AWorldManager::AWorldManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWorldManager::BeginPlay()
{
	SectionBiomes.Sort([](const TSubclassOf<USectionBiome> B1, const TSubclassOf<USectionBiome> B2) {
		USectionBiome* BiomeDefault1 = Cast<USectionBiome>(B1->GetDefaultObject());
		USectionBiome* BiomeDefault2 = Cast<USectionBiome>(B2->GetDefaultObject());
		return BiomeDefault1->Priority < BiomeDefault2->Priority;
	});

	int sectionCount = WORLD_SIZE / SECTION_BUCKET_SIZE;
	for (int x = 0; x < sectionCount; x++) {
		for (int y = 0; y < sectionCount; y++) {
			AddSectionBucket(FIntPoint(x, y));
		}
	}
	Super::BeginPlay();
}

void AWorldManager::Tick(float DeltaTime)
{
	int MaxPerFrame = FirstBatch ? SECTION_UPDATES_FIRST_BATCH : SECTION_UPDATES_PER_FRAME;
	FIntPoint SectionPoint;
	while (MaxPerFrame > 0 && DirtySections.Dequeue(SectionPoint)) {
		if (!Sections[SectionPoint].Update()) {
			DirtySections.Enqueue(SectionPoint); // if its not done, re-queue it.
		}
		else {
			Sections[SectionPoint].Dirty = false; // if its done, clear the dirty flag.
		}
		MaxPerFrame--;
	}
	
	if (DirtySections.Peek() == NULL) {
		FirstBatch = false;

		if (UpdatedSectionCullers.Num() != 0) {
			ASectionCuller* UpdatedCuller = UpdatedSectionCullers.Pop();
			if (UpdatedCuller) {
				UpdatedCuller->Dirty = false;
				FIntPoint BucketPoint = FIntPoint(
					FMath::FloorToInt(UpdatedCuller->GetActorLocation().X + (WORLD_SIZE / 2)) / SECTION_BUCKET_SIZE,
					FMath::FloorToInt(UpdatedCuller->GetActorLocation().Y + (WORLD_SIZE / 2)) / SECTION_BUCKET_SIZE
				);
				FIntPoint BucketBounds = FIntPoint(
					FMath::CeilToInt(UpdatedCuller->Bounds.X / SECTION_BUCKET_SIZE),
					FMath::CeilToInt(UpdatedCuller->Bounds.Y / SECTION_BUCKET_SIZE)
				);
				//BucketBounds += FIntPoint(1, 1);

				for (int x = -BucketBounds.X; x <= BucketBounds.X; x++) {
					for (int y = -BucketBounds.Y; y <= BucketBounds.Y; y++) {
						FIntPoint UpdatedSection = BucketPoint + FIntPoint(x, y);
						if (Sections.Contains(UpdatedSection)) {
							if (!Sections[UpdatedSection].Dirty) {

								Sections[UpdatedSection].DirtyInstances();
								DirtySections.Enqueue(UpdatedSection);
								Sections[UpdatedSection].Dirty = true;

							}
							
						}
					}
				}
			}
		}
	}

	Super::Tick(DeltaTime);
}

void AWorldManager::AddSectionBucket(FIntPoint point)
{
	Sections.Add(point, FSectionBucket(point, this));
	if (!Sections[point].Dirty) {
		DirtySections.Enqueue(point);
		Sections[point].Dirty = true;
	}
}

void AWorldManager::RegisterSectionCuller(ASectionCuller* NewCuller)
{
	UpdateSectionCuller(NewCuller);
}

void AWorldManager::UnregisterSectionCuller(ASectionCuller* OldCuller)
{
	UpdateSectionCuller(OldCuller);
}

void AWorldManager::UpdateSectionCuller(ASectionCuller* UpdatedCuller)
{
	RemoveSectionCullerToBucket(UpdatedCuller, true);
	AddSectionCullerToBucket(UpdatedCuller);

	if (!UpdatedCuller->Dirty) {
		UpdatedSectionCullers.Push(UpdatedCuller);
		UpdatedCuller->Dirty = true;
	}
}

TArray<ASectionCuller*> AWorldManager::GetOverlappingSectionCullers(FIntPoint SectionBucketPoint)
{
	return SectionCullerBucket.FindRef(SectionBucketPoint);
}

void AWorldManager::AddSectionCullerToBucket(ASectionCuller* Culler)
{
	FIntPoint BucketPoint = FIntPoint(
		FMath::FloorToInt(Culler->GetActorLocation().X + (WORLD_SIZE / 2)) / SECTION_BUCKET_SIZE,
		FMath::FloorToInt(Culler->GetActorLocation().Y + (WORLD_SIZE / 2)) / SECTION_BUCKET_SIZE
	);

	FIntPoint BucketBounds = FIntPoint(
		FMath::CeilToInt(Culler->Bounds.X / SECTION_BUCKET_SIZE),
		FMath::CeilToInt(Culler->Bounds.Y / SECTION_BUCKET_SIZE)
	);
	//BucketBounds += FIntPoint(1, 1);

	for (int x = -BucketBounds.X; x <= BucketBounds.X; x++) {
		for (int y = -BucketBounds.Y; y <= BucketBounds.Y; y++) {
			FIntPoint WorldBucketPoint = FIntPoint(x, y) + BucketPoint;
			if (!SectionCullerBucket.Contains(WorldBucketPoint)) {
				SectionCullerBucket.Add(WorldBucketPoint, TArray<ASectionCuller*>());
			}
			SectionCullerBucket[WorldBucketPoint].Add(Culler);
		}
	}
}

void AWorldManager::RemoveSectionCullerToBucket(ASectionCuller* Culler, bool UseLastLocation/*=false*/)
{
	FVector Location = Culler->GetActorLocation();
	FVector Bounds = Culler->Bounds;
	if (UseLastLocation) {
		Location = Culler->LastLocation;
		Bounds = Culler->LastBounds;
	}

	FIntPoint BucketPoint = FIntPoint(
		FMath::FloorToInt(Location.X + (WORLD_SIZE / 2)) / SECTION_BUCKET_SIZE,
		FMath::FloorToInt(Location.Y + (WORLD_SIZE / 2)) / SECTION_BUCKET_SIZE
	);

	FIntPoint BucketBounds = FIntPoint(
		FMath::CeilToInt(Bounds.X / SECTION_BUCKET_SIZE),
		FMath::CeilToInt(Bounds.Y / SECTION_BUCKET_SIZE)
	);
	//BucketBounds += FIntPoint(1, 1);

	for (int x = -BucketBounds.X; x <= BucketBounds.X; x++) {
		for (int y = -BucketBounds.Y; y <= BucketBounds.Y; y++) {
			FIntPoint WorldBucketPoint = FIntPoint(x, y) + BucketPoint;
			if (!SectionCullerBucket.Contains(WorldBucketPoint)) {
				continue;
			}
			SectionCullerBucket[WorldBucketPoint].Remove(Culler);
			if (SectionCullerBucket[WorldBucketPoint].Num() == 0) {
				SectionCullerBucket.Remove(WorldBucketPoint);
			}
		}
	}
}

UInstancedStaticMeshComponent* AWorldManager::CreateISM(TSubclassOf<USectionInstance> SectionClass)
{
	USectionInstance* InstanceDefault = Cast<USectionInstance>(SectionClass->GetDefaultObject());
	UInstancedStaticMeshComponent* ISM = NewObject<UInstancedStaticMeshComponent>(this, UInstancedStaticMeshComponent::StaticClass());
	ISM->RegisterComponent();
	ISM->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true));
	ISM->SetStaticMesh(InstanceDefault->Mesh);
	ISM->SetFlags(RF_Transactional);
	ISM->PreAllocateInstancesMemory(1000);
	ISM->SetCullDistance(8000);
	ISM->SetAbsolute(true, true, true);

	if (!InstanceDefault->Collision) {
		ISM->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	AddInstanceComponent(ISM);
	return ISM;
}

UStaticMeshComponent* AWorldManager::CreateGroundMesh()
{
	UStaticMeshComponent* SMC = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());
	SMC->RegisterComponent();
	SMC->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true));
	SMC->SetStaticMesh(GroundMesh);
	SMC->SetFlags(RF_Transactional);
	SMC->SetCullDistance(8000);
	SMC->SetAbsolute(true, true, true);
	SMC->SetWorldScale3D(FVector::OneVector * (SECTION_BUCKET_SIZE / 100) + ((SECTION_UNIT_SIZE * 2) / 100) + ((SECTION_BIOME_NOISE * 2) / 100));
	return SMC;
}

USectionBiome* AWorldManager::GetBiomeAtLocation(FVector Location)
{
	FIntPoint BucketPoint = FIntPoint(
		FMath::FloorToInt(Location.X) / SECTION_BUCKET_SIZE,
		FMath::FloorToInt(Location.Y) / SECTION_BUCKET_SIZE
	);
	FSectionBucket bucket = Sections.FindRef(BucketPoint);
	return bucket.SectionBiome;
}

FSectionBucket::FSectionBucket(FIntPoint _BucketPoint, AWorldManager* _WorldManager)
{
	BucketPoint = _BucketPoint;
	WorldManager = _WorldManager;
	GroundMeshComponent = NULL;
	
	CalculateInstanceTransforms();
}

UInstancedStaticMeshComponent* FSectionBucket::GetISM(TSubclassOf<USectionInstance> SectionClass)
{
	UInstancedStaticMeshComponent* ISM = ISMs.FindRef(SectionClass);
	if (!ISM) {
		ISM = WorldManager->CreateISM(SectionClass);
		ISMs.Add(SectionClass, ISM);
	}
	return ISM;
}

void FSectionBucket::CalculateInstanceTransforms()
{
	FVector WorldPosition = FVector(
		(BucketPoint.X * SECTION_BUCKET_SIZE) - (WORLD_SIZE / 2),
		(BucketPoint.Y * SECTION_BUCKET_SIZE) - (WORLD_SIZE / 2),
		0
	);

	// ensure this is empty, so the clean updates only happen.
	QueuedUpdates.Empty();
	InstanceTransforms.Empty();

	FRandomStream RandomStream;
	RandomStream.Initialize(WorldManager->Seed);

	SectionBiome = NULL;
	for (int b = 0; b < WorldManager->SectionBiomes.Num(); b++) {
		TSubclassOf<USectionBiome> BiomeClass = WorldManager->SectionBiomes[b];
		USectionBiome* BiomeDefault = Cast<USectionBiome>(BiomeClass->GetDefaultObject());

		float mosture = UNoiseBlueprintFunctionLibrary::GetSimplex2D(WorldManager->Seed + 1337, 0.00005f, FVector2D(WorldPosition.X, WorldPosition.Y));
		if (mosture < BiomeDefault->MostureRange.X || mosture > BiomeDefault->MostureRange.Y) {
			continue;
		}
		float elevation = UNoiseBlueprintFunctionLibrary::GetSimplex2D(WorldManager->Seed + 7331, 0.00005f, FVector2D(WorldPosition.X, WorldPosition.Y));
		if (elevation < BiomeDefault->ElevationRange.X || elevation > BiomeDefault->ElevationRange.Y) {
			continue;
		}
		BiomeIndex = b;
		SectionBiome = BiomeDefault;
	}
	if (!SectionBiome) return;

	for (int i = 0; i < SectionBiome->SectionInstances.Num(); i++) {
		TSubclassOf<USectionInstance> SectionClass = SectionBiome->SectionInstances[i];
		USectionInstance* InstanceDefault = Cast<USectionInstance>(SectionClass->GetDefaultObject());
		int unitCount = (SECTION_BUCKET_SIZE / SECTION_UNIT_SIZE) / InstanceDefault->GridSpace;

		for (int x = 0; x < unitCount; x++) {
			for (int y = 0; y < unitCount; y++) {
				FVector Position = FVector(x * InstanceDefault->GridSpace * SECTION_UNIT_SIZE, y * InstanceDefault->GridSpace * SECTION_UNIT_SIZE, 0);
				FVector UnitSize = FVector(
					InstanceDefault->GridSpace * SECTION_UNIT_SIZE,
					InstanceDefault->GridSpace * SECTION_UNIT_SIZE,
					0
				);
				Position += (UnitSize / 2);

				float sample = UNoiseBlueprintFunctionLibrary::GetSimplex2D(WorldManager->Seed, 0.001f * 0.5f, FVector2D(Position.X + WorldPosition.X, Position.Y + WorldPosition.Y));
				if (sample < InstanceDefault->SampleWeightRequired.X || sample > InstanceDefault->SampleWeightRequired.Y) {
					continue;
				}

				if (!InstanceTransforms.Contains(SectionClass)) {
					InstanceTransforms.Add(SectionClass, TArray<FTransform>());
				}

				FQuat Rotation = FRotator(0, RandomStream.FRandRange(0, 360), 0).Quaternion();
				FVector Offset = FVector(
					RandomStream.FRandRange(InstanceDefault->OffsetRange.X, InstanceDefault->OffsetRange.Y),
					RandomStream.FRandRange(InstanceDefault->OffsetRange.X, InstanceDefault->OffsetRange.Y),
					RandomStream.FRandRange(InstanceDefault->OffsetZRange.X, InstanceDefault->OffsetZRange.Y)
				);
				FVector Scale = FVector::OneVector;
				float RandScale = RandomStream.FRandRange(InstanceDefault->ScaleRange.X, InstanceDefault->ScaleRange.Y);
				Scale *= RandScale;

				InstanceTransforms[SectionClass].Add(FTransform(Rotation, Position + Offset, Scale));
			}
		}

		if (InstanceTransforms.Contains(SectionClass)) {
			QueuedUpdates.Add(SectionClass);
		}
	}
}

bool FSectionBucket::Update()
{
	FVector WorldPosition = FVector(
		(BucketPoint.X * SECTION_BUCKET_SIZE) - (WORLD_SIZE / 2),
		(BucketPoint.Y * SECTION_BUCKET_SIZE) - (WORLD_SIZE / 2),
		0
	);

	if (GroundMeshComponent == NULL && SectionBiome) {
		GroundMeshComponent = WorldManager->CreateGroundMesh();
		GroundMeshComponent->SetMaterial(0, SectionBiome->GroundMaterial);
		GroundMeshComponent->SetWorldLocation(WorldPosition + FVector(0, 0, 0.025f * BiomeIndex));
	}
	TArray<ASectionCuller*> OverlappingSectionCullers = WorldManager->GetOverlappingSectionCullers(BucketPoint);

	return RenderInstanceTransforms(OverlappingSectionCullers);
}

bool FSectionBucket::RenderInstanceTransforms(TArray<ASectionCuller*> OverlappingSectionCullers)
{
	// nothing to process. we're done return true.
	if (QueuedUpdates.Num() == 0) return true;

	FVector WorldPosition = FVector(
		(BucketPoint.X * SECTION_BUCKET_SIZE) - (WORLD_SIZE / 2),
		(BucketPoint.Y * SECTION_BUCKET_SIZE) - (WORLD_SIZE / 2),
		0
	);

	TArray<FTransform> Transforms = InstanceTransforms[QueuedUpdates[0]];
	if (Transforms.Num() > 0) {

		USectionInstance* InstanceDefault = Cast<USectionInstance>(QueuedUpdates[0]->GetDefaultObject());
		UInstancedStaticMeshComponent* ISM = GetISM(QueuedUpdates[0]);
		ISM->SetRelativeLocation(WorldPosition);
		ISM->ClearInstances();

		// TODO: check if the overlapping culler is completely contains the section.
		// if it is, we'd just skip the whole transform validation process.

		TArray<ASectionCuller*> ValidCullers;
		for (int o = 0; o < OverlappingSectionCullers.Num(); o++) {
			if (!OverlappingSectionCullers[o]->IsValidLowLevel()) {
				continue;
			}
			if (!OverlappingSectionCullers[o]->CullSolidInstances && InstanceDefault->Collision) {
				continue;
			}
			if (!OverlappingSectionCullers[o]->CullUnsolidInstances && !InstanceDefault->Collision) {
				continue;
			}
			ValidCullers.Add(OverlappingSectionCullers[o]);
		}

		TArray<FTransform> ValidTransforms;
		for (int t = 0; t < Transforms.Num(); t++) {
			FVector Location = Transforms[t].GetLocation() + WorldPosition;
			Location.Z = 0;

			FVector InstanceBounds = FVector(
				InstanceDefault->GridSpace * SECTION_UNIT_SIZE,
				InstanceDefault->GridSpace * SECTION_UNIT_SIZE,
				0
			);
			//InstanceBounds *= Transforms[t].GetScale3D();

			FBox InstanceBox = FBox(Location - (InstanceBounds / 2), Location + (InstanceBounds / 2));
			bool overlapped = false;
			for (int o = 0; o < ValidCullers.Num(); o++) {
				FVector TopLeft = ValidCullers[o]->GetActorLocation() - (ValidCullers[o]->Bounds / 2);
				FVector BottomRight = ValidCullers[o]->GetActorLocation() + (ValidCullers[o]->Bounds / 2);
				FBox CullerBox = FBox(TopLeft, BottomRight);

				if (CullerBox.IntersectXY(InstanceBox)) {
					overlapped = true;
					break;
				}
			}
			if (overlapped) continue;
			ValidTransforms.Add(Transforms[t]);
		}
		ISM->AddInstances(ValidTransforms, false);
	}

	QueuedUpdates.RemoveAt(0);
	return QueuedUpdates.Num() == 0;
}

void FSectionBucket::DirtyInstances()
{
	QueuedUpdates.Empty();
	InstanceTransforms.GenerateKeyArray(QueuedUpdates);
}
