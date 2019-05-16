// Fill out your copyright notice in the Description page of Project Settings.

#include "GsGameInstance.h"
#include "GameService.h"
#include "Runtime/Engine/Classes/Engine/World.h"

//-------------------------------------------------------------------------------
// 게임초기화 순서
//-------------------------------------------------------------------------------
// 1. 게임 앱의 초기화(PlayMode)	:	UGameInstance::Init
// 2. 레벨에 속한 액터의 초기화		:	AActor::PostInitailizeComponents
// 3. 플레이어의 로그인			:	AGameMode::PostLogin
// 4. 게임의 시작					:	AGameMode::StartPlay -> AActor::BeginPlay
//-------------------------------------------------------------------------------

// 플러그인 초기화, 플레이모드 실행시 호출됨 ------- 중복처리 주의
UGsGameInstance::UGsGameInstance()
{
	GSLOG_S(Warning);
}

// 플레이모드(즉 실제게임)에서만 호출
void UGsGameInstance::Init()
{
	GSLOG_S(Warning);
	Super::Init();

	_manage.InsertInstance(new FGsMessageManager());
	_manage.InsertInstance(new FGsGameFlowManager());
	_manage.InsertInstance(new FGsNetManager());
	//_manage.InsertInstance(NewObject<UGsObjectSpawner>());

	spawner = NewObject<UGsObjectSpawner>();
	spawner->Initialize();
	

	for(auto& mng : _manage.Get())
	{
		mng->Initialize();
	}

	GetTimerManager().SetTimer(_manageTickHandle, this, &UGsGameInstance::Update, 0.5f, true, 0.0f);

	//[LBY]
	//임의 월드 셋팅을 명시적으로 해준다. 추후 씬병경 관련 노티를 받아 처리 변경 필요
	GSpawner()->SetWorld(GetWorld());
}

void UGsGameInstance::Shutdown()
{
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
	GSLOG_S(Warning);
	for (auto& mng : _manage.Get())
	{
		if (mng.IsValid())
		{
			mng->Update();
		}
	}

	spawner->Update();
}



