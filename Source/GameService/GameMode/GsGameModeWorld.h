// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/WorldComposition.h"
#include "GsGameModeBase.h"
#include "GsGameModeWorld.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API AGsGameModeWorld : public AGsGameModeBase
{
	GENERATED_BODY()

private:	
	UPROPERTY()
	FTimerHandle _VisibleLevelsLoadingTimer;
	UPROPERTY()
	FTimerHandle _ClosestLevelLoadingTimer;	
	UPROPERTY()
	ULevelStreaming* _ClosestLevel;
	
public:
	AGsGameModeWorld();

	virtual void StartPlay() override;

	virtual void StartToLeaveMap();
	virtual void TeleportPlayer(FString in_tag, bool in_waitAllLoad = false);	

private:	
	bool IsLoadedVisibleLevels();		
	void OnCheckVisibleLevelsLoadComplete();
	bool IsLoadedClosestLevel();
	void OnCheckClosestLevelsLoadComplete();
	void SetPlayerUnspawnedState();
	void SetPlayerSpawendState();
	ULevelStreaming* GetClosestLevel();
	void StartCheckAllVisibleLevelsLoadTimer();
	void StartCheckClosestVisibleLevelLoadTimer();

	void SpawnPlayer();
};
