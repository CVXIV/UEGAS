// Copyright Cvxiv

#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Actor/MagicCircle.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/SplineComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "UI/Widget/AuraFloatTextWidget.h"

static const FBox LimitBox(FVector(-60, -100, 0), FVector(60, -50, 0));

AAuraPlayerController::AAuraPlayerController() {
	bReplicates = true;
	LastActor = nullptr;
	ThisActor = nullptr;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime) {
	Super::PlayerTick(DeltaTime);

	check(FloatTextWidgetClass)

	CursorTrace();

	AutoRunToDestination();

	UpdateMagicCircleLocation();
}

void AAuraPlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial) {
	if (!IsValid(MagicCircle)) {
		MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);
		if (DecalMaterial) {
			MagicCircle->SetDecalMaterial(DecalMaterial);
		}
		this->bShowMouseCursor = false;
	}
}

void AAuraPlayerController::HideMagicCircle() {
	if (IsValid(MagicCircle)) {
		MagicCircle->Destroy();
		this->bShowMouseCursor = true;
	}
}

void AAuraPlayerController::CursorTrace() {
	if (GetAuraAbilitySystemComponent() && (GetAuraAbilitySystemComponent()->HasAnyMatchingGameplayTags(FilterCursorTags) || GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(FAuraGameplayTags::Get().DeBuff_Stun))) {
		if (LastActor) {
			LastActor->UnHighlightActor();
		}
		if (ThisActor) {
			ThisActor->UnHighlightActor();
		}
		LastActor = nullptr;
		ThisActor = nullptr;
		return;
	}
	GetHitResultUnderCursor(IsValid(MagicCircle) ? ECC_Camera : ECC_Visibility, false, HitResult);
	if (HitResult.bBlockingHit) {
		LastActor = ThisActor;
		ThisActor = Cast<IEnemyInterface>(HitResult.GetActor());

		if (LastActor != ThisActor) {
			if (LastActor) {
				LastActor->UnHighlightActor();
			}
			if (ThisActor) {
				ThisActor->HighlightActor();
			}
		}
	}
}

void AAuraPlayerController::AutoRunToDestination() {
	if (bAutoRunning) {
		if (APawn* ControlledPawn = GetPawn()) {
			const FVector CurrentLoc3D = ControlledPawn->GetActorLocation();
			const FVector CurrentLoc = FVector(CurrentLoc3D.X, CurrentLoc3D.Y, 0);
			if (FVector::DistSquaredXY(CurrentLoc, CachedDestination) <= AutoRunAcceptanceRadius * AutoRunAcceptanceRadius) {
				bAutoRunning = false;
				return;
			}
			const FVector NextLoc = Spline->FindLocationClosestToWorldLocation(CurrentLoc3D, ESplineCoordinateSpace::World);
			FVector Direction = Spline->FindDirectionClosestToWorldLocation(NextLoc, ESplineCoordinateSpace::World);
			const FVector CurToNextDirection = NextLoc - CurrentLoc;
			if (CurToNextDirection.SquaredLength() > AutoRunAcceptanceRadius * AutoRunAcceptanceRadius) {
				Direction = (CurToNextDirection.GetSafeNormal() + Direction).GetSafeNormal();
			}
			ControlledPawn->AddMovementInput(Direction * GetWorld()->GetDeltaSeconds() * 100.f);
		}
	}
}

void AAuraPlayerController::UpdateMagicCircleLocation() const {
	if (IsValid(MagicCircle)) {
		MagicCircle->SetActorLocation(HitResult.ImpactPoint);
	}
}

void AAuraPlayerController::AbilityInputTagPressed(const FGameplayTag InputTag) {
	if (GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Action_Attack) || GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(FAuraGameplayTags::Get().DeBuff_Stun)) {
		return;
	}
	if (FAuraGameplayTags::Get().InputTag_LMB.MatchesTagExact(InputTag)) {
		bTargeting = ThisActor != nullptr;
		if (bTargeting) {
			bAutoRunning = false;
		}
	}
	if (GetAuraAbilitySystemComponent()) {
		GetAuraAbilitySystemComponent()->AbilityInputTagPressed(InputTag);
	}
}

