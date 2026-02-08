// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LighthousePlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMidMouseClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRightMouseClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMouseWheel, float, WheelVal);

/**
 *
 */
UCLASS()
class THE_LIGHTHOUSE_API ALighthousePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/** constructor*/
	ALighthousePlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float RotationSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LightActor")
	UStaticMeshComponent* LightActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightHouse Settings")
	float HorizontalAngleDeadzone = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightHouse Settings")
	float VerticalAngleDeadzone = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightHouse Settings")
	FVector InitialFwd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightHouse Settings")
	FRotator InitialRot = FRotator(0.0, 0.0, 0.0);

	UPROPERTY(BlueprintAssignable)
	FOnMidMouseClicked OnMidMousClicked;

	UPROPERTY(BlueprintAssignable)
	FOnMouseWheel OnMouseWheel;

	UPROPERTY(BlueprintAssignable)
	FOnRightMouseClicked OnRightMouseClicked;

	/** if this is true we don't accept clicking*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsMainMenu = true;

	/** only set to false after player uses the notebook*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLocked = true;

protected:
	/** MappingContext */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ScrollAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* RightMouseAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MiddleMouseDownAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	float WheelSpeed = 10.0f;

	/** handle inputs*/
	virtual void Look(const FInputActionValue& Value);

	/** handle middle mouse click*/
	void HandleMiddleMousePress();

	/** handle mouse whieel*/
	void HandleMouseWheel(const FInputActionValue& Value);

	/** handle right mouse*/
	void HandleRightMouseWheel();

	/** Initialize input bindings */
	virtual void SetupInputComponent() override;

private:
	FRotator CachedRotation = FRotator(0.0, 0.0, 0.0);
};
