// Copyright Cvxiv


#include "UI/Widget/AuraFloatTextWidget.h"

#include "Kismet/GameplayStatics.h"

void UAuraFloatTextWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	FVector2D ScreenPosition;
	UGameplayStatics::ProjectWorldToScreen(UGameplayStatics::GetPlayerController(GetWorld(), 0), OwningActor->GetActorLocation(), ScreenPosition, false);

	SetPositionInViewport(BaseOffset + ScreenPosition);
}
