// Fill out your copyright notice in the Description page of Project Settings.


#include "LighthousePlayerController.h"
#include "ProgressionSubsystem.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "The_LighthouseCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "The_Lighthouse.h"

ALighthousePlayerController::ALighthousePlayerController()
{
}


void ALighthousePlayerController::Look(const FInputActionValue& Value)
{
	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld()))
	{
		if (UProgressionSubsystem* Progression = GameInstance->GetSubsystem<UProgressionSubsystem>())
		{
			if (Progression->CurrProgress != DayProgression::Morning && Progression->CurrProgress != DayProgression::Evening && !Progression->bIsRadioRunning)
			{
				if (APawn* Char = GetPawn())
				{
					//float RotationAmount = Value.Get<float>() * RotationSpeed;
					FVector2D Axis = Value.Get<FVector2D>();
					// Yaw
					float RotationZ = Axis.X * RotationSpeed;
					// pitch
					float RotationY = Axis.Y * RotationSpeed;


					FRotator Rotate = FRotator(RotationY, RotationZ, 0.0f);
					CachedRotation += Rotate;
					CachedRotation.Yaw = FMath::Clamp(CachedRotation.Yaw, -HorizontalAngleDeadzone, HorizontalAngleDeadzone);
					CachedRotation.Pitch = FMath::Clamp(CachedRotation.Pitch, -VerticalAngleDeadzone, VerticalAngleDeadzone);

					Char->SetActorRotation(InitialRot + CachedRotation);
					//Char->AddActorWorldRotation(Rotate);

				}
			}
		}
	}
}

void ALighthousePlayerController::HandleMiddleMousePress()
{
	// broadcast t dlgate and let blueprint handle it
	if (!bIsMainMenu && !bIsLocked)
	{
		OnMidMousClicked.Broadcast();
	}
}

void ALighthousePlayerController::HandleMouseWheel(const FInputActionValue& Value)
{
	if (!bIsMainMenu && !bIsLocked)
	{
		float ChangeAmount = Value.Get<float>() * WheelSpeed;
		OnMouseWheel.Broadcast(ChangeAmount);
	}
}

void ALighthousePlayerController::HandleRightMouseWheel()
{
	if (!bIsMainMenu)
	{
		OnRightMouseClicked.Broadcast();
	}
}

void ALighthousePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		// Set up action bindings
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALighthousePlayerController::Look);
			EnhancedInputComponent->BindAction(MiddleMouseDownAction, ETriggerEvent::Triggered, this, &ALighthousePlayerController::HandleMiddleMousePress);
			EnhancedInputComponent->BindAction(ScrollAction, ETriggerEvent::Triggered, this, &ALighthousePlayerController::HandleMouseWheel);
			EnhancedInputComponent->BindAction(RightMouseAction, ETriggerEvent::Triggered, this, &ALighthousePlayerController::HandleRightMouseWheel);
		}
	}
}
