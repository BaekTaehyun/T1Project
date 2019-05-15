// Fill out your copyright notice in the Description page of Project Settings.


#include "GsCheatManager.h"
#include "GameMode/GsGameMode.h"
#include "Runtime/Engine/Classes/Engine/World.h"

void UGsCheatManager::TeleportPlayer(FString in_tag)
{	
	UWorld* world = GetWorld();

	if (world)
	{		
		AGsGameMode* mode = world->GetAuthGameMode();

		if (mode)
		{			
			mode->TeleportPlayer(in_tag);
		}
	}	
}