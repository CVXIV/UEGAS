// Copyright Cvxiv


#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadSlot::SetPlayerName(const FString& InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}

void UMVVM_LoadSlot::SetActiveIndex(int32 InActiveIndex)
{
	UE_MVVM_SET_PROPERTY_VALUE(ActiveIndex, InActiveIndex);
}

void UMVVM_LoadSlot::InitializeSlot() {
	SetActiveIndex(0);
}
