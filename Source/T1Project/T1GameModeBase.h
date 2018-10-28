// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "T1GameModeBase.generated.h"

/**
 * 게임의 틀을 잡아주는 역활
   1. 플레이어 점검
   2. 입장한 플레이어가 조종할 컨트롤러 할당
 */
UCLASS()
class T1PROJECT_API AT1GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	AT1GameModeBase();

public:

	// 플레이어가 게임에 입장하는 순간(로그인), Editor에서 Play
	virtual void PostLogin(APlayerController* newPlayer) override;
};
