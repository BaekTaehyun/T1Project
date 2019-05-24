// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsGameModeBase.h"
#include "BPFunction/GSBluePrintEnumLobby.h" // FIX: ����
#include "GsGameModeLobby.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API AGsGameModeLobby : public AGsGameModeBase
{
	GENERATED_BODY()
	
public:
	AGsGameModeLobby();
	//AGsGameModeLobby(const FObjectInitializer& ObjectInitializer);

	/** �κ� ���Ӹ�� ��� */
	static AGsGameModeLobby* GetGameModeLobby();

	/** Called when the game starts. */
	virtual void StartPlay() override;

	/** ���� �÷��̾� ��Ʈ�ѷ� ������ */
	class APlayerController* GetLocalPlayerController();
	class AGsUIManager* GetUIManager();

	/** ���߸���ΰ�. ���� �÷ο찡 �޶��� */
	UFUNCTION(BlueprintPure, Category = "Lobby")
	bool IsDevMode() const { return bIsDevMode; }

	/** �������� �Ϸ� �Ǿ��°�(DevMode�� ��� �α��� �Ϸ�) */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	bool IsAccountLoginComplete() const { return bIsAccountLoginComplete; }

	/** ���� ���� �õ�(���̺� ���� �õ�) */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void TryAccountLogin();

	/** �������� �ϷῩ�� */
	UFUNCTION()
	void SetAccountLogin(bool InComplete);

	/** ���� ���� �õ� */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void TrySelectServer(int32 InServerID);

	/** ���õ� ������ ID. GVS ������ �ֱ� ���� ������ �����ص� �� */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SetSelectedServer(int32 InServerID);

	/** ���õ� ������ ID. */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	int32 GetSelectedServer() const { return SelectedServerID; }

	/** DevMode�� �������� �õ� */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void TryDevAccountLogin(FString InName, FString InPassword);

	/** �������� �õ� */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void TryGameLogin();

	// TEST Begin -------------------------------------------------------------------------
	/** Ÿ�̸Ӹ� ���� DownloadRate�� ���� */
	void TestStartDownload();

	/** WidgetClassMap�� ��ϵ� WidgetClass �������� */
	TSubclassOf<class UGsUIWidgetBase> GetWidgetClass(EGS_LOBBY_WIDGET_Enum InType) const;

protected:
	/** Ÿ�̸� �ڵ鷯 */
	void OnRepeatTestTimer();
	// TEST End ---------------------------------------------------------------------------
	void OnLoginComplete(); // Hive �α��� or Dev ��� �������� �α��� �Ϸ�
protected:
	/** ���߸���ΰ�. ���� �ٿ�ε� ���� ��ŵ �� �α������� �ٷ� �Ѿ */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lobby")
	bool bIsDevMode;

	/** ���� ���� �Ϸ�� ���� */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lobby")
	bool bIsAccountLoginComplete;

	/** ���õ� ���� */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lobby")
	int32 SelectedServerID;

	// TEST Begin -------------------------------------------------------------------------
	/** �ٿ�ε��� */
	float DownloadRate;

	/** UI ���α׷����� ���� �׽�Ʈ�� Ÿ�̸� */
	FTimerHandle TestTimerHandle;

	/** Enum���� ��ϵ� UI�� */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lobby")
	TMap<EGS_LOBBY_WIDGET_Enum, TSubclassOf<class UGsUIWidgetBase>> WidgetClassMap;
	// TEST End ---------------------------------------------------------------------------
};
