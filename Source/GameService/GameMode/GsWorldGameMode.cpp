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
		world->GetTimerManager().ClearTimer(_ClosestLevelLoadingTimer);
		world->GetTimerManager().ClearTimer(_VisibleLevelsLoadingTimer);
	}
}

void AGsWorldGameMode::TeleportPlayer(FString in_tag, bool in_waitAllLoad)
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

						if (in_waitAllLoad)
						{
							world->GetTimerManager().ClearTimer(_VisibleLevelsLoadingTimer);
							world->GetTimerManager().SetTimer(_VisibleLevelsLoadingTimer
								, this
								, &AGsWorldGameMode::OnCheckVisibleLevelsLoadComplete
								, 0.1f, true, 0);
						}
						else
						{
							_ClosestLevel = GetClosestLevel();
							world->GetTimerManager().ClearTimer(_ClosestLevelLoadingTimer);
							world->GetTimerManager().SetTimer(_ClosestLevelLoadingTimer
								, this
								, &AGsWorldGameMode::OnCheckClosestLevelsLoadComplete
								, 0.1f, true, 0);
						}						
					}											
				}
			}		
		}		
	}
}

bool AGsWorldGameMode::IsLoadedVisibleLevels()
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
						FString temp = "streaming bound center : " + iter.StreamingLevel->GetStreamingVolumeBounds().GetCenter().ToString();
						
						UE_LOG(LogTemp, Log, TEXT("%s"), *temp);

						temp = "level location : " + iter.StreamingLevel->LevelTransform.GetLocation().ToString();

						UE_LOG(LogTemp, Log, TEXT("%s"), *temp);

						return false;
					}
				}
			}			

			return true;
		}
	}

	return false;
}

void AGsWorldGameMode::OnCheckVisibleLevelsLoadComplete()
{
	if (IsLoadedVisibleLevels())
	{
		SetPlayerSpawendState();

		UWorld* world = GetWorld();

		if (world)
		{
			world->GetTimerManager().ClearTimer(_VisibleLevelsLoadingTimer);
		}		
	}
}

bool AGsWorldGameMode::IsLoadedClosestLevel()
{
	if (_ClosestLevel)
	{
		return !_ClosestLevel->IsStreamingStatePending();
	}

	return false;
}

void AGsWorldGameMode::OnCheckClosestLevelsLoadComplete()
{
	if (IsLoadedClosestLevel())
	{
		SetPlayerSpawendState();

		UWorld* world = GetWorld();

		if (world)
		{
			world->GetTimerManager().ClearTimer(_ClosestLevelLoadingTimer);
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

ULevelStreaming* AGsWorldGameMode::GetClosestLevel()
{
	UWorld* world = GetWorld();
	float MinDistance = MAX_FLT;	

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
			FIntPoint worldOriginLocationXY = FIntPoint(world->OriginLocation.X, world->OriginLocation.Y);
			FVector playerLocation = player->GetActorLocation();
			ULevelStreaming* level = nullptr;

			for (FDistanceVisibleLevel& iter : VisibleLevels)
			{	
				if (iter.StreamingLevel)
				{
					const FWorldTileInfo& tile = world->WorldComposition->GetTileInfo(iter.StreamingLevel->GetWorldAssetPackageFName());

					FIntPoint levelOffsetXY = FIntPoint(tile.AbsolutePosition.X, tile.AbsolutePosition.Y);
					FIntPoint offset = levelOffsetXY - worldOriginLocationXY;
					FVector position = FVector(levelOffsetXY);
					float distance = (playerLocation - position).Size();

					if (MinDistance > distance)
					{
						MinDistance = distance;
						level = iter.StreamingLevel;
					}
				}				
			}

#if WITH_EDITOR			
			if (level)
			{
				FString loadedLevel = "Get closest level : " + level->GetWorldAssetPackageFName().ToString();
				UE_LOG(LogTemp, Log, TEXT("%s"), *loadedLevel);
			}			
#endif

			return level;
		}		
	}

	return nullptr;
}