#include "ResourcePack.h"
#include "Kismet/GameplayStatics.h"
#include "../../Resources/ResourceManagerComponent.h"
#include "Net/UnrealNetwork.h"

void AResourcePack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AResourcePack, ResourceUniqueName);
	DOREPLIFETIME(AResourcePack, MaxResourceUnits);
	DOREPLIFETIME(AResourcePack, CurrentResourceUnits);
}

void AResourcePack::BeginPlay()
{
	Super::BeginPlay();
	//ResourceManager = Cast<AResourceManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AResourceManager::StaticClass()));
}

bool AResourcePack::AddResource(FName _ResourceUniqueName, float amount)
{
	if (!GetWorld()->IsServer()) return false;
	if (ResourceUniqueName != _ResourceUniqueName && ResourceUniqueName != NAME_None) return false;

	ResourceUniqueName = _ResourceUniqueName;
	if (amount > MaxResourceUnits - CurrentResourceUnits) {
		return false;
	}
	CurrentResourceUnits += amount;
	return true;
}

bool AResourcePack::RemovedResource(FName _ResourceUniqueName, float amount)
{
	if (!GetWorld()->IsServer()) return false;
	if (ResourceUniqueName != _ResourceUniqueName) return false;

	if (amount > CurrentResourceUnits) {
		return false;
	}
	CurrentResourceUnits -= amount;
	return true;
}