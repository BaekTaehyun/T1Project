// Fill out your copyright notice in the Description page of Project Settings.


#include "GsUIPopup.h"
#include "GsUIManager.h"


void UGsUIPopup::Close()
{
	AGsUIManager* UIManager = AGsUIManager::GetUIManager(GetOwningPlayer());
	if (nullptr != UIManager)
	{
		UIManager->Pop(this);
	}
}