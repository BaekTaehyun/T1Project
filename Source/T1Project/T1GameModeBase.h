// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "T1GameModeBase.generated.h"

/**
 * ������ Ʋ�� ����ִ� ��Ȱ
   1. �÷��̾� ����
   2. ������ �÷��̾ ������ ��Ʈ�ѷ� �Ҵ�
 */
UCLASS()
class T1PROJECT_API AT1GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	AT1GameModeBase();

public:

	// �÷��̾ ���ӿ� �����ϴ� ����(�α���), Editor���� Play
	virtual void PostLogin(APlayerController* newPlayer) override;
};
