// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "../GsGameModeBase.h"
#include "GsGameMode.generated.h"


/**
 * 
 */
UCLASS()
class GAMESERVICE_API AGsGameMode : public AGsGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void TeleportPlayer(FString in_Tag, bool in_waitAllLoad = false){}
};
