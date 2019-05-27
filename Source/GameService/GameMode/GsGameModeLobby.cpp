// Fill out your copyright notice in the Description page of Project Settings.


#include "GsGameModeLobby.h"
#include "Kismet/GameplayStatics.h"
#include "Message/GsMessageManager.h"
#include "UI/GsUIManager.h"
#include "UI/GsUIEventInterface.h"


AGsGameModeLobby::AGsGameModeLobby()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bIsDevMode = false;
	bIsAccountLoginComplete = false;
	SelectedServerID = 0;

}

void AGsGameModeLobby::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	// 이벤트 핸들러 등록
	if (nullptr != GMessage())
	{
#pragma todo("must delete!!!")
		GMessage()->GetStage().AddUObject(MessageLobby::Stage::LOGIN_COMPLETE, this, &AGsGameModeLobby::OnLoginComplete);
		GMessage()->GetStage().AddUObject(MessageLobby::Stage::ENTER_INGAME, this, &AGsGameModeLobby::OnLoadGameScene);
	}
}

void AGsGameModeLobby::StartPlay()
{
	Super::StartPlay();

	AGsUIManager* UIManager = GetUIManager();
	if (nullptr != UIManager)
	{
		UIManager->Push(GetWidgetClass(EGS_LOBBY_WIDGET_Enum::GS_LOBBY_WIDGET_INTRO));
	}
}

AGsGameModeLobby* AGsGameModeLobby::GetGameModeLobby()
{
	UWorld* World = GEngine->GameViewport->GetWorld();
	if (nullptr != World)
	{
		AGameModeBase* GameMode = UGameplayStatics::GetGameMode(World);
		if (nullptr != GameMode)
		{
			return Cast<AGsGameModeLobby>(GameMode);
		}
	}

	return nullptr;
}

void AGsGameModeLobby::TestStartDownload()
{
	DownloadRate = 0.0f;

	GetWorld()->GetTimerManager().SetTimer(TestTimerHandle, this,
		&AGsGameModeLobby::OnRepeatTestTimer, 0.2f, true);
}

void AGsGameModeLobby::OnRepeatTestTimer()
{
	DownloadRate += 0.1f;

	// 다운로드 100프로 상황
	if (1.0f < DownloadRate)
	{
		DownloadRate = 1.0f;

		GetWorld()->GetTimerManager().ClearTimer(TestTimerHandle);

		GMessage()->GetStage().SendMessage(MessageLobby::Stage::ASSETDOWN_COMPLETE);
	}

	AGsUIManager* UIManager = GetUIManager();
	if (nullptr != UIManager)
	{
		auto WidgetClass = GetWidgetClass(EGS_LOBBY_WIDGET_Enum::GS_LOBBY_WIDGET_ASSET_DOWNLOAD);
		auto* TargetWidget = UIManager->GetCachedWidget(WidgetClass.Get()->GetPathName());
		if (nullptr != TargetWidget)
		{
			IGsUIEventInterface::Execute_UIEvent_ChangeDownloadRate(TargetWidget, DownloadRate);
		}
	}
}


void AGsGameModeLobby::OnLoginComplete()
{
	GSLOG(Warning, TEXT("AGsGameModeLobby : OnLoginComplete"));
	SetAccountLogin(true);
}

void AGsGameModeLobby::OnLoadGameScene()
{
	GSLOG(Warning, TEXT("AGsGameModeLobby : OnLoadGameScene"));
	UGameplayStatics::OpenLevel((UObject*)GetGameInstance(), FName(TEXT("WorldCompositionTest")), true, TEXT("?game=/Game/Blueprint/GameMode/BP_WorldMode.BP_WorldMode_C"));
}

void AGsGameModeLobby::TryGameLogin()
{
	// FIX: 서버에 로그인 정보, 서버정보를 통해 로그인 시도
	GMessage()->GetStage().SendMessage(MessageLobby::Stage::SERVER_SELECTCOMPLETE);
}

void AGsGameModeLobby::SetAccountLogin(bool InComplete)
{
	bIsAccountLoginComplete = InComplete;

	if (bIsAccountLoginComplete)
	{
		AGsUIManager* UIManager = GetUIManager();
		if (nullptr != UIManager)
		{
			auto WidgetClass = GetWidgetClass(EGS_LOBBY_WIDGET_Enum::GS_LOBBY_WIDGET_SERVER_SELECT);
			auto* TargetWidget = UIManager->GetCachedWidget(WidgetClass.Get()->GetPathName());
			if (nullptr != TargetWidget)
			{
				IGsUIEventInterface::Execute_UIEvent_AccountLoginComplete(TargetWidget);
			}
		}
	}
}

void AGsGameModeLobby::TryAccountLogin()
{
	// FIX: 하이브 로그인 요청 로직

	// 일단은 하이브 로그인 완료 이벤트를 바로 뿌림
	GMessage()->GetHive().SendMessage(MessageLobby::Hive::HIVE_LOGIN_COMPLETE);
}

void AGsGameModeLobby::TryDevAccountLogin(FString InName, FString InPassword)
{
	// FIX: 인증서버 로그인 요청 로직

	// 일단은 로그인 완료 이벤트를 바로 뿌림
	GMessage()->GetStage().SendMessage(MessageLobby::Stage::LOGIN_COMPLETE);
}

void AGsGameModeLobby::TrySelectServer(int32 InServerID)
{
	// FIX: 서버와 통신이 필요한가?

	// TEST: 임의로 무조건 1값을 세팅(0보다 크면 선택된 서버가 있다고 판단)
	SetSelectedServer(1);
	//SetSelectedServer(InServerID);
}

void AGsGameModeLobby::SetSelectedServer(int32 InServerID)
{
	SelectedServerID = InServerID;

	AGsUIManager* UIManager = GetUIManager();
	if (nullptr != UIManager)
	{
		auto WidgetClass = GetWidgetClass(EGS_LOBBY_WIDGET_Enum::GS_LOBBY_WIDGET_SERVER_SELECT);
		auto* TargetWidget = UIManager->GetCachedWidget(WidgetClass.Get()->GetPathName());
		if (nullptr != TargetWidget)
		{
			IGsUIEventInterface::Execute_UIEvent_ServerSelectComplete(TargetWidget, InServerID);
		}
	}
}

APlayerController* AGsGameModeLobby::GetLocalPlayerController()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PlayerController = It->Get();
		if (nullptr != PlayerController)
		{
			if (PlayerController->IsLocalController())
			{
				return PlayerController;
			}
		}
	}

	return nullptr;
}

AGsUIManager* AGsGameModeLobby::GetUIManager()
{
	APlayerController* LocalPC = GetLocalPlayerController();
	if (nullptr != LocalPC)
	{
		return Cast<AGsUIManager>(LocalPC->GetHUD());
	}

	return nullptr;
}

TSubclassOf<class UGsUIWidgetBase> AGsGameModeLobby::GetWidgetClass(EGS_LOBBY_WIDGET_Enum InType) const
{
	// FIX: Find에서 키를 못찾으면 에러가 나서 검사. 다른 함수를 써야하는지 알아볼 것.
	if (WidgetClassMap.Contains(InType))
	{
		return *WidgetClassMap.Find(InType);
	}

	return nullptr;
}