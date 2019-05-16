// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "GsCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API UGsCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec, Category = ExecFunctions)
		void TeleportPlayer(FString in_tag);
	
};
