// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface {
	GENERATED_BODY()
};

class AURA_API IPlayerInterface {
	GENERATED_BODY()

public:
	virtual void AddToXP(uint32 InXP) = 0;

	virtual void AddToAttributePoints(int32 InAttributePoints) = 0;

	virtual void AddToSpellPoints(int32 InSpellPoints) = 0;

	virtual uint32 FindLevelForXP(uint32 XP) = 0;

	virtual uint32 GetAttributePoints() const = 0;

	virtual uint32 GetSpellPoints() const = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HideMagicCircle();
};
