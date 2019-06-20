// Fill out your copyright notice in the Description page of Project Settings.

#include "GsGameInstance.h"
#include "GameService.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "class/GsSpawn.h"
#include "UI/GsUIManager.h"
#include "GameObject/GsGameObjectDataCenter.h"

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

	bIsDevMode = false;
	bImmediateStart = false;
}

// �÷��̸��(�� ��������)������ ȣ��
void UGsGameInstance::Init()
{
	GSLOG_S(Warning);
	Super::Init();

	_manage.InsertInstance(new FGsMessageManager());
	_manage.InsertInstance(new FGsGameFlowManager());
	_manage.InsertInstance(new FGsNetManager());
	_manage.InsertInstance(new FGsGameObjectDataCenter());
	_manage.InsertInstance(new FItemManager());

	for(auto& mng : _manage.Get())
	{
		mng->Initialize();
	}

	GetTimerManager().SetTimer(_manageTickHandle, this, &UGsGameInstance::Update, 0.5f, true, 0.0f);

	if (nullptr == UIManager)
	{
		UIManager = NewObject<UGsUIManager>(this);
		UIManager->Initialize();
	}
}

void UGsGameInstance::Shutdown()
{
	UIManager = nullptr;

	GetTimerManager().ClearTimer(_manageTickHandle);

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

void UGsGameInstance::Update()
{
	//GSLOG_S(Warning);
	for (auto& mng : _manage.Get())
	{
		if (mng.IsValid())
		{
			mng->Update();
		}
	}
}



