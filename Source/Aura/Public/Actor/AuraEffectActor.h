// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameFramework/Actor.h"
#include "AuraEffectActor.generated.h"

class UAuraAbilitySystemComponent;

class UGameplayEffect;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy: uint8 {
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy : uint8 {
	RemoveOnEndOverlap,
	DoNotRemove
};

USTRUCT(BlueprintType)
struct FGameplayEffectWrapper {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere)
	EEffectApplicationPolicy EffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere)
	EEffectRemovalPolicy EffectRemovalPolicy = EEffectRemovalPolicy::DoNotRemove;
};

UCLASS()
class AURA_API AAuraEffectActor : public AActor {
	GENERATED_BODY()

public:
	AAuraEffectActor();

	void SetLevel(int Level) {
		ActorLevel = Level;
	}

	void AddImpulse(const FVector& Impulse) const;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	bool ApplyEffectToTarget(AActor* TargetActor, const TSubclassOf<UGameplayEffect>& GameplayEffectClass) const;

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);

	UPROPERTY(EditAnywhere, Category = "Applied Effects")
	FGameplayEffectWrapper GameplayEffectInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	float ActorLevel = 1;

	UPROPERTY(EditAnywhere)
	bool bApplyEffectToEnemies;

	float SinAmplitude = 15;

	bool bCanSin = false;

	FVector SinInitLocation;

	float TotalSinTime = 0;
};
