// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LevelUtile.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API ULevelUtile : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Level Utile")		
	static FName GetLevelNameFromActor(AActor* Actor);
};
