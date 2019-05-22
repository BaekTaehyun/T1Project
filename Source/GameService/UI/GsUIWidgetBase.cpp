// Fill out your copyright notice in the Description page of Project Settings.


#include "GsUIWidgetBase.h"
#include "GsUIParameter.h"
#include "GameService.h"
#include "GsUIManager.h"


UGsUIWidgetBase::UGsUIWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),	bStackProcessed(false)
{

}

void UGsUIWidgetBase::OnPush_Implementation(UGsUIParameter* InParam)
{
	//GSLOG(Warning, TEXT("--------Push"));
}

void UGsUIWidgetBase::OnMessage_Implementation(FName InKey, UGsUIParameter* InParam)
{

}

void UGsUIWidgetBase::Close()
{
	AGsUIManager* UIManager = GetUIManager();
	if (nullptr != UIManager)
	{
		UIManager->Pop(this);
	}
}

UFUNCTION(BlueprintCallable, Category = "GsManaged")
class AGsUIManager* UGsUIWidgetBase::GetUIManager()
{
	return AGsUIManager::GetUIManager(GetOwningPlayer());
}

/*
void UGsUIWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	GSLOG(Warning, TEXT("--------NativeOnInitialized"));
}
void UGsUIWidgetBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	GSLOG(Warning, TEXT("--------NativePreConstruct"));
}
void UGsUIWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	GSLOG(Warning, TEXT("--------NativeConstruct"));
}
void UGsUIWidgetBase::NativeDestruct()
{
	Super::NativeDestruct();
	GSLOG(Warning, TEXT("--------NativeDestruct"));
}
void UGsUIWidgetBase::BeginDestroy()
{
	Super::BeginDestroy();
	GSLOG(Warning, TEXT("--------BeginDestroy"));
}
*/
