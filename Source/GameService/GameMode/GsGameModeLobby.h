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

	virtual void StartPlay() override;

	// ���� �÷��̾� ��Ʈ�ѷ� ������
	class APlayerController* GetLocalPlayerController();
	class AGsUIManager* GetUIManager();

protected:
	// �κ� ���� ���� ������Ʈ
	UPROPERTY(BlueprintReadWrite, Category = "Lobby")
	class UGsLobbyComponent* LobbyComponent;
};
