// Copyright Cvxiv

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/TranslucentInterface.h"
#include "TranslucentActor.generated.h"

UCLASS()
class AURA_API ATranslucentActor : public AActor, public ITranslucentInterface {
	GENERATED_BODY()

public:
	ATranslucentActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(BlueprintReadOnly)
	TArray<UMaterialInstanceDynamic*> DynamicInstanceMaterial;
};
