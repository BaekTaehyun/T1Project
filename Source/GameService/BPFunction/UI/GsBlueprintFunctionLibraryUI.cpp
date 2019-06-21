// Fill out your copyright notice in the Description page of Project Settings.


#include "GsBlueprintFunctionLibraryUI.h"
#include "GameService.h"
#include "GSGameInstance.h"
#include "UI/GsUIManager.h"
#include "UI/GsWidgetPoolManager.h"
#include "UI/GsDealScrollManager.h"
#include "UI/Common/GsUIDealScrollWidget.h"


UGsUIManager* UGsBlueprintFunctionLibraryUI::GetUIManager(UWorld* InWorld)
{
	UGsGameInstance* gameInstance = InWorld->GetGameInstance<UGsGameInstance>();
	if (nullptr != gameInstance)
	{
		return gameInstance->GetUIManager();
	}

	return nullptr;
}

UGsWidgetPoolManager* UGsBlueprintFunctionLibraryUI::GetWidgetPoolManager(UWorld* InWorld)
{
	UGsUIManager* uiManager = GetUIManager(InWorld);
	if (nullptr != uiManager)
	{
		return uiManager->GetWidgetPoolManager();
	}

	return nullptr;
}

UGsUIDealScrollWidget* UGsBlueprintFunctionLibraryUI::AddDealScroll(UWorld* InWorld, FName InKey)
{
	UGsUIManager* uiManager = GetUIManager(InWorld);
	if (nullptr == uiManager)
	{
		return nullptr;
	}

	UGsDealScrollManager* dealScrollManager = uiManager->GetDealScrollManager();
	if (nullptr != dealScrollManager)
	{
		return dealScrollManager->AddWidget(InKey);
	}

	return nullptr;
}

//void UGsBlueprintFunctionLibraryUI::ReleaseDealScroll(UWorld* InWorld, UGsUIDealScrollWidget* InWidget)
//{
//	UGsUIManager* uiManager = GetUIManager(InWorld);
//	if (nullptr == uiManager)
//	{
//		return;
//	}
//
//	UGsDealScrollManager* dealScrollManager = uiManager->GetDealScrollManager();
//	if (nullptr != dealScrollManager)
//	{
//		return dealScrollManager->ReleaseWidget(InWidget);
//	}
//}