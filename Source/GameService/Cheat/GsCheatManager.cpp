// Fill out your copyright notice in the Description page of Project Settings.


#include "GsCheatManager.h"
#include "GameMode/GsGameModeWorld.h"
#include "Runtime/Engine/Classes/Engine/World.h"

void UGsCheatManager::TeleportPlayer(FString in_tag)
{	
	UWorld* world = GetWorld();

	if (world)
	{		
		AGameModeBase* mode = world->GetAuthGameMode();

		if (mode)
		{
			AGsGameModeWorld* gameMode = Cast<AGsGameModeWorld>(mode);
			
			if (gameMode)
			{
				gameMode->TeleportPlayer(in_tag);
			}
		}		
	}	
}