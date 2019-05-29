// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GsTestGameMode.generated.h"

/**
 * 
 */
class GsDivergenceVersion;
UCLASS()
class GAMESERVICE_API AGsTestGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	// 다이버전스 버전
	TSharedPtr<GsDivergenceVersion> _divergenceVersion;
};
DECLARE_LOG_CATEGORY_EXTERN(LogDownloadMode, Log, All);