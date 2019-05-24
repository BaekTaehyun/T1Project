// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsGameModeBase.h"
#include "BPFunction/GSBluePrintEnumLobby.h" // FIX: 개선
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

	/** 로비 게임모드 얻기 */
	static AGsGameModeLobby* GetGameModeLobby();

	/** Called when the game starts. */
	virtual void StartPlay() override;

	/** 로컬 플레이어 컨트롤러 얻어오기 */
	class APlayerController* GetLocalPlayerController();
	class AGsUIManager* GetUIManager();

	/** 개발모드인가. 진입 플로우가 달라짐 */
	UFUNCTION(BlueprintPure, Category = "Lobby")
	bool IsDevMode() const { return bIsDevMode; }

	/** 계정인증 완료 되었는가(DevMode일 경우 로그인 완료) */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	bool IsAccountLoginComplete() const { return bIsAccountLoginComplete; }

	/** 계정 인증 시도(하이브 인증 시도) */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void TryAccountLogin();

	/** 계정인증 완료여부 */
	UFUNCTION()
	void SetAccountLogin(bool InComplete);

	/** 서버 선택 시도 */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void TrySelectServer(int32 InServerID);

	/** 선택된 서버의 ID. GVS 정보로 최근 접속 서버로 세팅해둘 것 */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SetSelectedServer(int32 InServerID);

	/** 선택된 서버의 ID. */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	int32 GetSelectedServer() const { return SelectedServerID; }

	/** DevMode의 계정인증 시도 */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void TryDevAccountLogin(FString InName, FString InPassword);

	/** 게임입장 시도 */
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void TryGameLogin();

	// TEST Begin -------------------------------------------------------------------------
	/** 타이머를 돌려 DownloadRate를 증가 */
	void TestStartDownload();

	/** WidgetClassMap에 등록된 WidgetClass 가져오기 */
	TSubclassOf<class UGsUIWidgetBase> GetWidgetClass(EGS_LOBBY_WIDGET_Enum InType) const;

protected:
	/** 타이머 핸들러 */
	void OnRepeatTestTimer();
	// TEST End ---------------------------------------------------------------------------
	void OnLoginComplete(); // Hive 로그인 or Dev 모드 인증서버 로그인 완료
protected:
	/** 개발모드인가. 에셋 다운로드 과정 스킵 후 로그인으로 바로 넘어감 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lobby")
	bool bIsDevMode;

	/** 계정 인증 완료된 상태 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lobby")
	bool bIsAccountLoginComplete;

	/** 선택된 서버 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lobby")
	int32 SelectedServerID;

	// TEST Begin -------------------------------------------------------------------------
	/** 다운로드율 */
	float DownloadRate;

	/** UI 프로그레스바 진행 테스트용 타이머 */
	FTimerHandle TestTimerHandle;

	/** Enum으로 등록된 UI들 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lobby")
	TMap<EGS_LOBBY_WIDGET_Enum, TSubclassOf<class UGsUIWidgetBase>> WidgetClassMap;
	// TEST End ---------------------------------------------------------------------------
};
