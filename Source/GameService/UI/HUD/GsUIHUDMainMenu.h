// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GsUIHUDMainMenu.generated.h"

/**
 * HUD ���� ���� �޴�
 */
UCLASS()
class GAMESERVICE_API UGsUIHUDMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	// FIX: �׽�Ʈ�� �ӽ� �Լ�
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void GotoLobby();
	
};
