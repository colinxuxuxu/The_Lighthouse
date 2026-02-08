// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Components/AudioComponent.h" // Required for UAudioComponent
#include "Sound/SoundBase.h" // Required for USoundBase
#include "RadioSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEventRaised, FString, EventName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEventLowered, FString, EventName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerRaised, FString, Text);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCalloutTimerRaised, FString, Text);

USTRUCT(BlueprintType)
struct FDollar :public FTableRowBase
{

	GENERATED_BODY()

public:
	// The name of the Dollar, should be the same as the RowName in the Datatable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FString Name;

	// The dialogue text
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FString Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TObjectPtr<USoundBase> VoiceOver;

	// The events that will get triggered after the dialogue is complete
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TArray<FString> Events;

	// All the events that is needed for this dialogue to be triggered
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TArray<FString> EventsNeeded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	bool bHasBeenSpent = false;

	/** if this is false then it will not count towards progression*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	bool bIsCritial = true;

	/** the time to wait before moving onto the next valid dollar*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	float TimeToWait = 5.0f;

	// IDs of the next possible Dollars
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TArray<FString> NextDollars;
};

UENUM(BlueprintType)
enum class TimeOfDay :uint8
{
	Morning,
	Evening,
	Night,
	StormNight,
	StormNight2,
	SonNight
};

UENUM(BlueprintType)
enum class RadioChannel :uint8
{
	None,
	Ship,
	SOS,
	News,
	Son
};

/**
 *
 */
UCLASS(Abstract, Blueprintable)
class THE_LIGHTHOUSE_API URadioSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Datatables, FString denotes current bundle name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TMap<FString, UDataTable*> DollarTables;

	// the map containing all callout tables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Callouts")
	TMap<FString, UDataTable*> CalloutsTables;

	// setting a cooldown so it's impossible for spamming callouts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Callouts")
	float CalloutCooldown = 3.0f;

	UPROPERTY(BlueprintAssignable)
	FOnEventRaised OnEventRaised;

	UPROPERTY(BlueprintAssignable)
	FOnEventLowered OnEventLowered;

	UPROPERTY(BlueprintAssignable)
	FOnTimerRaised OnTimerRaised;

	UPROPERTY(BlueprintAssignable)
	FOnCalloutTimerRaised OnCalloutRaised;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TimeOfDay CurrTime = TimeOfDay::Morning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	RadioChannel CurrChannel = RadioChannel::News;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CurrentDollarName;

	FDollar* CurrentDollar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	UAudioComponent* Audio = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CurrentBundleName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NewsChannel = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SoSChannel = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SonChannel = 170.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShipChannel = -90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ChannelThreshold = 10.0f;

	/** if this is false we do not allow players to switch radio? -> still needs discussion over this*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsReceivingRadio = false;

	//bool to check if we can do callouts or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanCallout = true;

public:
	UFUNCTION(BlueprintCallable)
	void PlayCurrentDollar();

	UFUNCTION(BlueprintCallable)
	bool CheckChannelRemainingMessages(RadioChannel ChannelName);

	UFUNCTION(BlueprintCallable)
	bool AdjustRadioChannel(float AudioInput);

	UFUNCTION(BlueprintCallable)
	void SwapDatatable(FString TableName, UDataTable* SwapTable);

	UFUNCTION(BlueprintCallable)
	void StopCurrentAudio();

	// returns the vo text of the played dollar
	UFUNCTION(BlueprintCallable)
	FString PlayFirstValidDollar();

	UFUNCTION(BlueprintCallable)
	FString PlayFirstValidDollarInCallout();

	UFUNCTION(BlueprintCallable)
	FString DoCallouts(FString CalloutTableName, bool bIsCritical = false);

	UFUNCTION(BlueprintCallable)
	FString GetCurrentBundleViaEnum();

	/** adds the amount of key radio messages to the progression subsystem*/
	UFUNCTION(BlueprintCallable)
	void AddKeyMessagesToProgression();

	FDollar* GetDollar(const FString& DollarName);
	FDollar* GetFirstValidDollarInBundle();

	void ResetDollarTable(UDataTable* Table);

protected:
	TMap<TimeOfDay, FString> TimeEnumToBundle = { {TimeOfDay::Evening, "Evening"},{TimeOfDay::Morning, "Morning"},
		{TimeOfDay::Night, "Night"}, {TimeOfDay::StormNight, "StormNight"}, {TimeOfDay::SonNight, "SonNight"}, {TimeOfDay::StormNight2, "StormNight2"} };
	TMap<RadioChannel, FString> ChannelEnumToBundle = { {RadioChannel::News, "News"},
		{RadioChannel::Ship, "Ship"}, {RadioChannel::SOS, "SOS"}, {RadioChannel::Son, "Son"} };

	// this is used to check if the player is trying to keep spamming a channel or not
	RadioChannel LastChannel = RadioChannel::None;

	FTimerHandle TimeHandle;
	FTimerHandle CalloutHandle;
	FTimerHandle CalloutTimerHandle;

	UFUNCTION(BlueprintCallable)
	void TimedMoveOnFunction();

	UFUNCTION()
	void HandleCalloutsTimer();

	UFUNCTION()
	void HandleCalloutRaised();



};
