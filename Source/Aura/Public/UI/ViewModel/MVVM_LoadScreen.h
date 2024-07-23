// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadScreen.generated.h"

class UMVVM_LoadSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadSlotSelectSignature, UMVVM_LoadSlot*, LoadSlot);

UCLASS()
class AURA_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void InitializeLoadSlots();

	UFUNCTION(BlueprintPure)
	UMVVM_LoadSlot* GetLoadSlotViewModelByIndex(int32 Index) const;

	void LoadAllSlotData();
	
	void SetSelectedSlot(UMVVM_LoadSlot* LoadSlot);

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlot> LoadSlotViewModelClass;

	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 Slot, const FString& EnteredName);

	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 Slot);

	UFUNCTION(BlueprintCallable)
	void SelectButtonPressed(int32 Slot);

	UFUNCTION(BlueprintCallable)
	void DeleteButtonPressed();

private:
	void SetSlotStatusToTaken(int32 SlotIndex, const FString& EnteredName);

	UPROPERTY(BlueprintAssignable)
	FOnLoadSlotSelectSignature OnLoadSlotSelect;

	UPROPERTY()
	TMap<int32, UMVVM_LoadSlot*> LoadSlots;

	TObjectPtr<UMVVM_LoadSlot> CurrentSelectedSlot;

	const int32 SlotNums = 3;
};
