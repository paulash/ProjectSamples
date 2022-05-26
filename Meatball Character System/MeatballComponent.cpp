#include "MeatballComponent.h"

UMeatballComponent::UMeatballComponent()
{
	bWantsInitializeComponent = true;
}

void UMeatballComponent::OnRegister()
{
	Super::OnRegister();
	character = Cast<ACharacter>(GetOwner());
	RefreshMeatballMeshes();
}

void UMeatballComponent::ApplyMeatballMap(UMeatballMap* meatballMap)
{
	if (!meatballMap) {
		return;
	}
	for (int i = 0; i < meatballMap->mapping.Num(); i++) {
		if (!meatballMap->mapping[i]) {
			continue;
		}
		if (!meatballMap->mapping[i]->bone) {
			continue;
		}
		if (!meatballMap->mapping[i]->staticMesh) {
			continue;
		}

		UStaticMeshComponent* staticMeshComponent = GetMeatballMeshComponent(meatballMap->mapping[i]->bone);
		if (staticMeshComponent) {
			staticMeshComponent->SetStaticMesh(meatballMap->mapping[i]->staticMesh);
		}
	}
}

void UMeatballComponent::RefreshMeatballMeshes()
{
	if (!character) {
		return;
	}
		
	TArray<UStaticMeshComponent*> staticMeshes;
	character->GetComponents(staticMeshes);

	for (int i = 0; i < staticMeshes.Num(); i++) {

		//TODO: might need to check that its related to the meatball maps.
		// check the meatball meshes list?
		if (staticMeshes[i]->GetAttachParent() == character->GetMesh()) {
			staticMeshes[i]->DestroyComponent();
		}
	}

	meatballMeshesByBone.Empty();
	ApplyMeatballMap(defaultMeatballMap);

	for (int i = 0; i < customizations.Num(); i++) {
		if (!customizations[i] || !customizations[i]->bone) {
			continue;
		}

		UStaticMeshComponent* staticMeshComponent = GetMeatballMeshComponent(customizations[i]->bone);
		if (!staticMeshComponent) continue;

		staticMeshComponent->SetStaticMesh(customizations[i]->staticMesh);
	}

	OnRefreshedMeatballMeshes.Broadcast();


	RefreshMaterialSlots();
}


void UMeatballComponent::GetMeatballs(TArray<UStaticMeshComponent*>& meatballs)
{
	for (const TPair<FName, UStaticMeshComponent*>& pair : meatballMeshesByBone) {
		meatballs.Add(pair.Value);
	}
}

UMaterialInstanceDynamic* UMeatballComponent::AssignMaterialSlot(FName MaterialSlot, UMaterial* material)
{
	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(material, this);
	AssignedMaterials.Emplace(MaterialSlot, DynMaterial);
	RefreshMaterialSlots();

	return DynMaterial;
}

void UMeatballComponent::RefreshMaterialSlots()
{
	TArray<FName> MaterialSlots;
	AssignedMaterials.GetKeys(MaterialSlots);

	TMap<FName, TArray<FMaterialSlotDef>> MaterialSlotDefs;
	for (const TPair<FName, UStaticMeshComponent*>& pair : meatballMeshesByBone) {
		TArray<FName> meshSlots = pair.Value->GetMaterialSlotNames();
		for (int i = 0; i < meshSlots.Num(); i++) {
			for (int s = 0; s < MaterialSlots.Num(); s++) {
				if (meshSlots[i].ToString().StartsWith(MaterialSlots[s].ToString())) {
					int materialIndex = pair.Value->GetMaterialIndex(meshSlots[i]);
					MaterialSlotDefs.FindOrAdd(FName(MaterialSlots[s])).Add(FMaterialSlotDef(materialIndex, pair.Value));
				}
			}
		}
	}
	for (const TPair<FName, TArray<FMaterialSlotDef>>& pair : MaterialSlotDefs) {
		UMaterialInstanceDynamic* material = AssignedMaterials.FindRef(pair.Key);
		if (!material) {
			continue;
		}
		for (int i = 0; i < pair.Value.Num(); i++) {
			FMaterialSlotDef matDef = pair.Value[i];
			matDef.staticMesh->SetMaterial(matDef.materialIndex, material);
		}
	}
}

UStaticMeshComponent* UMeatballComponent::GetMeatballMeshComponent(UMeatballBone* bone)
{
	if (!character) return NULL;
	if (!bone) return NULL;
	FName boneName = bone->GetFName();

	if (meatballMeshesByBone.Contains(boneName)) {
		return meatballMeshesByBone.FindRef(boneName);
	}

	UStaticMeshComponent* staticMeshComponent = NewObject<UStaticMeshComponent>(this);
	staticMeshComponent->RegisterComponent();
	staticMeshComponent->AttachToComponent(character->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, bone->targetBone);
	staticMeshComponent->SetRelativeLocation(bone->offset);
	staticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	meatballMeshesByBone.Emplace(boneName, staticMeshComponent);
	return staticMeshComponent;
}
