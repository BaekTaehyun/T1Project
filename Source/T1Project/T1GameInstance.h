// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "T1Project.h"
#include "Engine/GameInstance.h"
#include "T1GameInstance.generated.h"

//-------------------------------------------------------------------------------
// 게임초기화 순서
//-------------------------------------------------------------------------------
// 1. 게임 앱의 초기화				:	UGameInstance::Init
// 2. 레벨에 속한 액터의 초기화		:	AActor::PostInitailizeComponents
// 3. 플레이어의 로그인			:	AGameMode::PostLogin
// 4. 게임의 시작					:	AGameMode::StartPlay -> AActor::BeginPlay
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
