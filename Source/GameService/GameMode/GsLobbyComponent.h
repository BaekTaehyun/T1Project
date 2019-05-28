// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GsLobbyComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMESERVICE_API UGsLobbyComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGsLobbyComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// �������� �Ϸ� �Ǿ��°�(DevMode�� ��� �α��� �Ϸ�)
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	bool IsAccountLoginComplete() const { return bIsAccountLoginComplete; }

	// ���� ���� �õ�(���̺� ���� �õ�)
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void TryAccountLogin();

	// DevMode�� �������� �õ�
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void TryDevAccountLogin(FString InName, FString InPassword);

	// �������� �ϷῩ��
	UFUNCTION()
	void SetAccountLogin(bool InComplete);

	// ���� ���� �õ�
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void TrySelectServer(int32 InServerID);

	// ���õ� ������ ID. GVS ������ �ֱ� ���� ������ �����ص� ��
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SetSelectedServer(int32 InServerID);

	// ���õ� ������ ID
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	int32 GetSelectedServer() const { return SelectedServerID; }

	// �������� �õ�
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void TryGameLogin();
	
protected:
	// �޼��� �ڵ鷯
	void OnEnterAssetDownloadStage();
	void OnEnterServerSelectStage();
	void OnEnterCharacterSelectStage();
	void OnLoginComplete(); // Hive �α��� or Dev ��� �������� �α��� �Ϸ�
	void OnLoadGameScene();

	class AGsGameModeLobby* GetGameModeLobby();
	class AGsUIManager* GetUIManager();

	// TEST Begin -------------------------------------------------------------------------
	// Ÿ�̸Ӹ� ���� DownloadRate�� ����
	void TestStartDownload();

	// Ÿ�̸� �ڵ鷯
	void OnRepeatTestTimer(); 

	// �ٿ�ε���
	float DownloadRate;

	// UI ���α׷����� ���� �׽�Ʈ�� Ÿ�̸�
	FTimerHandle TestTimerHandle;
	// TEST End ---------------------------------------------------------------------------

protected:
	// ���� ������ �Ϸ�� ����
	bool bIsAccountLoginComplete;

	// ���õ� ����
	int32 SelectedServerID;
};
