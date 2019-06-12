// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GsUIHUD.h"
#include "GsUIHUDMain.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API UGsUIHUDMain : public UGsUIHUD
{
	GENERATED_BODY()
	
public:
	virtual void BeginDestroy() override;
	virtual void NativeConstruct() override;

	void SetPlayerName(const FString& InPlayerName);

private:

	// ���ÿ�
	class UTextBlock* LabelPlayerName;
	class UGsUIHUDMainMenu* MainMenu;

	// UserWidget ��ӹ��� �������� ���� �����ؼ� ���⿡ �����Ͽ� �������.
	// MainMenuUI
	// SkillUI
	// PlayerInfoUI
	// MiniMapUI
	// ChattingUI
	// QuestUI
};
