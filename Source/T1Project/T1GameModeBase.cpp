// Fill out your copyright notice in the Description page of Project Settings.

#include "T1GameModeBase.h"
#include "T1PlayerController.h"
#include "T1Player.h"
#include "T1Project.h"
#include "ConstructorHelpers.h"

#define USE_DUMMY false

AT1GameModeBase::AT1GameModeBase()
{
	//현재 게임모드의 기본 폰클래스를 지정한다.
	DefaultPawnClass = AT1Player::StaticClass();

	//현재 게임모드의 기본 컨트롤러 클래스를 지정한다.
	PlayerControllerClass = AT1PlayerController::StaticClass();

#if USE_DUMMY

	static ConstructorHelpers::FClassFinder<APawn> BP_DUMMY_C(TEXT("/Game/ThirdPersonBP/Blueprints/ThirdPersonCharacter.ThirdPersonCharacter_C"));
	if(BP_DUMMY_C.Succeeded())
	{
		DefaultPawnClass = BP_DUMMY_C.Class;
	}
#endif

}

//------------------------------------------------------------------
// 플레이어가 게임에 입장하는 순간(로그인), Editor에서 Play
//------------------------------------------------------------------
void AT1GameModeBase::PostLogin(APlayerController * newPlayer)
{
	T1LOG(Warning, TEXT("%s"), *newPlayer->GetFName().ToString());
	Super::PostLogin(newPlayer);
	T1LOG(Warning, TEXT("PostLogin End"));

}


