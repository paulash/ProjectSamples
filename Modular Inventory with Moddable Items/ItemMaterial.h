#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "ItemMaterial.generated.h"

UCLASS()
class BLACKOCEAN_API AItemMaterial : public AItem
{
	GENERATED_BODY()

protected:

	AItemMaterial() {
		Category = FName("Material");
	}

};