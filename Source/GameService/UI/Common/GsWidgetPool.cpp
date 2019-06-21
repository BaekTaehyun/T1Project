// Fill out your copyright notice in the Description page of Project Settings.


#include "GsWidgetPool.h"
#include "Blueprint/UserWidget.h"


void UGsWidgetPool::Initialize(TSubclassOf<class UUserWidget> InWidgetClass, int32 InWarmUpCount)
{
	WidgetClass = InWidgetClass;

	WarmUp(InWarmUpCount);
}

UUserWidget* UGsWidgetPool::CreateOrGetInstance()
{
	UUserWidget* widgetInstance = nullptr;

	if (0 == InactiveArray.Num())
	{
		widgetInstance = CreateWidgetInternal();
	}
	else
	{
		widgetInstance = InactiveArray.Pop();
	}

	if (widgetInstance)
	{
		ActiveArray.Add(widgetInstance);
	}

	UObject* outer = widgetInstance->GetOuter();

	return widgetInstance;
}

void UGsWidgetPool::Release(UUserWidget* InWidget)
{
	int32 index = ActiveArray.Find(InWidget);
	if (INDEX_NONE != index)
	{
		InactiveArray.Push(InWidget);
		ActiveArray.RemoveAt(index);
	}
}

void UGsWidgetPool::ReleaseAll()
{
	for (auto& widget : ActiveArray)
	{
		widget->RemoveFromParent();
		InactiveArray.Add(widget);
	}

	ActiveArray.Empty();
}

void UGsWidgetPool::WarmUp(int32 InWarmupCount)
{
	for (int32 i = 0; i < InWarmupCount; ++i)
	{
		UUserWidget* widgetInstance = CreateWidgetInternal();
		InactiveArray.Add(widgetInstance);
	}
}

UUserWidget* UGsWidgetPool::CreateWidgetInternal()
{
	if (nullptr == WidgetClass)
	{
		return nullptr;
	}

	// 월드로 넣어도 내부적으로 Instance가 있으면 Outer로 설정하도록 되어있음
	return CreateWidget(GetWorld(), WidgetClass);
}

void UGsWidgetPool::Empty()
{
	InactiveArray.Empty();

	for (auto& widget : ActiveArray)
	{
		widget->RemoveFromParent();
	}

	ActiveArray.Empty();
}
