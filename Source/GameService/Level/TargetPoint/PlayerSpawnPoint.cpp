// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSpawnPoint.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Level/LevelScriptActor/GsLevelScriptActor.h"

void APlayerSpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();

	if (world)
	{
		ALevelScriptActor* level = world->GetLevelScriptActor();

		if (level)
		{
			AGsLevelScriptActor* worldLevel = Cast<AGsLevelScriptActor>(level);

			if (worldLevel)
			{
				worldLevel->AddPlayerSpawnPoint(_Tag, this);
			}
		}
	}
}

void APlayerSpawnPoint::BeginDestroy()
{
	UWorld* world = GetWorld();

	if (world)
	{
		ALevelScriptActor* level = world->GetLevelScriptActor();

		if (level)
		{
			AGsLevelScriptActor* worldLevel = Cast<AGsLevelScriptActor>(level);

			if (worldLevel)
			{
				worldLevel->RemovePlayerSpawnPoint(_Tag);
			}
		}
	}

	Super::BeginDestroy();
}