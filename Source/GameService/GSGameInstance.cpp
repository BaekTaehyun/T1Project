// Fill out your copyright notice in the Description page of Project Settings.

#include "GSGameInstance.h"
#include "GameService.h"

//-------------------------------------------------------------------------------
// �����ʱ�ȭ ����
//-------------------------------------------------------------------------------
// 1. ���� ���� �ʱ�ȭ(PlayMode)	:	UGameInstance::Init
// 2. ������ ���� ������ �ʱ�ȭ		:	AActor::PostInitailizeComponents
// 3. �÷��̾��� �α���			:	AGameMode::PostLogin
// 4. ������ ����					:	AGameMode::StartPlay -> AActor::BeginPlay
//-------------------------------------------------------------------------------

// �÷����� �ʱ�ȭ, �÷��̸�� ����� ȣ��� ------- �ߺ�ó�� ����
UGSGameInstance::UGSGameInstance()
{
	GSLOG_S(Warning);
}

// �÷��̸��(�� ��������)������ ȣ��
void UGSGameInstance::Init()
{
	GSLOG_S(Warning);
	Super::Init();

	_gameMode = TUniquePtr<GSGameModeManager>(new GSGameModeManager());
	_gameMode.Get()->InitState();
}

void UGSGameInstance::Shutdown()
{
	Super::Shutdown();
	if (_gameMode.IsValid())
	{
		_gameMode->RemoveAll();
		_gameMode = NULL;
	}
	
}



