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

	virtual void StartPlay() override;

	// 로컬 플레이어 컨트롤러 얻어오기
	class APlayerController* GetLocalPlayerController();
	class AGsUIManager* GetUIManager();

protected:
	// 로비 관련 로직 컴포넌트
	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	class UGsLobbyComponent* LobbyComponent;
};
