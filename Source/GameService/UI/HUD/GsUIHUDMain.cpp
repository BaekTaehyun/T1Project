// Fill out your copyright notice in the Description page of Project Settings.


#include "GsUIHUDMain.h"
#include "Components/TextBlock.h"
#include "GsUIHUDMainMenu.h"

void UGsUIHUDMain::BeginDestroy()
{
	Super::BeginDestroy();
}

void UGsUIHUDMain::NativeConstruct()
{
	Super::NativeConstruct();

	LabelPlayerName = Cast<UTextBlock>(GetWidgetFromName(TEXT("TEXT_PlayerName")));
	MainMenu = Cast<UGsUIHUDMainMenu>(GetWidgetFromName(TEXT("HUDMainMenu")));
	ConsumableMenu = Cast<UGsUIHUDConsumable>(GetWidgetFromName(TEXT("HUDConsumable")));
	if (nullptr != ConsumableMenu)
	{
		ConsumableMenu->CreateConsumeItem();
	}

	//SetPlayerName(TEXT("ASDF"));
}

void UGsUIHUDMain::SetPlayerName(const FString& InPlayerName)
{
	if (nullptr != LabelPlayerName)
	{
		LabelPlayerName->SetText(FText::FromString(InPlayerName));
	}
}