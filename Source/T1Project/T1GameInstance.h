// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "T1Project.h"
#include "Engine/GameInstance.h"
#include "T1GameInstance.generated.h"

//-------------------------------------------------------------------------------
// �����ʱ�ȭ ����
//-------------------------------------------------------------------------------
// 1. ���� ���� �ʱ�ȭ				:	UGameInstance::Init
// 2. ������ ���� ������ �ʱ�ȭ		:	AActor::PostInitailizeComponents
// 3. �÷��̾��� �α���			:	AGameMode::PostLogin
// 4. ������ ����					:	AGameMode::StartPlay -> AActor::BeginPlay
//-------------------------------------------------------------------------------
UCLASS()
class T1PROJECT_API UT1GameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UT1GameInstance();

	virtual void Init() override;
	struct FT1PlayerData* GetT1PlayerData(int32 Level);
	
private:
	UPROPERTY()
	class UDataTable* T1PlayerTable;
};
