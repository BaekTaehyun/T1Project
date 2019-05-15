// Fill out your copyright notice in the Description page of Project Settings.


#include "GsGameModeBase.h"
#include "GameService.h"

AGsGameModeBase::AGsGameModeBase()
{
	//현재 게임모드의 기본 폰클래스를 지정한다.
	//DefaultPawnClass = AT1Player::StaticClass();

	//현재 게임모드의 기본 컨트롤러 클래스를 지정한다.
	//PlayerControllerClass = AT1PlayerController::StaticClass();

	/*PrimaryActorTick.bTickEvenWhenPaused = true;
	PrimaryActorTick.bCanEverTick = true;*/
}

void AGsGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	GSLOG(Warning, TEXT("InitGame Call"));
}

//------------------------------------------------------------------
// 플레이어가 게임에 입장하는 순간(로그인), Editor에서 Play
//------------------------------------------------------------------
void AGsGameModeBase::PostLogin(APlayerController * newPlayer)
{
	GSLOG(Warning, TEXT("%s"), *newPlayer->GetFName().ToString());
	Super::PostLogin(newPlayer);
	GSLOG(Warning, TEXT("PostLogin End"));
}

void AGsGameModeBase::StartPlay()
{
	GSLOG(Warning, TEXT("StartPlay"));
}

void AGsGameModeBase::Tick(float deltaSeconds)
{
}

