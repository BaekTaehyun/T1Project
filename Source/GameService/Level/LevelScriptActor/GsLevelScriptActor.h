// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "GsLevelScriptActor.generated.h"

/**
 * 
 */
class APlayerSpawnPoint;

UCLASS()
class GAMESERVICE_API AGsLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

private:
	UPROPERTY()
		TMap<FString, APlayerSpawnPoint*> _PlayerSpawnMap;

public:
	void AddPlayerSpawnPoint(FString in_name, APlayerSpawnPoint* in_point);
	void RemovePlayerSpawnPoint(FString in_name);
	bool TryGetPlayerSpawnPoint(FString in_tag, APlayerSpawnPoint*& out_point);
};
