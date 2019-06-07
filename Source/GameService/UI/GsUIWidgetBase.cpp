// Fill out your copyright notice in the Description page of Project Settings.


#include "GsUIWidgetBase.h"
#include "GsUIParameter.h"
#include "GameService.h"
#include "GsUIManager.h"
#include "GSGameInstance.h"


UGsUIWidgetBase::UGsUIWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bStackProcessed = false;
	bNotUseManagedZOrder = false;
	AddZOrder = 0;
	BackupVisibility = Visibility;
	bNotDestroy = false;
	bEnableAutoDestroy = false;
}

void UGsUIWidgetBase::NativeOnInitialized()
{
	// 블루프린트에서 입력된 값으로 가져온다
	SetEnableAutoDestroy(!bNotDestroy);

	Super::NativeOnInitialized();
}

void UGsUIWidgetBase::BeginDestroy()
{
	Super::BeginDestroy();
	GSLOG(Warning, TEXT("--------BeginDestroy"));
}

void UGsUIWidgetBase::RemoveFromParent()
{
	if (bEnableAutoDestroy)
	{
		Super::RemoveFromParent();
	}
}

void UGsUIWidgetBase::SetEnableAutoDestroy(bool bInEnableAutoDestroy)
{
	bEnableAutoDestroy = bInEnableAutoDestroy;

	// GC방지
	// RF_MarkAsRootSet : Object will be marked as root set on construction and not be garbage collected, 
	// even if unreferenced (DO NOT USE THIS FLAG in HasAnyFlags() etc)
	int32 CurrentFlags = (int32)(GetFlags());
	if (bEnableAutoDestroy)
	{
		SetFlags((EObjectFlags)(CurrentFlags | RF_MarkAsRootSet));
	}
	else
	{
		SetFlags((EObjectFlags)(CurrentFlags ^ RF_MarkAsRootSet));
	}
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
	UGsUIManager* UIManager = GetUIManager();
	if (nullptr != UIManager)
	{
		UIManager->Pop(this);
	}
}

UFUNCTION(BlueprintCallable, Category = "GsManaged")
UGsUIManager* UGsUIWidgetBase::GetUIManager()
{
	UGsGameInstance* gameInstance = GetGameInstance<UGsGameInstance>();
	if (nullptr != gameInstance)
	{
		return gameInstance->GetUIManager();
	}

	return nullptr;
}

int32 UGsUIWidgetBase::GetManagedZOrder() const
{
	return (bNotUseManagedZOrder) ? AddZOrder : GetManagedDefaultZOrder() + AddZOrder;
}

void UGsUIWidgetBase::Hide()
{
	BackupVisibility = Visibility;
	SetVisibility(ESlateVisibility::Hidden);
}

void UGsUIWidgetBase::Unhide()
{
	SetVisibility(BackupVisibility);
}

/*
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
