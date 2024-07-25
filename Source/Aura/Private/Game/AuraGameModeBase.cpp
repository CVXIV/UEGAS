// Copyright Cvxiv


#include "Game/AuraGameModeBase.h"

#include "EMSFunctionLibrary.h"
#include "Game/CustomSaveGame.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

FSlotInfo AAuraGameModeBase::SaveSlotData(const FString& SlotIndexName, const FString& SlotName) const
{
	UEMSFunctionLibrary::SetCurrentSaveGameName(GetWorld(), SlotIndexName);
	UCustomSaveGame* CustomSaveGame = Cast<UCustomSaveGame>(
		UEMSFunctionLibrary::GetCustomSave(GetWorld(), UCustomSaveGame::StaticClass()));
	CustomSaveGame->SlotName = SlotName;
	UEMSFunctionLibrary::SaveCustom(GetWorld());
	const UEMSInfoSaveGame* InfoSaveGame = UEMSFunctionLibrary::GetNamedSlotInfo(GetWorld(), SlotIndexName);
	return FSlotInfo(InfoSaveGame, CustomSaveGame);
}

bool AAuraGameModeBase::TryDeleteSlotData(const FString& SlotIndexName) const
{
	if (UEMSFunctionLibrary::DoesSaveSlotExist(GetWorld(), SlotIndexName))
	{
		UEMSFunctionLibrary::SetCurrentSaveGameName(GetWorld(), SlotIndexName);
		UEMSFunctionLibrary::DeleteAllSaveDataForSlot(GetWorld(), SlotIndexName);
		return true;
	}
	return false;
}

FSlotInfo AAuraGameModeBase::GetSaveSlotData(const UMVVM_LoadSlot* LoadSlot) const
{
	if (UEMSFunctionLibrary::DoesSaveSlotExist(GetWorld(), LoadSlot->SlotName))
	{
		UEMSFunctionLibrary::SetCurrentSaveGameName(GetWorld(), LoadSlot->SlotName);
		const UEMSInfoSaveGame* InfoSaveGame = UEMSFunctionLibrary::GetNamedSlotInfo(GetWorld(), LoadSlot->SlotName);
		const UCustomSaveGame* SaveGameObj = Cast<UCustomSaveGame>(UEMSFunctionLibrary::GetCustomSave(GetWorld(), UCustomSaveGame::StaticClass()));
		return FSlotInfo(InfoSaveGame, SaveGameObj);
	}
	return FSlotInfo();
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> AllPlayerStart;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), AllPlayerStart);
	if (AllPlayerStart.Num() > 0)
	{
		for (AActor* Actor : AllPlayerStart)
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				if (PlayerStart->PlayerStartTag == FName("GAGA"))
				{
					return PlayerStart;
				}
			}
		}
	}
	return nullptr;
}
