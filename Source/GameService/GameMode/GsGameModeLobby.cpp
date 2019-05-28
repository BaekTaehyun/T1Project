// Fill out your copyright notice in the Description page of Project Settings.


#include "GsGameModeLobby.h"
#include "Kismet/GameplayStatics.h"
#include "Message/GsMessageManager.h"
#include "UI/GsUIManager.h"
#include "GsLobbyComponent.h"


AGsGameModeLobby::AGsGameModeLobby()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// ...
	LobbyComponent = CreateDefaultSubobject<UGsLobbyComponent>(TEXT("LobbyComponent"));
}

void AGsGameModeLobby::StartPlay()
{
	Super::StartPlay();

	// FIX: 준비가 되었다고 메시지를 쏘고 해당 State이면 재생해야할 듯
	AGsUIManager* UIManager = GetUIManager();
	if (nullptr != UIManager)
	{
		UIManager->PushByKeyName(FName(TEXT("WindowIntro")));
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