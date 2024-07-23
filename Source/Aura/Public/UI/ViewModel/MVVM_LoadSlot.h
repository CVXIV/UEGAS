// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitcherIndex);

UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	FString GetPlayerName() const
	{
		return PlayerName;
	}

	void SetPlayerName(const FString& InPlayerName);

	int32 GetActiveIndex() const
	{
		return ActiveIndex;
	}

	void SetActiveIndex(int32 InActiveIndex);

	int32 GetSlotIndex() const
	{
		return SlotIndex;
	}

	void SetSlotIndex(int32 InSlotIndex)
	{
		SlotIndex = InSlotIndex;
	}

	UPROPERTY()
	FString SlotName;

	void InitializeSlot();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter)
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 ActiveIndex;

	UPROPERTY(BlueprintReadOnly)
	int32 SlotIndex;
};
