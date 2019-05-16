// Fill out your copyright notice in the Description page of Project Settings.


#include "GsWorldGameMode.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/WorldComposition.h"
#include "Runtime/Engine/Classes/GameFramework/Character.h"
#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"
#include "Runtime/Engine/Classes/Engine/LevelScriptActor.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "GameService/Level/TargetPoint/PlayerSpawnPoint.h"
#include "GameService/Level/LevelScriptActor/GsLevelScriptActor.h"
#include "GameService/Character/GsPlayer.h"

AGsWorldGameMode::AGsWorldGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGsWorldGameMode::StartToLeaveMap()
{
	UWorld* world = GetWorld();

	if (world)
	{
		world->GetTimerManager().ClearTimer(_loadCompleteTimer);
	}
}

void AGsWorldGameMode::TeleportPlayer(FString in_tag)
{
	UWorld* world = GetWorld();

	if (world)
	{
		ACharacter* character = UGameplayStatics::GetPlayerCharacter(world, 0);

		if (character)
		{
			ALevelScriptActor* level =	world->GetLevelScriptActor();

			if (level)
			{
				AGsLevelScriptActor* baseLevel = Cast<AGsLevelScriptActor>(level);

				if (baseLevel)
				{
					APlayerSpawnPoint* point = nullptr;

					if (baseLevel->TryGetPlayerSpawnPoint(in_tag, point))
					{						
						SetPlayerUnspawnedState();
						character->SetActorLocation(point->GetActorLocation());																		
						world->UpdateLevelStreaming();						
						
						world->GetTimerManager().ClearTimer(_loadCompleteTimer);
						world->GetTimerManager().SetTimer(_loadCompleteTimer, this, &AGsWorldGameMode::OnCheckLevelsLoadComplete, 0.1f, true, 0);
					}											
				}
			}		
		}		
	}
}

bool AGsWorldGameMode::IsWorldCompositionLoadComplete()
{
	UWorld* world = GetWorld();

	if (world)
	{
		TArray<FDistanceVisibleLevel> VisibleLevels;
		TArray<FDistanceVisibleLevel> HiddenLevels;

		ACharacter* player = UGameplayStatics::GetPlayerCharacter(world, 0);

		if (player)
		{
			world->WorldComposition->GetDistanceVisibleLevels(player->GetActorLocation()
				, VisibleLevels
				, HiddenLevels);

			for (FDistanceVisibleLevel& iter : VisibleLevels)
			{
				if (iter.StreamingLevel)
				{
					if (iter.StreamingLevel->IsStreamingStatePending())
					{					
						return false;
					}
				}
			}

			return true;
		}
	}

	return false;
}

void AGsWorldGameMode::OnCheckLevelsLoadComplete()
{
	if (IsWorldCompositionLoadComplete())
	{
		SetPlayerSpawendState();

		UWorld* world = GetWorld();

		if (world)
		{
			world->GetTimerManager().ClearTimer(_loadCompleteTimer);
		}		
	}
}

void AGsWorldGameMode::SetPlayerUnspawnedState()
{
	UWorld* world = GetWorld();

	if (world)
	{
		ACharacter* character = UGameplayStatics::GetPlayerCharacter(world, 0);

		if (character)
		{
			AGsPlayer* player = Cast<AGsPlayer>(character);

			if (player)
			{
				player->SetPlayerSpawnState(false);
			}
		}
	}
}

void AGsWorldGameMode::SetPlayerSpawendState()
{
	UWorld* world = GetWorld();

	if (world)
	{
		ACharacter* character = UGameplayStatics::GetPlayerCharacter(world, 0);

		if (character)
		{
			AGsPlayer* player = Cast<AGsPlayer>(character);

			if (player)
			{
				player->SetPlayerSpawnState(true);
			}
		}
	}
}
