// Fill out your copyright notice in the Description page of Project Settings.

#include "GsGameInstance.h"
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
UGsGameInstance::UGsGameInstance()
{
	GSLOG_S(Warning);
}

// �÷��̸��(�� ��������)������ ȣ��
void UGsGameInstance::Init()
{
	GSLOG_S(Warning);
	Super::Init();

	_manage.InsertInstance(new FGsMessageManager());
	_manage.InsertInstance(new FGsGameFlowManager());
	_manage.InsertInstance(new FGsNetManager());

	for(auto& mng : _manage.Get())
	{
		mng->Initialize();
	}
}

void UGsGameInstance::Shutdown()
{
	for(auto& mng : _manage.Get())
	{
		if (mng.IsValid())
		{
			mng->Finalize();
		}
	}
	_manage.Clear();

	Super::Shutdown();
}



