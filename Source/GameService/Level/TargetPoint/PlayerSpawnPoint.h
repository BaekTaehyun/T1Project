// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/TargetPoint/GsTargetPoint.h"
#include "PlayerSpawnPoint.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API APlayerSpawnPoint : public AGsTargetPoint
{
	GENERATED_BODY()
	
public:
		UPROPERTY(EditAnywhere)
		FString _Tag;

public:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
};
