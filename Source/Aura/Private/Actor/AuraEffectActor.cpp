// Copyright Cvxiv


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

AAuraEffectActor::AAuraEffectActor() {
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	StaticMeshComponent->SetSimulatePhysics(false);
	StaticMeshComponent->SetGenerateOverlapEvents(false);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	SetRootComponent(StaticMeshComponent);
}

void AAuraEffectActor::AddImpulse(const FVector& Impulse) {
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->AddImpulse(Impulse);
}

void AAuraEffectActor::BeginPlay()
{
	StaticMeshComponent->OnComponentHit.AddUniqueDynamic(this, &AAuraEffectActor::OnHit);
	Super::BeginPlay();
}

void AAuraEffectActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherComp->GetCollisionObjectType() == ECC_WorldStatic)
	{
		StaticMeshComponent->SetSimulatePhysics(false);
		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

bool AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, const TSubclassOf<UGameplayEffect>& GameplayEffectClass) const {
	if (!IsValid(GameplayEffectClass)) {
		return false;
	}
	UAuraAbilitySystemComponent* Asc = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
	if (!IsValid(Asc)) {
		return false;
	}
	if (!bApplyEffectToEnemies && TargetActor->ActorHasTag(FName("Enemy"))) {
		return false;
	}

	//创建Effect的句柄 包含了实例化Effect所需数据
	FGameplayEffectContextHandle EffectContextHandle = Asc->MakeEffectContext();
	//设置创建Effect的对象
	EffectContextHandle.AddSourceObject(this);
	//Effect的实例化后的句柄，可以通过此来寻找调用
	const FGameplayEffectSpecHandle GameplayEffectSpecHandle = Asc->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	//从句柄中获取到实例的地址，并被应用
	const FActiveGameplayEffectHandle ActiveGameplayEffectHandle = Asc->ApplyGameplayEffectSpecToSelf(*GameplayEffectSpecHandle.Data.Get());
	if (GameplayEffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite) {
		if (!ActiveGameplayEffectHandle.WasSuccessfullyApplied()) {
			Asc->GetGameplayEffectDenyCount().FindOrAdd(GameplayEffectClass)++;
		} else {
			const TSharedPtr<TQueue<FActiveGameplayEffectHandle>> Queue = Asc->GetGameplayEffectHandle().FindOrAdd(GameplayEffectClass, MakeShared<TQueue<FActiveGameplayEffectHandle>>());
			Queue->Enqueue(ActiveGameplayEffectHandle);
		}
		return false;
	}
	return true;
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor) {
	if (GameplayEffectInfo.EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap) {
		if (TargetActor->HasAuthority()) {
			if (ApplyEffectToTarget(TargetActor, GameplayEffectInfo.GameplayEffectClass)) {
				Destroy();
			}
		}
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor) {
	// 添加效果
	if (GameplayEffectInfo.EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap) {
		if (TargetActor->HasAuthority()) {
			if (ApplyEffectToTarget(TargetActor, GameplayEffectInfo.GameplayEffectClass)) {
				Destroy();
			}
		}
	}

	// 删除效果
	if (GameplayEffectInfo.EffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap) {
		if (TargetActor->HasAuthority()) {
			if (UAuraAbilitySystemComponent* AbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))) {
				if (int& Count = AbilitySystemComponent->GetGameplayEffectDenyCount().FindOrAdd(GameplayEffectInfo.GameplayEffectClass); Count > 0) {
					Count--;
				} else {
					const TSharedPtr<TQueue<FActiveGameplayEffectHandle>> Queue = AbilitySystemComponent->GetGameplayEffectHandle().FindOrAdd(GameplayEffectInfo.GameplayEffectClass, MakeShared<TQueue<FActiveGameplayEffectHandle>>());
					if (FActiveGameplayEffectHandle ActiveGameplayEffectHandle; Queue->Dequeue(ActiveGameplayEffectHandle)) {
						AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveGameplayEffectHandle, 1);
					}
				}
			}
		}
	}
}
