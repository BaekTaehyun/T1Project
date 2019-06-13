// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UIUtile.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API UUIUtile : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Level Utile")
	static bool TryGetRotationForFacePlayerCamera(UWorld* in_world, FVector in_location, FRotator& out_rot);
};
