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
#include "HUD/GsUIHUDMain.h"
#include "UI/GsWidgetPoolManager.h"
#include "UI/GsDealScrollManager.h"


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
	WidgetPoolManager = nullptr;
	DealScrollManager = nullptr;
	
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

	if (nullptr == WidgetPoolManager)
	{
		WidgetPoolManager = NewObject<UGsWidgetPoolManager>(this);
		WidgetPoolManager->Initialize();
	}

	if (nullptr == DealScrollManager)
	{
		DealScrollManager = NewObject<UGsDealScrollManager>(this);
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

void UGsUIManager::ClearStack()
{
	if (nullptr != UIControllerNormal)
	{
		UIControllerNormal->ClearStack();
	}

	if (nullptr != UIControllerNotDestroy)
	{
		UIControllerNotDestroy->ClearStack();
	}
}

TWeakObjectPtr<UGsUIWidgetBase> UGsUIManager::PushAndGetWidget(const FName& InKey, UGsUIParameter* InParam)
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

void UGsUIManager::PopByKeyName(const FName& InKey)
{
	FGsTableUIPath* tableRow = GetTableRow(InKey);
	if (nullptr == tableRow)
	{
		return;
	}

	if (tableRow->bNotDestroy)
	{
		UIControllerNotDestroy->RemoveWidget(InKey);
	}

	UIControllerNormal->RemoveWidget(InKey);
}

UGsUIWidgetBase* UGsUIManager::PushInter(const FName& InKey, UGsUIParameter* InParam)
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

	UGsUIWidgetBase* widget = nullptr;
	UGsGameInstance* gameInstance = Cast<UGsGameInstance>(GetOuter());
	if (nullptr != gameInstance)
	{
		if (tableRow->bNotDestroy)
		{
			widget = UIControllerNotDestroy->CreateOrFind(gameInstance, tableRow->WidgetClass.Get(), InKey);
			UIControllerNotDestroy->AddWidget(widget, InParam);
		}
		else
		{
			widget = UIControllerNormal->CreateOrFind(gameInstance, tableRow->WidgetClass.Get(), InKey);
			UIControllerNormal->AddWidget(widget, InParam);
		}
	}

	return widget;
}

FGsTableUIPath* UGsUIManager::GetTableRow(const FName& InKey)
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
	WidgetPoolManager->ReleaseAll();
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

void UGsUIManager::ChangeHUD(const FName& InKey)
{
	// 이전항목 삭제
	if (CurrentHUDWidget.IsValid())
	{
		TWeakObjectPtr<UGsUIWidgetBase> widget = GetCachedWidget(InKey, false);
		if (CurrentHUDWidget.Get() == widget.Get())
		{
			return;
		}
		else
		{
			// 다르면 이전 것을 제거
			Pop(CurrentHUDWidget.Get());
			CurrentHUDWidget = nullptr;
		}
	}

	CurrentHUDWidget = PushAndGetWidget(InKey);
}

TWeakObjectPtr<UGsUIWidgetBase> UGsUIManager::GetCachedWidget(const FName& InKey, bool InActiveCheck)
{
	FGsTableUIPath* tableRow = GetTableRow(InKey);
	if (nullptr == tableRow)
	{
		return nullptr;
	}

	if (tableRow->bNotDestroy)
	{
		return UIControllerNotDestroy->GetCachedWidgetByName(InKey, InActiveCheck);
	}

	return UIControllerNormal->GetCachedWidgetByName(InKey, InActiveCheck);
}

void UGsUIManager::HideAll()
{
	//SetHideFlags(EGsUIHideFlags::UI_HIDE_WINDOW | EGsUIHideFlags::UI_HIDE_POPUP);
	SetHideFlags(EGsUIHideFlags::UI_HIDE_ALL);
}

void UGsUIManager::UnhideAll()
{
	//ClearHideFlags(EGsUIHideFlags::UI_HIDE_WINDOW | EGsUIHideFlags::UI_HIDE_POPUP);
	ClearHideFlags(EGsUIHideFlags::UI_HIDE_ALL);
}

void UGsUIManager::SetHideFlags(EGsUIHideFlags InFlag)
{
	// 중첩일 하나라도 없으면 Hide 수행
	if (false == EnumHasAllFlags(HideFlags, InFlag))
	{
		HideFlags = HideFlags | InFlag;
		OnUIHide.Broadcast(static_cast<int32>(HideFlags));
	}
}

void UGsUIManager::ClearHideFlags(EGsUIHideFlags InFlag)
{
	// 해당 플래그가 있으면 수행
	if (EnumHasAnyFlags(HideFlags, InFlag))
	{
		if (EGsUIHideFlags::UI_HIDE_ALL == InFlag)
		{
			HideFlags = EGsUIHideFlags::UI_HIDE_NONE;
		}
		else
		{
			HideFlags = HideFlags ^ InFlag;
		}

		OnUIHide.Broadcast(static_cast<int32>(HideFlags));
	}
}

/*
void UGsUIManager::TestGC()
{
	GEngine->ForceGarbageCollection();
}

void UGsUIManager::RemoveNormal()
{
	UIControllerNormal->RemoveAll();
}

void UGsUIManager::Back()
{
	bool isSuccess = UIControllerNotDestroy->Back();

	if (false == isSuccess)
	{
		UIControllerNormal->Back();
	}

	// 스텍 테스트
}
*/