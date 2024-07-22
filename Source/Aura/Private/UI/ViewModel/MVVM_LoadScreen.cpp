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
