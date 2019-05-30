// Fill out your copyright notice in the Description page of Project Settings.


#include "GsGameModeBase.h"
#include "GameService.h"
#include "GameObject/GsSpawnComponent.h"

AGsGameModeBase::AGsGameModeBase()
{
	//���� ���Ӹ���� �⺻ ��Ŭ������ �����Ѵ�.
	//DefaultPawnClass = AT1Player::StaticClass();

	//���� ���Ӹ���� �⺻ ��Ʈ�ѷ� Ŭ������ �����Ѵ�.
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
// �÷��̾ ���ӿ� �����ϴ� ����(�α���), Editor���� Play
//------------------------------------------------------------------
void AGsGameModeBase::PostLogin(APlayerController * newPlayer)
{
	GSLOG(Warning, TEXT("%s"), *newPlayer->GetFName().ToString());
	Super::PostLogin(newPlayer);
	GSLOG(Warning, TEXT("PostLogin End"));
}

void AGsGameModeBase::StartPlay()
{
	Super::StartPlay();

	GSLOG(Warning, TEXT("StartPlay"));
	
	_SpawnComponent = NewObject<UGsSpawnComponent>(this, NAME_None, RF_Transient);
	_SpawnComponent->RegisterComponent();
}

void AGsGameModeBase::Tick(float deltaSeconds)
{
}

