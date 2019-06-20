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

	// ���߸���ΰ�
	UFUNCTION(BlueprintPure, Category = "Dev")
	bool IsDevMode() const { return bIsDevMode; }

	// ��ý��� ����ΰ�
	UFUNCTION(BlueprintPure, Category = "Dev")
	bool IsImmediateStart() const { return bImmediateStart; }

	// 9�� �ÿ� ����ΰ�
	UFUNCTION(BlueprintPure, Category = "Dev")
	bool IsDemoMode() const { return bIsDemoMode; }

	UFUNCTION(BlueprintCallable, Category = "Dev")
	class UGsUIManager* GetUIManager() const { return UIManager; }

protected:

	// ���߸��
	UPROPERTY(EditDefaultsOnly, Category = "Dev")
	bool bIsDevMode;

	// ��ý���
	UPROPERTY(EditDefaultsOnly, Category = "Dev")
	bool bImmediateStart;

	// 9�� �ÿ� ���
	UPROPERTY(EditDefaultsOnly, Category = "Dev")
	bool bIsDemoMode;

	// UI�Ŵ���
	UPROPERTY(Transient)
	class UGsUIManager* UIManager;
};
