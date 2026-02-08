// Fill out your copyright notice in the Description page of Project Settings.


#include "RadioSubsystem.h"
#include "ProgressionSubsystem.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

void URadioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// manually reset all dollars when intializing
	TArray<UDataTable*> Tables;
	DollarTables.GenerateValueArray(Tables);
	for (UDataTable* Table : Tables)
	{
		ResetDollarTable(Table);
	}

	// adding to target count
	AddKeyMessagesToProgression();
}

void URadioSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void URadioSubsystem::PlayCurrentDollar()
{
	CurrentDollar = GetDollar(CurrentDollarName);
	if (CurrentDollar != nullptr && !CurrentDollar->bHasBeenSpent)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), CurrentDollar->VoiceOver);
		if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
		{
			if (UProgressionSubsystem* Progression = GameInstance->GetSubsystem<UProgressionSubsystem>())
			{
				Progression->CurrCount++;
				Progression->CheckProgress();
			}
		}
		CurrentDollar->bHasBeenSpent = true;
	}
}

bool URadioSubsystem::CheckChannelRemainingMessages(RadioChannel ChannelName)
{
	// this will be the channel we are checking for remaining messages
	FString Channel = TimeEnumToBundle[CurrTime] + ChannelEnumToBundle[ChannelName];
	if (DollarTables.Contains(Channel))
	{
		if (const UDataTable* Table = DollarTables[Channel])
		{
			TArray<FDollar*> Dollars;
			Table->GetAllRows<FDollar>(TEXT(""), Dollars);
			// loop through to find the first unused dollar
			for (FDollar* Dollar : Dollars)
			{
				if (!Dollar->bHasBeenSpent)
				{
					// this means there's remaining dollars in the datatable
					return true;
				}
			}
		}
	}
	return false;
}

// return value act as a leading/trailing edge detector on whether the current channel is None or not
bool URadioSubsystem::AdjustRadioChannel(float AudioInput)
{
	// comparing the audio input value to all of our thresholds to know which channel we are on
	if (AudioInput < (ShipChannel + ChannelThreshold) && AudioInput >(ShipChannel - ChannelThreshold))
	{
		CurrChannel = RadioChannel::Ship;
		return false;
	}
	if (AudioInput < (NewsChannel + ChannelThreshold) && AudioInput >(NewsChannel - ChannelThreshold))
	{
		CurrChannel = RadioChannel::News;
		return false;
	}
	if (AudioInput < (SoSChannel + ChannelThreshold) && AudioInput >(SoSChannel - ChannelThreshold))
	{
		CurrChannel = RadioChannel::SOS;
		return false;
	}
	if (AudioInput < (SonChannel + ChannelThreshold) && AudioInput >(SonChannel - ChannelThreshold))
	{
		CurrChannel = RadioChannel::Son;
		return false;
	}
	CurrChannel = RadioChannel::None;
	return true;
}

void URadioSubsystem::SwapDatatable(FString TableName, UDataTable* SwapTable)
{
	if (DollarTables.Contains(TableName))
	{
		DollarTables[TableName] = SwapTable;
	}
}

void URadioSubsystem::StopCurrentAudio()
{
	if (Audio != nullptr)
	{
		Audio->Stop();
	}
}

FString URadioSubsystem::PlayFirstValidDollar()
{
	CurrentDollar = GetFirstValidDollarInBundle();
	if (CurrentDollar != nullptr && CurrChannel != LastChannel && CurrChannel != RadioChannel::None)
	{
		//check if we have a valid audio component, if not we make one first
		if (Audio == nullptr)
		{
			Audio = UGameplayStatics::SpawnSound2D(GetWorld(), CurrentDollar->VoiceOver);
		}
		else// if Audio is already valid we just use the same component
		{
			Audio->SetSound(CurrentDollar->VoiceOver);
			Audio->Play();
		}
		CurrentDollar->bHasBeenSpent = true;
		bIsReceivingRadio = true;

		if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
		{
			//set up timer
			GameInstance->GetTimerManager().SetTimer(TimeHandle, this, &URadioSubsystem::TimedMoveOnFunction, CurrentDollar->TimeToWait, false);
		}

		// only counts towards progression if it's marked as critical
		if (CurrentDollar->bIsCritial)
		{
			if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
			{
				if (UProgressionSubsystem* Progression = GameInstance->GetSubsystem<UProgressionSubsystem>())
				{
					Progression->CurrCount++;
					Progression->CheckProgress();
				}
			}
		}
		LastChannel = CurrChannel;
		return CurrentDollar->Text;
	}
	LastChannel = RadioChannel::None;
	return "";
}

FString URadioSubsystem::PlayFirstValidDollarInCallout()
{
	CurrentDollar = GetFirstValidDollarInBundle();
	if (CurrentDollar != nullptr)
	{
		//check if we have a valid audio component, if not we make one first
		if (Audio == nullptr)
		{
			Audio = UGameplayStatics::SpawnSound2D(GetWorld(), CurrentDollar->VoiceOver);
		}
		else// if Audio is already valid we just use the same component
		{
			Audio->SetSound(CurrentDollar->VoiceOver);
			Audio->Play();
		}
		CurrentDollar->bHasBeenSpent = true;
		bIsReceivingRadio = true;

		if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
		{
			//set up timer
			GameInstance->GetTimerManager().SetTimer(CalloutTimerHandle, this, &URadioSubsystem::HandleCalloutRaised, CurrentDollar->TimeToWait, false);
		}

		// only counts towards progression if it's marked as criticalx
		if (CurrentDollar->bIsCritial)
		{
			if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
			{
				if (UProgressionSubsystem* Progression = GameInstance->GetSubsystem<UProgressionSubsystem>())
				{
					Progression->CurrCount++;
					Progression->CheckProgress();
				}
			}
		}
		LastChannel = CurrChannel;
		return CurrentDollar->Text;
	}
	LastChannel = CurrChannel;
	return "";
}

