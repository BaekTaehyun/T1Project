// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "T1Project.h"
#include "GameFramework/GameModeBase.h"
#include "ABGameMode.generated.h"

/**
 * 
 */
UCLASS()
class T1PROJECT_API AABGameMode : public AGameModeBase
{
	GENERATED_BODY()	

public:
	AABGameMode();

	virtual void PostInitializeComponents() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void AddScore(class AABPlayerController *ScoredPlayer);
	int32 GetScore() const;

private:
	class AABGameState* ABGameState;

	UPROPERTY()
	int32 ScoreToClear;
};
