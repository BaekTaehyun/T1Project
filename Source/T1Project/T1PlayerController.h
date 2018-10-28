// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "T1PlayerController.generated.h"

/**
 * 플레이어 모델을 제어하기위한 언리얼 내부클래스
   플레이어와 유저의 인풋의 연결통로라 보면됨
 */
UCLASS()
class T1PROJECT_API AT1PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void PostInitializeComponents() override;
	virtual void Possess(APawn* pawn) override;
protected:
	virtual void BeginPlay() override;
};
