// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyUI.generated.h"

UCLASS()
class T1PROJECT_API UMyUI : public UObject
{
	GENERATED_BODY()

	UMyUI();
	
public:
	UPROPERTY(EditAnywhere, Category = "Session")
	FString SpeakerName;

	UPROPERTY(VisibleAnywhere, Category = "Session")
	FDateTime SessionStart;

	UPROPERTY(VisibleAnywhere, Category = "Session")
	int SessionDuration;
};
