#include "BatteryPack.h"
#include "Net/UnrealNetwork.h"

void ABatteryPack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABatteryPack, Energy);
}

void ABatteryPack::TickCharge(float rate)
{
	float energyGained = GetWorld()->GetDeltaSeconds() * rate;
	Energy = FMath::Clamp<float>(energyGained + Energy, 0, MaxEnergy);
}

void ABatteryPack::TickDischarge(float rate)
{
	float energyLost = GetWorld()->GetDeltaSeconds() * rate;
	Energy = FMath::Clamp<float>(Energy - energyLost, 0, MaxEnergy);
}

void ABatteryPack::Discharge(float amount)
{
	Energy = FMath::Clamp<float>(Energy - amount, 0, MaxEnergy);
}
