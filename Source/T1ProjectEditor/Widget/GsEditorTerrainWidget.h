// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GsEditorTerrainWidget.generated.h"

/**
 * 
 */
class UTextBlock;

UCLASS()
class T1PROJECTEDITOR_API UGsEditorTerrainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
		UTextBlock* _TextBlock;
	
};
