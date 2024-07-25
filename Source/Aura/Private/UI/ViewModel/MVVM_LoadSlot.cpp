// Copyright Cvxiv


#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadSlot::SetPlayerName(const FString& InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}

void UMVVM_LoadSlot::SetFormatSlotCreateTime(const FString& InFormatSlotCreateTime)
{
	UE_MVVM_SET_PROPERTY_VALUE(FormatSlotCreateTime, InFormatSlotCreateTime);
}

void UMVVM_LoadSlot::SetSlotCreateTime(const FDateTime& DateTime)
{
	SetFormatSlotCreateTime(DateTime.ToFormattedString(TEXT("%Y-%m-%d %H:%M:%S")));
}

void UMVVM_LoadSlot::SetActiveIndex(int32 InActiveIndex)
{
	UE_MVVM_SET_PROPERTY_VALUE(ActiveIndex, InActiveIndex);
}

void UMVVM_LoadSlot::InitializeSlot()
{
	SetActiveIndex(0);
}