FString URadioSubsystem::DoCallouts(FString CalloutTableName, bool bIsCritical)
{
	if (!CalloutsTables.IsEmpty())
	{
		if (UDataTable* Callout = CalloutsTables[CalloutTableName])
		{
			TArray<FDollar*> Dollars;
			// get all dollars
			Callout->GetAllRows<FDollar>(TEXT(""), Dollars);
			// getting a random index from the dollar table
			int RandomIndex = FMath::RandRange(0, Dollars.Num() - 1);
			// we ignore the check for condition since we absolutely want this to be on screen
			if (bIsCritical)
			{
				bCanCallout = false;
				if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
				{
					//set up timer
					GameInstance->GetTimerManager().SetTimer(CalloutHandle, this, &URadioSubsystem::HandleCalloutsTimer, CalloutCooldown, false);
				}
				return Dollars[RandomIndex]->Text;
			}
			// only do callouts if a dialogue is not playing
			if (!bIsReceivingRadio && bCanCallout)
			{
				bCanCallout = false;
				if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
				{
					//set up timer
					GameInstance->GetTimerManager().SetTimer(CalloutHandle, this, &URadioSubsystem::HandleCalloutsTimer, CalloutCooldown, false);
				}
				return Dollars[RandomIndex]->Text;
			}
		}
	}
	return "";
}

FString URadioSubsystem::GetCurrentBundleViaEnum()
{
	if (TimeEnumToBundle.Contains(CurrTime) && ChannelEnumToBundle.Contains(CurrChannel))
	{
		// we use the format of Time + channel type
		CurrentBundleName = TimeEnumToBundle[CurrTime] + ChannelEnumToBundle[CurrChannel];
		return CurrentBundleName;
	}
	return "";
}

void URadioSubsystem::AddKeyMessagesToProgression()
{
	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		if (UProgressionSubsystem* Progression = GameInstance->GetSubsystem<UProgressionSubsystem>())
		{
			for (auto& Pair : ChannelEnumToBundle)
			{
				// grabbing the name of the datatable
				FString Key = TimeEnumToBundle[CurrTime] + Pair.Value;
				// check if this is one of the dollar tables
				if (DollarTables.Contains(Key))
				{
					// finds out how many messages are in the dollar table
					TArray<FDollar*> Dollars;
					DollarTables[Key]->GetAllRows<FDollar>(TEXT(""), Dollars);
					for (FDollar* Dollar : Dollars)
					{
						// we only add the critical dollars towards the progression
						if (Dollar->bIsCritial)
						{
							Progression->TargetKeyCount += 1;
						}
					}
				}
			}
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Int Value: %d"), Progression->TargetKeyCount));
		}
	}
}

FDollar* URadioSubsystem::GetDollar(const FString& DollarName)
{
	// need to make sure we have a valid string
	if (!DollarName.IsEmpty())
	{
		if (DollarTables.Contains(GetCurrentBundleViaEnum()))
		{
			if (FDollar* Dollar = DollarTables[CurrentBundleName]->FindRow<FDollar>(FName(*DollarName), ""))
			{
				return Dollar;
			}
		}
	}
	return nullptr;
}

FDollar* URadioSubsystem::GetFirstValidDollarInBundle()
{
	// trying to fetch the current bundle
	if (DollarTables.Contains(CurrentBundleName))
	{
		if (const UDataTable* DollarTable = DollarTables[CurrentBundleName])
		{
			TArray<FDollar*> Dollars;
			// get all dollars
			DollarTable->GetAllRows<FDollar>(TEXT(""), Dollars);

			// loop through to find the first unused dollar
			for (FDollar* Dollar : Dollars)
			{
				if (!Dollar->bHasBeenSpent)
				{
					return Dollar;
				}
			}
		}
	}
	return nullptr;
}

void URadioSubsystem::ResetDollarTable(UDataTable* Table)
{
	TArray<FDollar*> Dollars;
	Table->GetAllRows<FDollar>(TEXT(""), Dollars);

	for (FDollar* Dollar : Dollars)
	{
		Dollar->bHasBeenSpent = false;
	}
}

void URadioSubsystem::TimedMoveOnFunction()
{
	//set radio receiving bool to false
	bIsReceivingRadio = false;
	// check if the player is staying on a channel
	if (LastChannel == CurrChannel)
	{
		// a cheeky way of bypassing the playfirstvalid dollar check
		LastChannel = RadioChannel::None;
		// broadcast the text so we can change the subtitle
		OnTimerRaised.Broadcast(PlayFirstValidDollar());
	}
}

void URadioSubsystem::HandleCalloutsTimer()
{
	bCanCallout = true;
}

void URadioSubsystem::HandleCalloutRaised()
{
	OnCalloutRaised.Broadcast(PlayFirstValidDollarInCallout());
	if (CurrentDollar != nullptr)
	{
		if (!CurrentDollar->Events.IsEmpty())
		{
			OnEventRaised.Broadcast(CurrentDollar->Events[0]);
		}
	}
}



