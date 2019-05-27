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

void AGsGameModeLobby::StartPlay()
{
	Super::StartPlay();

	// 핸들러 등록을 PreInitializeComponents에 넣어보려고 했는데, 이러면 에디터에서 블루프린트 편집 시 불리며 에러가 난다.
	//GMessage()->GetStage().AddUObject(MessageLobby::Stage::LOGIN_COMPLETE, this, &AGsGameModeLobby::OnLoginComplete);
	if (nullptr != GMessage())
	{
#pragma todo("must delete!!!")
		GMessage()->GetStage().AddUObject(MessageLobby::Stage::LOGIN_COMPLETE, this, &AGsGameModeLobby::OnLoginComplete);
		GMessage()->GetStage().AddUObject(MessageLobby::Stage::ENTER_INGAME, this, &AGsGameModeLobby::OnLoadGameScene);
	}

	AGsUIManager* UIManager = GetUIManager();
	if (nullptr != UIManager)
	{
		UIManager->PushByKeyName(FName(TEXT("WindowIntro")));
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
		auto WidgetClass = UIManager->GetWidgetClass(FName(TEXT("WindowAssetDownload")));
		if (nullptr != WidgetClass)
		{
			auto* TargetWidget = UIManager->GetCachedWidget(WidgetClass.Get()->GetPathName());
			if (nullptr != TargetWidget)
			{
				IGsUIEventInterface::Execute_UIEvent_ChangeDownloadRate(TargetWidget, DownloadRate);
			}
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
			auto WidgetClass = UIManager->GetWidgetClass(FName(TEXT("WindowServerSelect")));
			if (nullptr != WidgetClass)
			{
				auto* TargetWidget = UIManager->GetCachedWidget(WidgetClass.Get()->GetPathName());
				if (nullptr != TargetWidget)
				{
					IGsUIEventInterface::Execute_UIEvent_AccountLoginComplete(TargetWidget);
				}
			}
		}
	}
}

void AGsGameModeLobby::TryAccountLogin()
{
#pragma todo("yjchoung: Request Hive Account Login")
	// FIX: 하이브 로그인 요청 로직 처리 필요.
	// 일단은 로그인 완료되었다고 치고 완료 이벤트를 바로 뿌림
	GMessage()->GetHive().SendMessage(MessageLobby::Hive::HIVE_LOGIN_COMPLETE);
}

void AGsGameModeLobby::TryDevAccountLogin(FString InName, FString InPassword)
{
#pragma todo("yjchoung: Request Dev Account Login")
	// FIX: 인증서버 로그인 요청 로직
	// 일단은 로그인 완료되었다고 치고 완료 이벤트를 바로 뿌림
	GMessage()->GetStage().SendMessage(MessageLobby::Stage::LOGIN_COMPLETE);
}

void AGsGameModeLobby::TrySelectServer(int32 InServerID)
{
#pragma todo("yjchoung: Test Code")
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
		auto WidgetClass = UIManager->GetWidgetClass(FName("WindowServerSelect"));
		if (nullptr != WidgetClass)
		{
			auto* TargetWidget = UIManager->GetCachedWidget(WidgetClass.Get()->GetPathName());
			if (nullptr != TargetWidget)
			{
				IGsUIEventInterface::Execute_UIEvent_ServerSelectComplete(TargetWidget, InServerID);
			}
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