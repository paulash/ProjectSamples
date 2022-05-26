#include "ItemGear.h"
#include "ContainerComponent.h"

AItemGear::AItemGear()
{
	Category = FName("Gear");
	UseTag = FGameplayTag::RequestGameplayTag(FName("Item.Trigger.Equip"));
}

void AItemGear::Equip()
{
	if (!GetOwner()) return;
	UContainerComponent* ContainerComponent = Cast<UContainerComponent>(GetOwner()->GetComponentByClass(UContainerComponent::StaticClass()));
	if (!ContainerComponent) return;
	ContainerComponent->Equip(this);
}

void AItemGear::Unequip()
{
	if (!GetOwner()) return;
	UContainerComponent* ContainerComponent = Cast<UContainerComponent>(GetOwner()->GetComponentByClass(UContainerComponent::StaticClass()));
	if (!ContainerComponent) return;
	ContainerComponent->Unequip(this);
}

ABaseCharacter* AItemGear::GetEquipper()
{
	if (!GetOwner()) return NULL;
	UContainerComponent* ContainerComponent = Cast<UContainerComponent>(GetOwner()->GetComponentByClass(UContainerComponent::StaticClass()));
	if (!ContainerComponent) return NULL;
	return ContainerComponent->IsEquipped(this) ? Cast<ABaseCharacter>(GetOwner()) : NULL;
}

bool AItemGear::IsEquipped()
{
	if (!GetOwner()) return NULL;
	UContainerComponent* ContainerComponent = Cast<UContainerComponent>(GetOwner()->GetComponentByClass(UContainerComponent::StaticClass()));
	if (!ContainerComponent) return NULL;
	return ContainerComponent->IsEquipped(this);
}

void AItemGear::OnTransfered()
{
	Unequip(); // automatically unequip anytime a transfer happens.
}

// implemented in children
void AItemGear::OnEquipped_Implementation(UContainerComponent* Equipper)
{
}

void AItemGear::OnUnequip_Implementation(UContainerComponent* Unequipper)
{
}