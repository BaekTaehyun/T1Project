// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintPlatformLibrary.h"
#include "Class/GsContainer.h"
#include "GameFlow/GsGameFlowManager.h"
#include "Message/GsMessageManager.h"
#include "Net/GsNetManager.h"

#include "GsGameInstance.generated.h"

class IGsManager;

//-------------------------------------------------------------------------------
// 게임초기화 순서
//-------------------------------------------------------------------------------
// 1. 게임 앱의 초기화(PlayMode)	:	UGameInstance::Init
// 2. 레벨에 속한 액터의 초기화		:	AActor::PostInitailizeComponents
// 3. 플레이어의 로그인			:	AGameMode::PostLogin
// 4. 게임의 시작					:	AGameMode::StartPlay -> AActor::BeginPlay
//-------------------------------------------------------------------------------

UCLASS()
class GAMESERVICE_API UGsGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	TGsArray<IGsManager> _manage;
	FTimerHandle		 _manageTickHandle;
public:
	UGsGameInstance();
	virtual void Init() override;
	virtual void Shutdown() override;
	void Update();
};
