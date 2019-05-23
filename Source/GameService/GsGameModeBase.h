// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GsGameModeBase.generated.h"

/**
 * ������ Ʋ�� ����ִ� ��Ȱ
   1. �÷��̾� ����
   2. ������ �÷��̾ ������ ��Ʈ�ѷ� �Ҵ�
 */
UCLASS()
class GAMESERVICE_API AGsGameModeBase : public AGameModeBase
{
	GENERATED_BODY()	
public:
	AGsGameModeBase();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	// �÷��̾ ���ӿ� �����ϴ� ����(�α���), Editor���� Play
	virtual void PostLogin(APlayerController* newPlayer) override;

	virtual void StartPlay() override;
	virtual void Tick(float deltaSeconds);
};
