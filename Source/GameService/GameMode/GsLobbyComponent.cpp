// Fill out your copyright notice in the Description page of Project Settings.


#include "GsLobbyComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Message/GsMessageManager.h"
#include "Message/GsMessageLobby.h"
#include "GsGameModeLobby.h"
#include "UI/GsUIManager.h"
#include "GameService.h"
#include "GSGameInstance.h"

// Sets default values for this component's properties
UGsLobbyComponent::UGsLobbyComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	bIsAccountLoginComplete = false;
	SelectedServerID = 0;
}

// Called when the game starts
void UGsLobbyComponent::BeginPlay()
{
	Super::BeginPlay();

	if (nullptr != GMessage())
	{
		// 메시지 핸들러 등록
		GMessage()->GetStage().AddUObject(MessageLobby::Stage::ENTER_ASSETDOWNLOAD_STAGE, this, &UGsLobbyComponent::OnEnterAssetDownloadStage);
		GMessage()->GetStage().AddUObject(MessageLobby::Stage::ENTER_SERVERSELECT_STAGE, this, &UGsLobbyComponent::OnEnterServerSelectStage);
		GMessage()->GetStage().AddUObject(MessageLobby::Stage::ENTER_CHARACTERSELECT_STAGE, this, &UGsLobbyComponent::OnEnterCharacterSelectStage);

		GMessage()->GetStage().AddUObject(MessageLobby::Stage::LOGIN_COMPLETE, this, &UGsLobbyComponent::OnLoginComplete);
		GMessage()->GetStage().AddUObject(MessageLobby::Stage::ENTER_INGAME, this, &UGsLobbyComponent::OnLoadGameScene);
	}
}

void UGsLobbyComponent::OnEnterAssetDownloadStage()
{
	AGsGameModeLobby* GameMode = GetGameModeLobby();
	if (nullptr != GameMode)
	{
		UGsGameInstance* GameInstance = Cast<UGsGameInstance>(GameMode->GetGameInstance());
		if (nullptr != GameInstance)
		{
			// 즉시시작 플래그가 켜져있으면 바로 인게임으로 보낸다
			if (GameInstance->IsImmediateStart())
			{
				GMessage()->GetStage().SendMessage(MessageLobby::Stage::ENTER_INGAME);
			}

			// DevMode면 다운로드 끝났다고 처리하고 넘긴다
			if (GameInstance->IsDevMode())
			{
				GMessage()->GetStage().SendMessage(MessageLobby::Stage::ASSETDOWN_COMPLETE);
				return;
			}
		}
	}

	UGsUIManager* uiManager = GetUIManager();
	if (nullptr != uiManager)
	{
		WindowAssetDownload = uiManager->PushAndGetWidget(FName(TEXT("WindowAssetDownload")));

#pragma todo("yjchoung: Test Code")
		// TEST: 다운로드 시작
		TestStartDownload();
	}
}

void UGsLobbyComponent::OnEnterServerSelectStage()
{
	UGsUIManager* uiManager = GetUIManager();
	if (nullptr != uiManager)
	{
		WindowServerSelect = uiManager->PushAndGetWidget(FName(TEXT("WindowServerSelect")));
	}
}

void UGsLobbyComponent::OnEnterCharacterSelectStage()
{
	UGsUIManager* uiManager = GetUIManager();
	if (nullptr != uiManager)
	{
		uiManager->Push(FName(TEXT("WindowCharacterSelect")));
	}
}

void UGsLobbyComponent::OnLoginComplete()
{
	GSLOG(Warning, TEXT("AGsGameModeLobby : OnLoginComplete"));
	SetAccountLogin(true);
}

void UGsLobbyComponent::OnLoadGameScene()
{
	GSLOG(Warning, TEXT("AGsGameModeLobby : OnLoadGameScene"));

	UGsGameInstance* Inst = Cast<UGsGameInstance>(GetWorld()->GetGameInstance());
	if (nullptr != Inst)
	{
		Inst->GetUIManager()->OnChangeLevel();
	}

	AGsGameModeLobby* GameMode = GetGameModeLobby();
	if (nullptr != GameMode)
	{
		UGameplayStatics::OpenLevel((UObject*)GameMode->GetGameInstance(), FName(TEXT("WorldCompositionTest")), true, TEXT("?game=/Game/Blueprint/GameMode/BP_WorldMode.BP_WorldMode_C"));
	}
}

