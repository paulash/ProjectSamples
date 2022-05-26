#include "BatterySocket.h"
#include "../Packages/BatteryPack.h"

ABatterySocket::ABatterySocket()
{
	AllowedCarriableClasses.Add(ABatteryPack::StaticClass());

	SphereComponent = CreateDefaultSubobject<USphereComponent>("Sphere");
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->bEditableWhenInherited = true;
	SphereComponent->SetCollisionProfileName(FName("Interactable"));
}

ABatteryPack* ABatterySocket::GetSocketedBattery()
{
	return Cast<ABatteryPack>(CarrierComponent->CurrentCarried);
}

void ABatterySocket::OnInsert_Implementation(AActor* Carriable, bool wasInitial)
{

}

void ABatterySocket::OnEject_Implementation(AActor* Carriable)
{

}

FText ABatterySocket::GetInteractTitle_Implementation(UPrimitiveComponent* InteractionComponent)
{
	ABatteryPack* BatteryPack = Cast<ABatteryPack>(CarrierComponent->CurrentCarried);
	if (!BatteryPack) {
		return FText::FromString(TEXT("Battery Socket [EMPTY]"));
	}
	else {
		return FText::FromString(FString::Printf(TEXT("Battery Socket [%dgw]"), FMath::FloorToInt(BatteryPack->Energy)));
	}
}
