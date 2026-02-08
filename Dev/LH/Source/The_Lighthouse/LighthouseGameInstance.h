// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LighthouseGameInstance.generated.h"

/**
 *
 */
UCLASS()
class THE_LIGHTHOUSE_API ULighthouseGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<TSubclassOf<USubsystem>> BPSubsystems;
};