void UGsLobbyComponent::TryAccountLogin()
{
#pragma todo("yjchoung: Request Hive Account Login")
	// FIX: 하이브 로그인 요청 로직 처리 필요.
	// 일단은 로그인 완료되었다고 치고 완료 이벤트를 바로 뿌림
	GMessage()->GetHive().SendMessage(MessageLobby::Hive::HIVE_LOGIN_COMPLETE);
}

void UGsLobbyComponent::TryDevAccountLogin(FString InName, FString InPassword)
{
#pragma todo("yjchoung: Request Dev Account Login")
	// FIX: 인증서버 로그인 요청 로직
	// 일단은 로그인 완료되었다고 치고 완료 이벤트를 바로 뿌림
	GMessage()->GetStage().SendMessage(MessageLobby::Stage::LOGIN_COMPLETE);
}

void UGsLobbyComponent::SetAccountLogin(bool InComplete)
{
	bIsAccountLoginComplete = InComplete;

	if (bIsAccountLoginComplete)
	{
		if (WindowServerSelect.IsValid() &&
			WindowServerSelect.Get()->IsInViewport())
		{
			IGsUIEventInterface::Execute_UIEvent_AccountLoginComplete(WindowServerSelect.Get());
		}
	}
}

void UGsLobbyComponent::TrySelectServer(int32 InServerID)
{
#pragma todo("yjchoung: Test Code")
	// TEST: 임의로 무조건 1값을 세팅(0보다 크면 선택된 서버가 있다고 판단)
	SetSelectedServer(1);
	//SetSelectedServer(InServerID);
}

void UGsLobbyComponent::SetSelectedServer(int32 InServerID)
{
	SelectedServerID = InServerID;

	if (WindowServerSelect.IsValid() &&
		WindowServerSelect.Get()->IsInViewport())
	{
		IGsUIEventInterface::Execute_UIEvent_ServerSelectComplete(WindowServerSelect.Get(), InServerID);
	}
}

void UGsLobbyComponent::TryGameLogin()
{
	// FIX: 서버에 로그인 정보, 서버정보를 통해 로그인 시도
	GMessage()->GetStage().SendMessage(MessageLobby::Stage::SERVER_SELECTCOMPLETE);
}

AGsGameModeLobby* UGsLobbyComponent::GetGameModeLobby()
{
	if (GameModeLobby.IsValid())
	{
		return GameModeLobby.Get();
	}

	AActor* Owner = GetOwner();
	if (nullptr != Owner)
	{
		GameModeLobby = Cast<AGsGameModeLobby>(Owner);
		return GameModeLobby.Get();
	}

	return nullptr;
}

UGsUIManager* UGsLobbyComponent::GetUIManager()
{
	UGsGameInstance* gameInstance = Cast<UGsGameInstance>(GetWorld()->GetGameInstance());
	if (nullptr != gameInstance)
	{
		return gameInstance->GetUIManager();
	}

	return nullptr;
}

void UGsLobbyComponent::TestStartDownload()
{
	DownloadRate = 0.0f;

	GetWorld()->GetTimerManager().SetTimer(TestTimerHandle, this,
		&UGsLobbyComponent::OnRepeatTestTimer, 0.2f, true);
}

void UGsLobbyComponent::OnRepeatTestTimer()
{
	DownloadRate += 0.1f;

	// 다운로드 100프로 상황
	if (1.0f < DownloadRate)
	{
		DownloadRate = 1.0f;

		GetWorld()->GetTimerManager().ClearTimer(TestTimerHandle);

		GMessage()->GetStage().SendMessage(MessageLobby::Stage::ASSETDOWN_COMPLETE);
	}

	if (WindowAssetDownload.IsValid() &&
		WindowAssetDownload.Get()->IsInViewport())
	{
		IGsUIEventInterface::Execute_UIEvent_ChangeDownloadRate(WindowAssetDownload.Get(), DownloadRate);
	}
}
