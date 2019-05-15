// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/GsGameMode.h"
#include "GsWorldGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API AGsWorldGameMode : public AGsGameMode
{
	GENERATED_BODY()

private:
	bool _bCheckWorldComponentLoad;
	
public:
	AGsWorldGameMode();
	virtual void Tick(float in_delta) override;
	virtual void StartToLeaveMap();
	
	virtual void TeleportPlayer(FString in_tag);

private:	
	bool IsWorldCompositionLoadComplete();
	void WorldCompositionTick();
	void SetPlayerUnspawnedState();
	void SetPlayerSpawendState();
};
