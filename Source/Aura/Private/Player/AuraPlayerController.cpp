// Copyright Cvxiv

#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController() {
	bReplicates = true;
	LastActor = nullptr;
	ThisActor = nullptr;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime) {
	Super::PlayerTick(DeltaTime);

	CursorTrace();

	AutoRunToDestination();
}

void AAuraPlayerController::CursorTrace() {
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
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

void AAuraPlayerController::AbilityInputTagPressed(const FGameplayTag InputTag) {
	if (FAuraGameplayTags::Get().InputTag_LMB.MatchesTagExact(InputTag)) {
		bTargeting = ThisActor != nullptr;
		if (bTargeting) {
			bAutoRunning = false;
		}
	}
}

void AAuraPlayerController::AbilityInputTagReleased(const FGameplayTag InputTag) {
	if (!FAuraGameplayTags::Get().InputTag_LMB.MatchesTagExact(InputTag)) {
		GetAuraAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
	} else {
		if (bTargeting) {
			GetAuraAbilitySystemComponent()->AbilityInputTagReleased(InputTag);
		} else {
			if (FollowTime <= ShortPressedThreshold) {
				if (const APawn* ControlledPawn = GetPawn()) {
					if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination)) {
						Spline->ClearSplinePoints();
						for (const FVector& LocPoint : NavPath->PathPoints) {
							Spline->AddSplinePoint(LocPoint, ESplineCoordinateSpace::World);
							DrawDebugSphere(GetWorld(), LocPoint, 8, 8, FColor::Green, false, 3);
						}
						CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
						bAutoRunning = true;
					}
				}
			}
		}
		FollowTime = 0.f;
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(const FGameplayTag InputTag) {
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

void AAuraPlayerController::BeginPlay() {
	Super::BeginPlay();
	check(AuraContext)
	check(MoveAction)

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		Subsystem->AddMappingContext(AuraContext, 0);
	}

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

void AAuraPlayerController::Move(const struct FInputActionValue& InputActionValue) {
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
