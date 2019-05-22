// Fill out your copyright notice in the Description page of Project Settings.


#include "GsUITray.h"
#include "GsUIManager.h"

void UGsUITray::Close()
{
	AGsUIManager* UIManager = AGsUIManager::GetUIManager(GetOwningPlayer());
	if (nullptr != UIManager)
	{
		UIManager->Pop(this);
	}
}