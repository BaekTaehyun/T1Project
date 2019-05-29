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
	// 계정인증 완료 되었는가(DevMode일 경우 로그인 완료)
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	bool IsAccountLoginComplete() const { return bIsAccountLoginComplete; }

	// 계정 인증 시도(하이브 인증 시도)
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void TryAccountLogin();

	// DevMode의 계정인증 시도
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void TryDevAccountLogin(FString InName, FString InPassword);

	// 계정인증 완료여부
	UFUNCTION()
	void SetAccountLogin(bool InComplete);

	// 서버 선택 시도
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void TrySelectServer(int32 InServerID);

	// 선택된 서버의 ID. GVS 정보로 최근 접속 서버로 세팅해둘 것
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SetSelectedServer(int32 InServerID);

	// 선택된 서버의 ID
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	int32 GetSelectedServer() const { return SelectedServerID; }

	// 게임입장 시도
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void TryGameLogin();
	
protected:
	// 메세지 핸들러
	void OnEnterAssetDownloadStage();
	void OnEnterServerSelectStage();
	void OnEnterCharacterSelectStage();
	void OnLoginComplete(); // Hive 로그인 or Dev 모드 인증서버 로그인 완료
	void OnLoadGameScene();

	class AGsGameModeLobby* GetGameModeLobby();
	class AGsUIManager* GetUIManager();

	// TEST Begin -------------------------------------------------------------------------
	// 타이머를 돌려 DownloadRate를 증가
	void TestStartDownload();

	// 타이머 핸들러
	void OnRepeatTestTimer(); 

	// 다운로드율
	float DownloadRate;

	// UI 프로그레스바 진행 테스트용 타이머
	FTimerHandle TestTimerHandle;
	// TEST End ---------------------------------------------------------------------------

protected:
	// 계정 인증이 완료된 상태
	bool bIsAccountLoginComplete;

	// 선택된 서버
	int32 SelectedServerID;
};
