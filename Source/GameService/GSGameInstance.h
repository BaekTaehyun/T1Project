// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintPlatformLibrary.h"
#include "GameMode/GSGameModeManager.h"
#include "GSGameInstance.generated.h"

//-------------------------------------------------------------------------------
// �����ʱ�ȭ ����
//-------------------------------------------------------------------------------
// 1. ���� ���� �ʱ�ȭ(PlayMode)	:	UGameInstance::Init
// 2. ������ ���� ������ �ʱ�ȭ		:	AActor::PostInitailizeComponents
// 3. �÷��̾��� �α���			:	AGameMode::PostLogin
// 4. ������ ����					:	AGameMode::StartPlay -> AActor::BeginPlay
//-------------------------------------------------------------------------------

UCLASS()
class GAMESERVICE_API UGSGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	TUniquePtr<GSGameModeManager>	_gameMode;
	
public:
	UGSGameInstance();
	virtual void Init() override;
	virtual void Shutdown() override;
};
