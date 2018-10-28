// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "T1PlayerController.generated.h"

/**
 * �÷��̾� ���� �����ϱ����� �𸮾� ����Ŭ����
   �÷��̾�� ������ ��ǲ�� ������ζ� �����
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
