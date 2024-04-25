// Copyright Cvxiv


#include "Actor/TranslucentActor.h"

#include "Aura/Aura.h"
#include "Kismet/KismetMaterialLibrary.h"

ATranslucentActor::ATranslucentActor() {
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMeshComponent->Mobility = EComponentMobility::Static;
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_PROJECTILE, ECR_Overlap);
	StaticMeshComponent->SetGenerateOverlapEvents(true);
	StaticMeshComponent->SetCollisionObjectType(ECC_WorldStatic);
	SetRootComponent(StaticMeshComponent);
}

void ATranslucentActor::BeginPlay() {
	Super::BeginPlay();

	TArray<UMaterialInterface*> MaterialInterfaces = StaticMeshComponent->GetMaterials();
	for (int i = 0; i < MaterialInterfaces.Num(); ++i) {
		UMaterialInstanceDynamic* InstanceDynamic = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), MaterialInterfaces[i]);
		StaticMeshComponent->SetMaterial(i, InstanceDynamic);
		DynamicInstanceMaterial.Add(InstanceDynamic);
	}
}