void AAuraPlayerController::AbilityInputTagReleased(const FGameplayTag InputTag) {
	if (FAuraGameplayTags::Get().InputTag_LMB.MatchesTagExact(InputTag) && (GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Action_Attack) || GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(FAuraGameplayTags::Get().DeBuff_Stun))) {
		return;
	}
	if (!FAuraGameplayTags::Get().InputTag_LMB.MatchesTagExact(InputTag)) {
		GetAuraAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
	} else {
		if (bTargeting) {
			GetAuraAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
		} else {
			if (FollowTime <= ShortPressedThreshold) {
				if (const APawn* ControlledPawn = GetPawn()) {
					if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination)) {
						if (!NavPath->PathPoints.IsEmpty()) {
							Spline->ClearSplinePoints();
							for (const FVector& LocPoint : NavPath->PathPoints) {
								Spline->AddSplinePoint(LocPoint, ESplineCoordinateSpace::World);
							}
							CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
							bAutoRunning = true;
						}
					}
				}
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
			}
		}
		FollowTime = 0.f;
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(const FGameplayTag InputTag) {
	if (FAuraGameplayTags::Get().InputTag_LMB.MatchesTagExact(InputTag) && (GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Action_Attack) || GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(FAuraGameplayTags::Get().DeBuff_Stun))) {
		return;
	}
	if (!FAuraGameplayTags::Get().InputTag_LMB.MatchesTagExact(InputTag)) {
		GetAuraAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
	} else {
		if (bTargeting) {
			GetAuraAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
		} else {
			FollowTime += GetWorld()->GetDeltaSeconds();
			if (HitResult.bBlockingHit) {
				CachedDestination = HitResult.ImpactPoint;
				if (FollowTime > ShortPressedThreshold) {
					bAutoRunning = false;
					if (APawn* ControlledPawn = GetPawn()) {
						const FVector Direction = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
						ControlledPawn->AddMovementInput(Direction * GetWorld()->GetDeltaSeconds() * 100.f);
					}
				}
			}
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetAuraAbilitySystemComponent() {
	if (!AuraAbilitySystemComponent) {
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}
	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::ClientShowWidget_Implementation(AActor* Target, float Damage, bool bIsCriticalHit, bool bIsBlockedHit) {
	if (IsValid(Target)) {
		UAuraFloatTextWidget* FloatTextWidget = Cast<UAuraFloatTextWidget>(CreateWidget(GetWorld(), FloatTextWidgetClass));

		const FVector RandomPoint = FMath::RandPointInBox(LimitBox);
		FloatTextWidget->BaseOffset = FVector2D(RandomPoint.X, RandomPoint.Y);
		FloatTextWidget->OwningActor = Target;
		FloatTextWidget->SetDamageText(Damage, bIsCriticalHit, bIsBlockedHit);
		FloatTextWidget->AddToViewport();
	}
}

void AAuraPlayerController::BeginPlay() {
	Super::BeginPlay();
	check(AuraContext)
	check(MoveAction)

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	FilterCursorTags.AddTag(FAuraGameplayTags::Get().Action_Attack);
	FilterCursorTags.AddTag(FAuraGameplayTags::Get().Action_HitReact);

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameOnly InputMode;
	InputMode.SetConsumeCaptureMouseDown(false);
	SetInputMode(InputMode);
}

void AAuraPlayerController::SetupInputComponent() {
	Super::SetupInputComponent();
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);

	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue) {
	if (GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Action_Attack) || GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(FAuraGameplayTags::Get().DeBuff_Stun)) {
		return;
	}
	bAutoRunning = false;
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* Obj = GetPawn<APawn>()) {
		Obj->AddMovementInput(ForwardDirection, static_cast<float>(InputAxisVector.Y));
		Obj->AddMovementInput(RightDirection, static_cast<float>(InputAxisVector.X));
	}
}
