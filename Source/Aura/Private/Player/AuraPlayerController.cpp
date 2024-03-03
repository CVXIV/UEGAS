// Copyright Cvxiv

#include "Player/AuraPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController() {
	bReplicates = true;
	LastActor = nullptr;
	ThisActor = nullptr;
}

void AAuraPlayerController::PlayerTick(float DeltaTime) {
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::CursorTrace() {
	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	if (HitResult.bBlockingHit) {
		LastActor = ThisActor;
		ThisActor = Cast<IEnemyInterface>(HitResult.GetActor());

		if (LastActor) {
			if (ThisActor) {
				if (LastActor != ThisActor) {
					LastActor->UnHighlightActor();
					ThisActor->HighlightActor();
				}
			} else {
				LastActor->UnHighlightActor();
			}
		} else {
			if (ThisActor) {
				ThisActor->HighlightActor();
			}
		}
	}
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

	const FInputModeGameOnly InputModeData;
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent() {
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::Move(const struct FInputActionValue& InputActionValue) {
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
