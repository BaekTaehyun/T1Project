// Fill out your copyright notice in the Description page of Project Settings.


#include "GsDealScrollManager.h"
#include "UObject/ConstructorHelpers.h"
#include "UI/GsWidgetPoolManager.h"
#include "UI/Common/GsUIDealScrollWidget.h"
#include "UI/Table/GsDealScrollTable.h"
#include "UI/Common/GsWidgetPool.h"
#include "BPFunction/UI/GsBlueprintFunctionLibraryUI.h"


UGsDealScrollManager::UGsDealScrollManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/*
	static ConstructorHelpers::FObjectFinder<UDataTable> DEALSCROLL_TABLE(
		TEXT("/Game/UI/Test/DealScrollTest/DealScrollTable.DealScrollTable"));

	if (DEALSCROLL_TABLE.Succeeded())
	{
		DealScrollDataTable = DEALSCROLL_TABLE.Object;
	}

	static ConstructorHelpers::FClassFinder<UGsUIDealScrollWidget> DEALSCROLL_WIDGET(
		TEXT("/Game/UI/Test/DealScrollTest/WBP_DealScroll.WBP_DealScroll_C"));

	if (DEALSCROLL_WIDGET.Succeeded())
	{
		WidgetClass = DEALSCROLL_WIDGET.Class;
	}
	*/
}

const FGsTableDealScroll* UGsDealScrollManager::GetTableRow(const FName& InKey)
{
	return DealScrollDataTable->FindRow<FGsTableDealScroll>(InKey, TEXT(""));
}

UGsUIDealScrollWidget* UGsDealScrollManager::AddWidget(const FName& InKey)
{
	const FGsTableDealScroll* data = GetTableRow(InKey);
	if (nullptr == data)
	{
		return nullptr;
	}

	if (nullptr == WidgetPool.Get())
	{
		WidgetPool = GetWidgetPool();
		if (nullptr == WidgetPool.Get())
		{
			return nullptr;
		}
	}

	UGsUIDealScrollWidget* widget = WidgetPool.Get()->Clame<UGsUIDealScrollWidget>();
	if (nullptr != widget)
	{
		widget->AddToViewport();
		widget->SetData(*data);
	}

	return widget;
}

void UGsDealScrollManager::ReleaseWidget(UGsUIDealScrollWidget* InWidget)
{
	if (nullptr != InWidget)
	{
		if (nullptr != WidgetPool.Get())
		{
			InWidget->RemoveFromParent();
			WidgetPool->Release(InWidget);
		}
	}
}

UGsWidgetPool* UGsDealScrollManager::GetWidgetPool()
{
	if (nullptr == WidgetClass)
	{
		return nullptr;
	}

	UGsWidgetPoolManager* poolManager = UGsBlueprintFunctionLibraryUI::GetWidgetPoolManager(GetWorld());
	if (nullptr == poolManager)
	{
		return nullptr;
	}

	return poolManager->CreateOrGetPool(WidgetClass);
}