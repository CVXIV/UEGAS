// Copyright Cvxiv


#include "AuraAssetManager.h"

#include "AuraGameplayTags.h"

UAuraAssetManager& UAuraAssetManager::Get() {
	check(GEngine)

	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager.Get());
	return *AuraAssetManager;
}

void UAuraAssetManager::StartInitialLoading() {
	Super::StartInitialLoading();

	FAuraGameplayTags::InitializeNativeGameplayTags();
}
