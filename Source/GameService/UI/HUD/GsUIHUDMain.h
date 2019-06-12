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

	// 예시용
	class UTextBlock* LabelPlayerName;
	class UGsUIHUDMainMenu* MainMenu;

	// UserWidget 상속받은 위젯들을 각각 구현해서 여기에 연결하여 사용하자.
	// MainMenuUI
	// SkillUI
	// PlayerInfoUI
	// MiniMapUI
	// ChattingUI
	// QuestUI
};
