// Copyright Cvxiv


#include "Game/AuraGameModeBase.h"

#include "EMSFunctionLibrary.h"
#include "Game/CustomSaveGame.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void AAuraGameModeBase::SaveSlotData(const FString& SlotName, const FString& PlayerName) const {
	UEMSFunctionLibrary::SetCurrentSaveGameName(GetWorld(), SlotName);
	UCustomSaveGame* CustomSaveGame = Cast<UCustomSaveGame>(UEMSFunctionLibrary::GetCustomSave(GetWorld(), UCustomSaveGame::StaticClass()));
	CustomSaveGame->PlayerName = PlayerName;
	UEMSObject::Get(GetWorld())->SaveCustom(CustomSaveGame);
}

bool AAuraGameModeBase::TryDeleteSlotData(const FString& SlotName) const {
	if (UEMSFunctionLibrary::DoesSaveSlotExist(GetWorld(), SlotName)) {
		UCustomSaveGame* CustomSaveGame = Cast<UCustomSaveGame>(UEMSFunctionLibrary::GetCustomSave(GetWorld(), UCustomSaveGame::StaticClass()));
		UEMSFunctionLibrary::DeleteCustomSave(GetWorld(), CustomSaveGame);
		return true;
	}
	return false;
}

UCustomSaveGame* AAuraGameModeBase::GetSaveSlotData(const UMVVM_LoadSlot* LoadSlot) const {
	UCustomSaveGame* SaveGameObj = nullptr;
	if (UEMSFunctionLibrary::DoesSaveSlotExist(GetWorld(), LoadSlot->SlotName)) {
		return Cast<UCustomSaveGame>(UEMSFunctionLibrary::GetCustomSave(GetWorld(), UCustomSaveGame::StaticClass()));
	}
	return SaveGameObj;
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player) {
	TArray<AActor*> AllPlayerStart;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), AllPlayerStart);
	if (AllPlayerStart.Num() > 0) {
		for (AActor* Actor : AllPlayerStart) {
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor)) {
				if (PlayerStart->PlayerStartTag == FName("GAGA")) {
					return PlayerStart;
				}
			}
		}
	}
	return nullptr;
}
