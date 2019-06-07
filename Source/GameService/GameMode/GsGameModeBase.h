// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GsGameModeBase.generated.h"

class UGsSpawnComponent;
/**
 * 게임의 틀을 잡아주는 역활
   1. 플레이어 점검
   2. 입장한 플레이어가 조종할 컨트롤러 할당
 */
UCLASS()
class GAMESERVICE_API AGsGameModeBase : public AGameModeBase
{
	GENERATED_BODY()	
public:
	AGsGameModeBase();
	virtual ~AGsGameModeBase();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	// 플레이어가 게임에 입장하는 순간(로그인), Editor에서 Play
	virtual void PostLogin(APlayerController* newPlayer) override;

	virtual void TeleportPlayer(FString in_Tag, bool in_waitAllLoad = false) {}

	virtual void StartPlay() override;
	virtual void Tick(float deltaSeconds);

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, Meta = (AllowPrivateAccess = true))
	UGsSpawnComponent*	_SpawnComponent;
};
