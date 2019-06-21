// Fill out your copyright notice in the Description page of Project Settings.


#include "GsWidgetPoolManager.h"
#include "UI/Common/GsWidgetPool.h"


void UGsWidgetPoolManager::Initialize()
{

}

void UGsWidgetPoolManager::BeginDestroy()
{
	RemoveAll();

	Super::BeginDestroy();
}

void UGsWidgetPoolManager::RemoveAll()
{
	for (auto& iter : PoolMap)
	{
		iter.Value->Empty();
	}

	PoolMap.Empty();
}

void UGsWidgetPoolManager::ReleaseAll()
{
	for (auto& iter : PoolMap)
	{
		iter.Value->ReleaseAll();
	}
}

UGsWidgetPool* UGsWidgetPoolManager::CreateOrGetPool(TSubclassOf<UUserWidget> InWidgetClass)
{
	UGsWidgetPool* widgetPool = FindPool(InWidgetClass);
	if (nullptr == widgetPool)
	{
		widgetPool = NewObject<UGsWidgetPool>(this);
		widgetPool->Initialize(InWidgetClass);

		FName key = FName(*InWidgetClass.Get()->GetPathName());
		PoolMap.Add(key, widgetPool);
	}
	else
	{
		widgetPool->Initialize(InWidgetClass);
	}

	return widgetPool;
}

UUserWidget* UGsWidgetPoolManager::CreateOrGetWidget(TSubclassOf<UUserWidget> InWidgetClass)
{
	UGsWidgetPool* widgetPool = CreateOrGetPool(InWidgetClass);

	return widgetPool->Clame();
}

void UGsWidgetPoolManager::ReleaseWidget(TSubclassOf<UUserWidget> InWidgetClass, UUserWidget* InWidget)
{
	UGsWidgetPool* widgetPool = FindPool(InWidgetClass);
	if (nullptr != widgetPool)
	{
		widgetPool->Release(InWidget);
	}
}

UGsWidgetPool* UGsWidgetPoolManager::FindPool(TSubclassOf<UUserWidget> InWidgetClass)
{
	FName key = FName(*InWidgetClass.Get()->GetPathName());
	UGsWidgetPool** result = PoolMap.Find(key);
	if (nullptr != result)
	{
		return *result;
	}

	return nullptr;
}

//UGsWidgetPoolManager* UGsWidgetPoolManager::GetManager(UWorld* InWorld)
//{
//	UGsUIMana
//}