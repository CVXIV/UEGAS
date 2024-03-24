// Copyright Cvxiv


#include "AuraAssetManager.h"

#include "AbilitySystemGlobals.h"
#include "AuraGameplayTags.h"

UAuraAssetManager& UAuraAssetManager::Get() {
	check(GEngine)

	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager.Get());
	return *AuraAssetManager;
}

void UAuraAssetManager::StartInitialLoading() {
	Super::StartInitialLoading();

	FAuraGameplayTags::InitializeNativeGameplayTags();

	// 5.3自动调用，TargetData需要
	//UAbilitySystemGlobals::Get().InitGlobalData();
}
