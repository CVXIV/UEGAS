// Copyright Cvxiv


#include "Game/AuraGameModeBase.h"

#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void AAuraGameModeBase::SaveSlotData(const UMVVM_LoadSlot* LoadSlot) const
{
	Async(EAsyncExecution::TaskGraph, [LoadSlot, this]()
	{
		TryDeleteSlotData(LoadSlot);
		ULoadScreenSaveGame* SaveGameObj = Cast<ULoadScreenSaveGame>(
			UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass));
		SaveGameObj->PlayerName = LoadSlot->GetPlayerName();

		UGameplayStatics::SaveGameToSlot(SaveGameObj, LoadSlot->SlotName, LoadSlot->GetSlotIndex());
	});
}

bool AAuraGameModeBase::TryDeleteSlotData(const UMVVM_LoadSlot* LoadSlot)
{
	bool bDeleted = false;
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->SlotName, LoadSlot->GetSlotIndex()))
	{
		bDeleted = UGameplayStatics::DeleteGameInSlot(LoadSlot->SlotName, LoadSlot->GetSlotIndex());
	}
	return bDeleted;
}

ULoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const UMVVM_LoadSlot* LoadSlot)
{
	ULoadScreenSaveGame* SaveGameObj = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->SlotName, LoadSlot->GetSlotIndex()))
	{
		SaveGameObj = Cast<ULoadScreenSaveGame>(
			UGameplayStatics::LoadGameFromSlot(LoadSlot->SlotName, LoadSlot->GetSlotIndex()));
	}
	return SaveGameObj;
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
