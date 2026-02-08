// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ProgressionSubsystem.generated.h"



UENUM(BlueprintType)
enum class DayProgression :uint8
{
	Morning,
	Evening,
	Night,
	StormNight,
	StormNight2,
	SonNight
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayTimeChanged, DayProgression, CurrTime);

/**
 *
 */
UCLASS(Abstract, Blueprintable)
class THE_LIGHTHOUSE_API UProgressionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void ProgressGame();

	UFUNCTION(BlueprintCallable)
	bool CheckProgress();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	DayProgression CurrProgress = DayProgression::Morning;

	UPROPERTY(BlueprintAssignable)
	FOnDayTimeChanged TimeDelegate;

	// this is the amount of key messages we are after before progressing into the other time
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TargetKeyCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrCount = 0;

	// used to check if a radio is running
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsRadioRunning = false;

protected:
	void HandleBroadcastTime(DayProgression Progression);

};
