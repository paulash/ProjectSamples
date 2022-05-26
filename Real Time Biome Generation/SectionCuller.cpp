#include "SectionCuller.h"
#include "Kismet/GameplayStatics.h"
#include "WorldManager.h"
#include "DrawDebugHelpers.h"

ASectionCuller::ASectionCuller()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	GroundMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("GroundMesh");
	GroundMeshComponent->SetupAttachment(RootComponent);
	GroundMeshComponent->bEditableWhenInherited = true;
	

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ASectionCuller::BeginPlay()
{
	DynamicMaterial = UMaterialInstanceDynamic::Create(GroundMaterial, this);
	GroundMeshComponent->SetMaterial(0, DynamicMaterial);

	GroundMeshComponent->SetRelativeLocation(FVector(0, 0, 1));
	GroundMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LastLocation = GetActorLocation();
	LastBounds = Bounds;
	wasCullSolid = CullSolidInstances;
	wasCullUnsolid = CullUnsolidInstances;

	DynamicMaterial->SetScalarParameterValue(FName("Width"), Bounds.X);
	DynamicMaterial->SetScalarParameterValue(FName("Depth"), Bounds.Y);
	GroundMeshComponent->SetWorldScale3D(FVector(
		(Bounds.X / 100) + ((SECTION_UNIT_SIZE * 2) / 100) + ((SECTION_BIOME_NOISE * 2) / 100),
		(Bounds.Y / 100) + ((SECTION_UNIT_SIZE * 2) / 100) + ((SECTION_BIOME_NOISE * 2) / 100),
		0.01));

	AWorldManager* WorldManager = Cast<AWorldManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AWorldManager::StaticClass()));
	if (WorldManager) {
		WorldManager->RegisterSectionCuller(this);
	}
	Super::BeginPlay();
}

void ASectionCuller::EndPlay(EEndPlayReason::Type reason)
{
	AWorldManager* WorldManager = Cast<AWorldManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AWorldManager::StaticClass()));
	if (WorldManager) {
		WorldManager->UnregisterSectionCuller(this);
	}
	Super::EndPlay(reason);
}

void ASectionCuller::Tick(float DeltaTime)
{
	FVector Location = GetActorLocation();
	Location.Z = 0;
	SetActorLocation(Location);

	if (FVector::Dist2D(LastLocation, GetActorLocation()) > SECTION_UNIT_SIZE) {
		AWorldManager* WorldManager = Cast<AWorldManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AWorldManager::StaticClass()));
		if (WorldManager) {
			WorldManager->UpdateSectionCuller(this);
		}
		LastLocation = GetActorLocation();
	}

	if (FVector::Dist2D(LastBounds, Bounds) > SECTION_UNIT_SIZE) {
		AWorldManager* WorldManager = Cast<AWorldManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AWorldManager::StaticClass()));
		if (WorldManager) {
			WorldManager->UpdateSectionCuller(this);
		}
		LastBounds = Bounds;

		DynamicMaterial->SetScalarParameterValue(FName("Width"), Bounds.X);
		DynamicMaterial->SetScalarParameterValue(FName("Height"), Bounds.Y);
		GroundMeshComponent->SetWorldScale3D(FVector::OneVector * (SECTION_BUCKET_SIZE / 100) + ((SECTION_UNIT_SIZE * 2) / 100) + ((SECTION_BIOME_NOISE * 2) / 100));
	}

	if (wasCullSolid != CullSolidInstances) {
		wasCullSolid = CullSolidInstances;
		AWorldManager* WorldManager = Cast<AWorldManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AWorldManager::StaticClass()));
		if (WorldManager) {
			WorldManager->UpdateSectionCuller(this);
		}
	}

	if (wasCullUnsolid != CullUnsolidInstances) {
		wasCullUnsolid = CullUnsolidInstances;
		AWorldManager* WorldManager = Cast<AWorldManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AWorldManager::StaticClass()));
		if (WorldManager) {
			WorldManager->UpdateSectionCuller(this);
		}
	}

	Super::Tick(DeltaTime);
}

