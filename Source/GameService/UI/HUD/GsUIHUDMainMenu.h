// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GsUIHUDMainMenu.generated.h"

/**
 * HUD 우상단 메인 메뉴
 */
UCLASS()
class GAMESERVICE_API UGsUIHUDMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	// FIX: 테스트용 임시 함수
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void GotoLobby();
	
};
