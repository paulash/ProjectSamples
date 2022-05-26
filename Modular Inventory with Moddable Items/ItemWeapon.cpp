#include "ItemWeapon.h"
#include "Net/UnrealNetwork.h"
#include "../BaseCharacter.h"
#include "ContainerComponent.h"
#include "Characteristics/AmmoCharacteristic.h"

void AItemWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemWeapon, Ammo);
	DOREPLIFETIME(AItemWeapon, MaxAmmo);
	DOREPLIFETIME(AItemWeapon, FireAbilityClass);
	DOREPLIFETIME(AItemWeapon, TacticalAbilityClass);
}

AItemWeapon::AItemWeapon()
{
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("MeshComponent");
	MeshComponent->SetupAttachment(RootComponent);
	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	Sockets.Add(FName("WEAPON"));
	AnimationMode = FGameplayTag::RequestGameplayTag(FName("AnimationMode.Rifle"), false);
}

void AItemWeapon::BeginPlay()
{
	if (GetWorld()->IsServer()) {
		Characteristic.Add(FCharacteristicInstance(UAmmoCharacteristic::StaticClass(), 0));
	}

	Super::BeginPlay();
	Ammo = MaxAmmo;
}

void AItemWeapon::OnRep_Ammo(int oldValue)
{
	OnAmmoChanged.Broadcast(oldValue, Ammo);
}

bool AItemWeapon::ConsumeAmmo()
{
	if (Ammo <= 0) return false;
	int oldAmmo = Ammo;
	Ammo--;
	OnRep_Ammo(Ammo);
	return true;
}

void AItemWeapon::ReloadAmmo()
{
	int needed = MaxAmmo - Ammo;
	if (needed == 0) return;

	ABaseCharacter* Character = Cast<ABaseCharacter>(GetOwner());
	if (!Character) return;
	AItem* AmmoItem = Character->Inventory->GetItem(AmmoItemClass);
	if (!AmmoItem) return;
	int reserveAmmo = AmmoItem->GetCount();
	int oldAmmo = Ammo;
	int removeAmmo = 0;

	// more needed then reserved.
	if (needed > reserveAmmo) {
		Ammo = reserveAmmo;
		removeAmmo = reserveAmmo;
	}
	else {
		Ammo = MaxAmmo;
		removeAmmo = needed;
	}

	Character->Inventory->RemoveItem(AmmoItem, removeAmmo);
	OnRep_Ammo(oldAmmo);
}

bool AItemWeapon::IsStackable_Implementation(AItem* OtherItem)
{
	return false; // weapons never stack.
}

void AItemWeapon::OnEquipped_Implementation(UContainerComponent* Equipper)
{
	ABaseCharacter* Character = Cast<ABaseCharacter>(Equipper->GetOwner());
	if (!Character) return;

	AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponSocket"));

	UpdateFireAbility(true, FireAbilityClass);
	UpdateFireAbility(false, TacticalAbilityClass);

	GetRootComponent()->SetVisibility(true, true);

	Character->AbilitySystemComponent->AddLooseGameplayTags(AssignedHolderTags);
}

void AItemWeapon::OnUnequip_Implementation(UContainerComponent* Unequipper)
{
	ABaseCharacter* Character = Cast<ABaseCharacter>(Unequipper->GetOwner());
	if (!Character) return;

	Character->ClearInputAbility(EAbilityInputID::Fire);
	Character->ClearInputAbility(EAbilityInputID::FireAlternate);

	GetRootComponent()->SetVisibility(false, true);
	ItemPreviewContainer->SetVisibility(true, true);

	Character->AbilitySystemComponent->RemoveLooseGameplayTags(AssignedHolderTags);
}

void AItemWeapon::UpdateFireAbility(bool primary, TSubclassOf<UCharacterAbility> AbilityClass)
{
	ABaseCharacter* Character = Cast<ABaseCharacter>(GetOwner());
	if (!Character) return;

	Character->ClearInputAbility(primary ? EAbilityInputID::Fire : EAbilityInputID::FireAlternate);
	Character->SetInputAbility(AbilityClass, primary ? EAbilityInputID::Fire : EAbilityInputID::FireAlternate);

	if (primary) {
		FireAbilityClass = AbilityClass;
		OnRep_UpdateFireAbilityClass();
	}
	else {
		TacticalAbilityClass = AbilityClass;
		OnRep_UpdateTacticalAbilityClass();
	}
}

void AItemWeapon::OnRep_UpdateFireAbilityClass()
{
	OnUpdateFireAbility.Broadcast(true);
}

void AItemWeapon::OnRep_UpdateTacticalAbilityClass()
{
	OnUpdateFireAbility.Broadcast(false);
}
