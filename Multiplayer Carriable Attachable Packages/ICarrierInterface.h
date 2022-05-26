#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CarrierComponent.h"
#include "../CharacterAbility.h"
#include "../BlackOcean.h"
#include "ICarrierInterface.generated.h"

UINTERFACE(Blueprintable)
class BLACKOCEAN_API UCarrierInterface : public UInterface
{
	GENERATED_BODY()
};

class BLACKOCEAN_API ICarrierInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	void OnInsert(AActor* Carriable, bool wasInitial);

	UFUNCTION(BlueprintNativeEvent)
	void OnEject(AActor* Carriable);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetInputCarriableAbility(TSubclassOf<UCharacterAbility> ability);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ClearInputCarriableAbility(EAbilityInputID input);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UCarrierComponent* GetCarrierComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USceneComponent* GetAttachmentComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FName GetAttachmentSocket();
};