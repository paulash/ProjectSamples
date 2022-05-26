#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Character.h"
#include "MeatballMap.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MeatballComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRefreshedMeatballMeshes);


USTRUCT()
struct FMaterialSlotDef 
{
	GENERATED_BODY()

public:

	int materialIndex;
	UStaticMeshComponent* staticMesh;

	FMaterialSlotDef() {
		materialIndex = 0;
		staticMesh = NULL;
	}

	FMaterialSlotDef(int nMaterialIndex, UStaticMeshComponent* nStaticMesh) {
		materialIndex = nMaterialIndex;
		staticMesh = nStaticMesh;
	}
};

/**
 *
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VESTIGO_API UMeatballComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	ACharacter* character;

	TMap<FName, UStaticMeshComponent*> meatballMeshesByBone;

	UPROPERTY(EditAnywhere)
	TArray<UMeatballChunk*> customizations;

	TMap<FName, UMaterialInstanceDynamic*> AssignedMaterials;

	void RefreshMaterialSlots();

public:

	UMeatballComponent();

	virtual void OnRegister() override;

	void ApplyMeatballMap(UMeatballMap* meatballMap);

	UFUNCTION()
	void RefreshMeatballMeshes();

	UFUNCTION(BlueprintPure)
	void GetMeatballs(TArray<UStaticMeshComponent*>& meatballs);

	UPROPERTY(BlueprintAssignable)
	FRefreshedMeatballMeshes OnRefreshedMeatballMeshes;

	UFUNCTION(BlueprintCallable)
	UMaterialInstanceDynamic* AssignMaterialSlot(FName MaterialSlot, UMaterial* material);

	UFUNCTION(BlueprintPure)
	UStaticMeshComponent* GetMeatballMeshComponent(UMeatballBone* bone);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMeatballMap* defaultMeatballMap;

};