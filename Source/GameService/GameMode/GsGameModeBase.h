// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GsGameModeBase.generated.h"

class UGsSpawnComponent;
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
	virtual ~AGsGameModeBase();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	// �÷��̾ ���ӿ� �����ϴ� ����(�α���), Editor���� Play
	virtual void PostLogin(APlayerController* newPlayer) override;

	virtual void TeleportPlayer(FString in_Tag, bool in_waitAllLoad = false) {}

	virtual void StartPlay() override;
	virtual void Tick(float deltaSeconds);

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, Meta = (AllowPrivateAccess = true))
	UGsSpawnComponent*	_SpawnComponent;
};
