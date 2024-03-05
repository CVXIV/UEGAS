// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

/**
 *
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface {
	GENERATED_BODY()

public:
	AAuraEnemy();

	virtual void HighlightActor() override;

	virtual void UnHighlightActor() override;

	virtual int32 GetPlayerLevel() override;

protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo() override;

private:
	// Enemy的Level不需要网络传递，因为涉及到的关键计算只在服务器完成，没有必要传递到客户端
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults", meta = (AllowPrivateAccess = "true"))
	int32 Level;
};
