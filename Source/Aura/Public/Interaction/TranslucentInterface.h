// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TranslucentInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTranslucentInterface : public UInterface {
	GENERATED_BODY()
};

class AURA_API ITranslucentInterface {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ActorFadeIn();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ActorFadeOut();
};
