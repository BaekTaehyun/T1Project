// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintPlatformLibrary.h"
#include "Class/GsContainer.h"
#include "GameFlow/GsGameFlowManager.h"
#include "Message/GsMessageManager.h"
#include "Net/GsNetManager.h"
#include "GameObject/Item/ItemManager.h"


#include "GsGameInstance.generated.h"

class IGsManager;

//-------------------------------------------------------------------------------
// 게임초기화 순서
//-------------------------------------------------------------------------------
// 1. 게임 앱의 초기화(PlayMode)	:	UGameInstance::Init
// 2. 레벨에 속한 액터의 초기화		:	AActor::PostInitailizeComponents
// 3. 플레이어의 로그인			:	AGameMode::PostLogin
// 4. 게임의 시작					:	AGameMode::StartPlay -> AActor::BeginPlay
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

	// 개발모드인가
	UFUNCTION(BlueprintPure, Category = "Dev")
	bool IsDevMode() const { return bIsDevMode; }

	// 즉시시작 모드인가
	UFUNCTION(BlueprintPure, Category = "Dev")
	bool IsImmediateStart() const { return bImmediateStart; }

	// 9월 시연 모드인가
	UFUNCTION(BlueprintPure, Category = "Dev")
	bool IsDemoMode() const { return bIsDemoMode; }

	UFUNCTION(BlueprintCallable, Category = "Dev")
	class UGsUIManager* GetUIManager() const { return UIManager; }

protected:

	// 개발모드
	UPROPERTY(EditDefaultsOnly, Category = "Dev")
	bool bIsDevMode;

	// 즉시시작
	UPROPERTY(EditDefaultsOnly, Category = "Dev")
	bool bImmediateStart;

	// 9월 시연 모드
	UPROPERTY(EditDefaultsOnly, Category = "Dev")
	bool bIsDemoMode;

	// UI매니저
	UPROPERTY(Transient)
	class UGsUIManager* UIManager;
};
