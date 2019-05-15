// Fill out your copyright notice in the Description page of Project Settings.


#include "GsWorldGameMode.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/WorldComposition.h"
#include "Runtime/Engine/Classes/GameFramework/Character.h"
#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"
#include "GameService/Character/GsPlayer.h"

AGsWorldGameMode::AGsWorldGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGsWorldGameMode::Tick(float in_delta)
{
	Super::Tick(in_delta);

	if (_bCheckWorldComponentLoad)
	{
		WorldCompositionTick();
	}
}

void AGsWorldGameMode::StartToLeaveMap()
{
	_bCheckWorldComponentLoad = false;
}

void AGsWorldGameMode::TeleportPlayer(FString in_tag)
{
	_bCheckWorldComponentLoad = true;

	SetPlayerUnspawnedState();
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

void AGsWorldGameMode::WorldCompositionTick()
{
	if (IsWorldCompositionLoadComplete())
	{
		SetPlayerSpawendState();

		_bCheckWorldComponentLoad = false;
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
