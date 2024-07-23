// Copyright Cvxiv


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/AuraGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	for (int i = 0; i < SlotNums; ++i)
	{
		UMVVM_LoadSlot* Slot = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
		Slot->SlotName = FString::Printf(TEXT("Slot_%d"), i);
		Slot->SetSlotIndex(i);
		Slot->InitializeSlot();
		LoadSlots.Add(i, Slot);
	}
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnteredName)
{
	if (!EnteredName.IsEmpty())
	{
		SetSlotStatusToTaken(Slot, EnteredName);
		const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
		GameMode->SaveSlotData(LoadSlots[Slot]);
	}
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	LoadSlots[Slot]->SetActiveIndex(1);
}

void UMVVM_LoadScreen::SelectButtonPressed(int32 Slot)
{
	SetSelectedSlot(LoadSlots[Slot]);
}

void UMVVM_LoadScreen::DeleteButtonPressed()
{
	const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode->TryDeleteSlotData(CurrentSelectedSlot))
	{
		CurrentSelectedSlot->SetActiveIndex(0);
		SetSelectedSlot(nullptr);
	}
}

void UMVVM_LoadScreen::SetSlotStatusToTaken(int32 SlotIndex, const FString& EnteredName)
{
	LoadSlots[SlotIndex]->SetPlayerName(EnteredName);
	LoadSlots[SlotIndex]->SetActiveIndex(2);
}

void UMVVM_LoadScreen::SetSelectedSlot(UMVVM_LoadSlot* LoadSlot)
{
	CurrentSelectedSlot = LoadSlot;
	OnLoadSlotSelect.Broadcast(CurrentSelectedSlot);
}

void UMVVM_LoadScreen::LoadAllSlotData()
{
	const AAuraGameModeBase* GameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	for (const auto& Pair : LoadSlots)
	{
		if (const ULoadScreenSaveGame* SaveGame = GameModeBase->GetSaveSlotData(Pair.Value))
		{
			SetSlotStatusToTaken(Pair.Key, SaveGame->PlayerName);
		}
	}
}
