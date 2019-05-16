// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintPlatformLibrary.h"
#include "Class/GsContainer.h"
#include "GameFlow/GsGameFlowManager.h"
#include "Message/GsMessageManager.h"
#include "Net/GsNetManager.h"

#include "GsGameInstance.generated.h"

class IGsManager;

//-------------------------------------------------------------------------------
// �����ʱ�ȭ ����
//-------------------------------------------------------------------------------
// 1. ���� ���� �ʱ�ȭ(PlayMode)	:	UGameInstance::Init
// 2. ������ ���� ������ �ʱ�ȭ		:	AActor::PostInitailizeComponents
// 3. �÷��̾��� �α���			:	AGameMode::PostLogin
// 4. ������ ����					:	AGameMode::StartPlay -> AActor::BeginPlay
//-------------------------------------------------------------------------------

UCLASS()
class GAMESERVICE_API UGsGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	TGsArray<IGsManager> _manage;
	FTimerHandle		 _manageTickHandle;
public:
	UGsGameInstance();
	virtual void Init() override;
	virtual void Shutdown() override;
	void Update();
};
