// Fill out your copyright notice in the Description page of Project Settings.


#include "GsUIWindow.h"
#include "GsUIManager.h"


void UGsUIWindow::Close()
{
	AGsUIManager* UIManager = AGsUIManager::GetUIManager(GetOwningPlayer());
	if (nullptr != UIManager)
	{
		UIManager->Pop(this);
	}
}