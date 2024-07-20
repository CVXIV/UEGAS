// Copyright Cvxiv


#include "Actor/MagicCircle.h"

#include "Components/DecalComponent.h"

AMagicCircle::AMagicCircle() {
	PrimaryActorTick.bCanEverTick = false;
	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>("MagicCircleDecal");
	SetRootComponent(MagicCircleDecal);
}

void AMagicCircle::SetDecalMaterial(UMaterialInterface* MaterialInterface) const {
	MagicCircleDecal->SetMaterial(0, MaterialInterface);
}

void AMagicCircle::BeginPlay() {
	Super::BeginPlay();
}
