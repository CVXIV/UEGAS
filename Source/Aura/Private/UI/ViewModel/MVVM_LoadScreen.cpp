// Copyright Cvxiv


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots() {
	for (int i = 0; i < SlotNums; ++i) {
		UMVVM_LoadSlot* Slot = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
		LoadSlots.Add(i, Slot);
	}
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const {
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnteredName) {
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot) {
	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::SelectButtonPressed(int32 Slot) {
}
