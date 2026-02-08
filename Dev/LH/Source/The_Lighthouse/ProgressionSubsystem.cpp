// Fill out your copyright notice in the Description page of Project Settings.


#include "ProgressionSubsystem.h"
#include "RadioSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UProgressionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UProgressionSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UProgressionSubsystem::ProgressGame()
{
	CurrCount = 0;
	// we also want to change the day time enum of the radio subsystem
	if (CurrProgress == DayProgression::Morning)
	{
		CurrProgress = DayProgression::Evening;
		if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld()))
		{
			if (URadioSubsystem* Radio = GameInstance->GetSubsystem<URadioSubsystem>())
			{
				Radio->CurrTime = TimeOfDay::Evening;
				// reset our target count and increment here
				TargetKeyCount = 0;
				Radio->AddKeyMessagesToProgression();
			}
		}

	}
	else if (CurrProgress == DayProgression::Evening)
	{
		CurrProgress = DayProgression::Night;
		if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld()))
		{
			if (URadioSubsystem* Radio = GameInstance->GetSubsystem<URadioSubsystem>())
			{
				Radio->CurrTime = TimeOfDay::Night;
				TargetKeyCount = 0;
				Radio->AddKeyMessagesToProgression();
				// messages + one boat
				TargetKeyCount += 1;
			}
		}
	}
	else if (CurrProgress == DayProgression::Night)
	{
		CurrProgress = DayProgression::StormNight;
		if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld()))
		{
			if (URadioSubsystem* Radio = GameInstance->GetSubsystem<URadioSubsystem>())
			{
				Radio->CurrTime = TimeOfDay::StormNight;
				TargetKeyCount = 0;
				Radio->AddKeyMessagesToProgression();
				// messages + 1 boat
				TargetKeyCount += 1;
			}
		}
	}
	else if (CurrProgress == DayProgression::StormNight)
	{
		CurrProgress = DayProgression::StormNight2;
		if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld()))
		{
			if (URadioSubsystem* Radio = GameInstance->GetSubsystem<URadioSubsystem>())
			{
				Radio->CurrTime = TimeOfDay::StormNight2;
				TargetKeyCount = 0;
				Radio->AddKeyMessagesToProgression();
				// messages + 1 boats
				TargetKeyCount += 1;
			}
		}
	}
	else//son sequence
	{
		CurrProgress = DayProgression::SonNight;
		if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld()))
		{
			if (URadioSubsystem* Radio = GameInstance->GetSubsystem<URadioSubsystem>())
			{
				Radio->CurrTime = TimeOfDay::SonNight;
				TargetKeyCount = 10000;
			}
		}
	}
	HandleBroadcastTime(CurrProgress);
}

bool UProgressionSubsystem::CheckProgress()
{
	//debug message
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Target Value: %d, Curr Value: %d"),
		TargetKeyCount, CurrCount));
	if (CurrCount >= TargetKeyCount)
	{
		ProgressGame();
		return true;
	}
	return false;
}

void UProgressionSubsystem::HandleBroadcastTime(DayProgression Progression)
{
	TimeDelegate.Broadcast(Progression);
}
