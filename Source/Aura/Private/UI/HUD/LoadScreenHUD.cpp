// Copyright Cvxiv


#include "UI/HUD/LoadScreenHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/ViewModel/MVVM_LoadScreen.h"
#include "UI/Widget/LoadScreenWidget.h"

void ALoadScreenHUD::BeginPlay() {
	Super::BeginPlay();

	LoadScreenViewModel = NewObject<UMVVM_LoadScreen>(this, LoadScreenViewModelClass);
	LoadScreenViewModel->InitializeLoadSlots();
	
	LoadScreenWidget = CreateWidget<ULoadScreenWidget>(GetWorld(), LoadScreenWidgetClass);
	LoadScreenWidget->AddToViewport();

	// 必须在蓝图初始化之后
	LoadScreenWidget->BlueprintInitializeWidget();
	LoadScreenViewModel->SetSelectedSlot(nullptr);
	
	GetOwningPlayerController()->SetShowMouseCursor(true);
	LoadScreenViewModel->LoadAllSlotData();
}
