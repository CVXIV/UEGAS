// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicCircle.generated.h"

UCLASS()
class AURA_API AMagicCircle : public AActor {
	GENERATED_BODY()

public:
	AMagicCircle();

	void SetDecalMaterial(UMaterialInterface* MaterialInterface) const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UDecalComponent> MagicCircleDecal;
};
