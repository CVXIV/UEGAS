// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "EMSCustomSaveGame.h"
#include "CustomSaveGame.generated.h"

UCLASS()
class AURA_API UCustomSaveGame : public UEMSCustomSaveGame {
	GENERATED_BODY()

	UCustomSaveGame();

public:
	UPROPERTY()
	FString PlayerName;

	UPROPERTY()
	int32 PlayerLevel = 0;

	UPROPERTY()
	FVector PlayerLocation;

	UPROPERTY()
	FRotator PlayerOrientation;
};
