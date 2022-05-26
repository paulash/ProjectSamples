#include "ItemImplant.h"
#include "ContainerComponent.h"

AItemImplant::AItemImplant()
{
	Sockets.Add(FName("IMPLANT"));
}

void AItemImplant::OnEquipped_Implementation(UContainerComponent* Equipper)
{
	ABaseCharacter* Character = Cast<ABaseCharacter>(Equipper->GetOwner());
	if (!Character) return;
	Character->SetInputAbility(Ability1, EAbilityInputID::Ability1);
	Character->SetInputAbility(Ability2, EAbilityInputID::Ability2);
	Character->SetInputAbility(Ability3, EAbilityInputID::Ability3);
}

void AItemImplant::OnUnequip_Implementation(UContainerComponent* Unequipper)
{
	ABaseCharacter* Character = Cast<ABaseCharacter>(Unequipper->GetOwner());
	if (!Character) return;

	Character->ClearInputAbility(EAbilityInputID::Ability1);
	Character->ClearInputAbility(EAbilityInputID::Ability2);
	Character->ClearInputAbility(EAbilityInputID::Ability3);
}

