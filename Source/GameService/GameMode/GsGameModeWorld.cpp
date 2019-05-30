// Fill out your copyright notice in the Description page of Project Settings.


#include "GsGameModeWorld.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/WorldComposition.h"
#include "Runtime/Engine/Classes/GameFramework/Character.h"
#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"
#include "Runtime/Engine/Classes/Engine/LevelScriptActor.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "GameFramework/PlayerStart.h"
#include "GameService/Level/TargetPoint/PlayerSpawnPoint.h"
#include "GameService/Level/LevelScriptActor/GsLevelScriptActor.h"
#include "GameService/Character/GsPlayer.h"
#include "GameObject/ActorExtend/GsLocalCharacter.h"
#include "GameObject/GsSpawnComponent.h"
#include "GameObject/ObjectClass/GsGameObjectLocal.h"


AGsGameModeWorld::AGsGameModeWorld()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGsGameModeWorld::StartToLeaveMap()
{
	UWorld* world = GetWorld();

	if (world)
	{
		world->GetTimerManager().ClearTimer(_ClosestLevelLoadingTimer);
		world->GetTimerManager().ClearTimer(_VisibleLevelsLoadingTimer);
	}
}

void AGsGameModeWorld::StartPlay()
{
	Super::StartPlay();

	SpawnPlayer();
}

void AGsGameModeWorld::TeleportPlayer(FString in_tag, bool in_waitAllLoad)
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
								, &AGsGameModeWorld::OnCheckVisibleLevelsLoadComplete
								, 0.1f, true, 0);
						}
						else
						{
							_ClosestLevel = GetClosestLevel();
							world->GetTimerManager().ClearTimer(_ClosestLevelLoadingTimer);
							world->GetTimerManager().SetTimer(_ClosestLevelLoadingTimer
								, this
								, &AGsGameModeWorld::OnCheckClosestLevelsLoadComplete
								, 0.1f, true, 0);
						}						
					}											
				}
			}		
		}		
	}
}

bool AGsGameModeWorld::IsLoadedVisibleLevels()
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

void AGsGameModeWorld::OnCheckVisibleLevelsLoadComplete()
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

bool AGsGameModeWorld::IsLoadedClosestLevel()
{
	if (_ClosestLevel)
	{
		return !_ClosestLevel->IsStreamingStatePending();
	}

	return false;
}

void AGsGameModeWorld::OnCheckClosestLevelsLoadComplete()
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

void AGsGameModeWorld::SetPlayerUnspawnedState()
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

void AGsGameModeWorld::SpawnPlayer()
{
	UWorld* world = GetWorld();

	if (world)
	{
		//캐릭터 스폰
		//임시로 스폰 포지션을 찾음
		TArray<AActor*> startPos;
		UGameplayStatics::GetAllActorsOfClass(world, APlayerStart::StaticClass(), startPos);

		FString path = TEXT("Blueprint'/Game/Blueprint/GameObject/BP_LocalCharacter.BP_LocalCharacter'");
		if (auto loadObject = StaticLoadObject(UObject::StaticClass(), nullptr, *path))
		{
			if (UBlueprint* castBP = Cast<UBlueprint>(loadObject))
			{
				auto Local = GSpawner()->SpawnObject<UGsGameObjectLocal>(castBP->GeneratedClass, 
					startPos.Top()->GetActorLocation(), FRotator::ZeroRotator);
				if (NULL != Local)
				{
					auto controller = UGameplayStatics::GetPlayerController(world, 0);
					controller->Possess(Local->GetLocalCharacter());
				}
			}
		}
	}
}

void AGsGameModeWorld::SetPlayerSpawendState()
{
	UWorld* world = GetWorld();

	if (world)
	{
	}
}

ULevelStreaming* AGsGameModeWorld::GetClosestLevel()
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