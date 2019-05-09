// Fill out your copyright notice in the Description page of Project Settings.

#include "GSGameInstance.h"
#include "GameService.h"

//-------------------------------------------------------------------------------
// 게임초기화 순서
//-------------------------------------------------------------------------------
// 1. 게임 앱의 초기화(PlayMode)	:	UGameInstance::Init
// 2. 레벨에 속한 액터의 초기화		:	AActor::PostInitailizeComponents
// 3. 플레이어의 로그인			:	AGameMode::PostLogin
// 4. 게임의 시작					:	AGameMode::StartPlay -> AActor::BeginPlay
//-------------------------------------------------------------------------------

// 플러그인 초기화, 플레이모드 실행시 호출됨 ------- 중복처리 주의
UGSGameInstance::UGSGameInstance()
{
	GSLOG_S(Warning);
}

// 플레이모드(즉 실제게임)에서만 호출
void UGSGameInstance::Init()
{
	GSLOG_S(Warning);
	Super::Init();

	_message = TUniquePtr<GSFMessageManager>(new GSFMessageManager());
	_message->Init();

	_gameMode = TUniquePtr<GSFGameModeManager>(new GSFGameModeManager());
	_gameMode->InitState();
	
}

void UGSGameInstance::Shutdown()
{
	if (_gameMode.IsValid())
	{
		_gameMode->RemoveAll();
		_gameMode = NULL;
	}

	if (_message.IsValid())
	{
		_message->RemoveAll();
		_message = NULL;
	}
	

	Super::Shutdown();
}



