// Fill out your copyright notice in the Description page of Project Settings.

#include "GsUIManager.h"
#include "GameService.h"
#include "GsUIWidgetBase.h"
#include "GsUIParameter.h"
#include "GsUIController.h"
#include "GsUIControllerNotDestroy.h"
#include "GsUIEventInterface.h"
#include "UObject/ConstructorHelpers.h"
#include "GsUIPathTable.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "GSGameInstance.h"


UGsUIManager::UGsUIManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UDataTable> UIPathTable(TEXT("/Game/UI/UIPathTable.UIPathTable"));
	WidgetClassTable = UIPathTable.Object;
}

void UGsUIManager::BeginDestroy()
{
	RemoveAll();

	UIControllerNormal = nullptr;
	UIControllerNotDestroy = nullptr;
	WidgetClassTable = nullptr;
	
	Super::BeginDestroy();
}

void UGsUIManager::Initialize()
{
	if (nullptr == UIControllerNormal)
	{
		UIControllerNormal = NewObject<UGsUIController>(this);
	}

	if (nullptr == UIControllerNotDestroy)
	{
		UIControllerNotDestroy = NewObject<UGsUIControllerNotDestroy>(this);
	}
}

void UGsUIManager::RemoveAll()
{
	if (nullptr != UIControllerNormal)
	{
		UIControllerNormal->RemoveAll();
	}

	if (nullptr != UIControllerNotDestroy)
	{
		UIControllerNotDestroy->RemoveAll();
	}
}

TWeakObjectPtr<UGsUIWidgetBase> UGsUIManager::PushAndGetWidget(FName InKey, UGsUIParameter* InParam)
{
	return PushInter(InKey, InParam);
}

void UGsUIManager::Push(FName InKey, class UGsUIParameter* InParam)
{
	PushInter(InKey, InParam);
}

void UGsUIManager::Pop(UGsUIWidgetBase* InWidget)
{
	if (InWidget->IsNondestructiveWidget())
	{
		UIControllerNotDestroy->RemoveWidget(InWidget);
	}
	else
	{
		UIControllerNormal->RemoveWidget(InWidget);
	}
}

void UGsUIManager::PopByKeyName(FName InKey)
{
	FGsTableUIPath* tableRow = GetTableRow(InKey);
	if (nullptr == tableRow)
	{
		return;
	}

	FName keyName = FName(*tableRow->WidgetClass.Get()->GetName());
	if (tableRow->bNotDestroy)
	{
		UIControllerNotDestroy->RemoveWidget(keyName);
	}

	UIControllerNormal->RemoveWidget(keyName);
}

UGsUIWidgetBase* UGsUIManager::PushInter(FName InKey, UGsUIParameter* InParam)
{
	FGsTableUIPath* tableRow = GetTableRow(InKey);
	if (nullptr == tableRow)
	{
		return nullptr;
	}

	if (nullptr == tableRow->WidgetClass.Get())
	{
		GSLOG(Error, TEXT("UIWidget class is null"));
		return nullptr;
	}

	return CreateOrFind(tableRow->WidgetClass.Get(), tableRow->bNotDestroy, InParam);
}

UGsUIWidgetBase* UGsUIManager::CreateOrFind(TSubclassOf<UGsUIWidgetBase> InClass, bool bNotDestroy, UGsUIParameter* InParam)
{
	UGsUIWidgetBase* widget = nullptr;

	if (bNotDestroy)
	{
		UGsGameInstance* gameInstance = Cast<UGsGameInstance>(GetOuter());

		widget = UIControllerNotDestroy->CreateOrFind(gameInstance, InClass);
		UIControllerNotDestroy->AddWidget(widget, InParam);
	}
	else
	{
		UWorld* world = GetWorld(); // 이게 오너여

		widget = UIControllerNormal->CreateOrFind(world, InClass);
		UIControllerNormal->AddWidget(widget, InParam);
	}

	return widget;
}

FGsTableUIPath* UGsUIManager::GetTableRow(FName InKey)
{
	if (nullptr != WidgetClassTable)
	{
		FGsTableUIPath* tableRow = WidgetClassTable->FindRow<FGsTableUIPath>(InKey, TEXT(""));
		if (nullptr != tableRow)
		{
			if (tableRow->WidgetClass.IsNull())
			{
				GSLOG(Error, TEXT("WidgetClass is null. Key: %s"), *InKey.ToString());
				return nullptr;
			}

			if (tableRow->WidgetClass.IsPending())
			{
				FStreamableManager& assetMgr = UAssetManager::GetStreamableManager();
				tableRow->WidgetClass = assetMgr.LoadSynchronous(tableRow->WidgetClass, true);
			}

			return tableRow;
		}
	}

	GSLOG(Error, TEXT("Fail to find row in UIPathTable. Key: %s"), *InKey.ToString());
	
	return nullptr;
}

void UGsUIManager::OnChangeLevel()
{
	ShowLoading();
	
	UIControllerNormal->RemoveAll();
}

void UGsUIManager::ShowLoading()
{
	if (LoadingWidget.IsValid())
	{
		UIControllerNotDestroy->AddWidget(LoadingWidget.Get());
	}
	else
	{
		LoadingWidget = PushAndGetWidget(FName(TEXT("TrayLoading")));
	}
}

void UGsUIManager::HideLoading()
{
	if (LoadingWidget.IsValid())
	{
		LoadingWidget.Get()->OnMessage(FName(TEXT("FadeOut")));
	}
}
/*
void UGsUIManager::TestGC()
{
	GEngine->ForceGarbageCollection();
}
*/