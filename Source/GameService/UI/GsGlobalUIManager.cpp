// Fill out your copyright notice in the Description page of Project Settings.


#include "GsGlobalUIManager.h"
#include "GameService.h"
#include "UObject/ConstructorHelpers.h"
#include "GsUIPathTable.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "GsUINonDestructiveWidget.h"
#include "UserWidget.h"
#include "Message/GsMessageManager.h"
#include "Message/GsMessageSystem.h"
#include "GSGameInstance.h"

UGsGlobalUIManager::UGsGlobalUIManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
//UGsGlobalUIManager::UGsGlobalUIManager()
{
	static ConstructorHelpers::FClassFinder<UGsUINonDestructiveWidget> LOADING_WIDGET(
		TEXT("/Game/UI/Global/WBP_Loading.WBP_Loading_C"));

	if (LOADING_WIDGET.Succeeded())
	{
		LoadingWidgetClass = LOADING_WIDGET.Class;
	}
}

void UGsGlobalUIManager::ShowLoading(bool InVisible)
{
	if (nullptr == LoadingWidget)
	{
		auto gameInstance = Cast<UGsGameInstance>(GetOuter());
		if (nullptr != gameInstance)
		{
			LoadingWidget = CreateWidget<UGsUINonDestructiveWidget>(gameInstance, LoadingWidgetClass);
		}
	}

	if (nullptr != LoadingWidget)
	{
		if (InVisible)
		{
			if (false == LoadingWidget->IsInViewport())
			{
				LoadingWidget->AddToViewport();
			}

			LoadingWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			LoadingWidget->OnMessage(FName(TEXT("FadeOut")));
			//LoadingWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UGsGlobalUIManager::OnLevelLoadStart()
{
	ShowLoading(true);
}

void UGsGlobalUIManager::OnLevelLoadComplete()
{
	ShowLoading(false);
}