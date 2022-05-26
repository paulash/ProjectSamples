#pragma once

#include "CoreMinimal.h"
#include "../CarriablePackage.h"
#include "../../Structures/BaseStructure.h"
#include "StructurePackage.generated.h"


UCLASS(Blueprintable, BlueprintType)
class BLACKOCEAN_API AStructurePackage : public ACarriablePackage
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ABaseStructure> PlacedStructure;

};