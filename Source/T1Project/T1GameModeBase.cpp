// Fill out your copyright notice in the Description page of Project Settings.

#include "T1GameModeBase.h"
#include "T1PlayerController.h"
#include "T1Player.h"
#include "T1Project.h"
#include "ConstructorHelpers.h"

#define USE_DUMMY false

AT1GameModeBase::AT1GameModeBase()
{
	//���� ���Ӹ���� �⺻ ��Ŭ������ �����Ѵ�.
	DefaultPawnClass = AT1Player::StaticClass();

	//���� ���Ӹ���� �⺻ ��Ʈ�ѷ� Ŭ������ �����Ѵ�.
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
// �÷��̾ ���ӿ� �����ϴ� ����(�α���), Editor���� Play
//------------------------------------------------------------------
void AT1GameModeBase::PostLogin(APlayerController * newPlayer)
{
	T1LOG(Warning, TEXT("%s"), *newPlayer->GetFName().ToString());
	Super::PostLogin(newPlayer);
	T1LOG(Warning, TEXT("PostLogin End"));

}


