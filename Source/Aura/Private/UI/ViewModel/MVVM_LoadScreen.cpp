// Copyright Cvxiv


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "EMSInfoSaveGame.h"
#include "Game/AuraGameModeBase.h"
#include "Game/CustomSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots() {
	for (int i = 0; i < SlotNums; ++i) {
		UMVVM_LoadSlot* Slot = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
		Slot->SlotName = FString::Printf(TEXT("Slot_%d"), i);
		Slot->SetSlotIndex(i);
		Slot->InitializeSlot();
		LoadSlots.Add(i, Slot);
	}
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const {
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnteredName) {
	if (!EnteredName.IsEmpty()) {
		const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
		const FSlotInfo& SlotInfo = GameMode->SaveSlotData(LoadSlots[Slot]->SlotName, EnteredName);
		SetSlotStatusToTaken(Slot, EnteredName, SlotInfo.InfoSaveGame->SlotInfo.TimeStamp);
	}
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot) {
	LoadSlots[Slot]->SetActiveIndex(1);
}

void UMVVM_LoadScreen::SelectButtonPressed(int32 Slot) {
	SetSelectedSlot(LoadSlots[Slot]);
}

void UMVVM_LoadScreen::DeleteButtonPressed() {
	const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode->TryDeleteSlotData(CurrentSelectedSlot->SlotName)) {
		SetSlotStatusToVacant(CurrentSelectedSlot->GetSlotIndex());
		SetSelectedSlot(nullptr);
	}
}

void UMVVM_LoadScreen::SetSlotStatusToTaken(int32 SlotIndex, const FString& EnteredName, const FDateTime& DateTime) {
	LoadSlots[SlotIndex]->SetPlayerName(EnteredName);
	LoadSlots[SlotIndex]->SetSlotCreateTime(DateTime);
	LoadSlots[SlotIndex]->SetActiveIndex(2);
}

void UMVVM_LoadScreen::SetSlotStatusToVacant(int32 SlotIndex)
{
	LoadSlots[SlotIndex]->SetPlayerName("");
	LoadSlots[SlotIndex]->SetActiveIndex(0);	
}

void UMVVM_LoadScreen::SetSelectedSlot(UMVVM_LoadSlot* LoadSlot) {
	CurrentSelectedSlot = LoadSlot;
	OnLoadSlotSelect.Broadcast(CurrentSelectedSlot);
}

void UMVVM_LoadScreen::LoadAllSlotData() {
	const AAuraGameModeBase* GameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	for (const auto& Pair : LoadSlots) {
		const FSlotInfo& SlotInfo = GameModeBase->GetSaveSlotData(Pair.Value);
		if (SlotInfo.InfoSaveGame && SlotInfo.CustomSaveGame) {
			SetSlotStatusToTaken(Pair.Key, SlotInfo.CustomSaveGame->SlotName, SlotInfo.InfoSaveGame->SlotInfo.TimeStamp);
		}
	}
}
