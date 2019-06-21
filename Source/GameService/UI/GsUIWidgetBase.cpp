// Fill out your copyright notice in the Description page of Project Settings.


#include "GsUIWidgetBase.h"
#include "GsUIParameter.h"
#include "GameService.h"
#include "GsUIManager.h"
#include "GSGameInstance.h"


UGsUIWidgetBase::UGsUIWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, BackupVisibility(Visibility)
{

}

void UGsUIWidgetBase::NativeOnInitialized()
{
	// 블루프린트에서 입력된 값으로 가져온다
	SetEnableAutoDestroy(!bNotDestroy);

	Super::NativeOnInitialized();
}

void UGsUIWidgetBase::NativeConstruct()
{
	BackupVisibility = Visibility;
	UGsUIManager* uiManager = GetUIManager();
	if (nullptr != uiManager)
	{
		uiManager->OnUIHide.AddDynamic(this, &UGsUIWidgetBase::OnHide);
	}

	Super::NativeConstruct();
	//GSLOG(Warning, TEXT("--------NativeConstruct"));
}

void UGsUIWidgetBase::NativeDestruct()
{
	UGsUIManager* uiManager = GetUIManager();
	if (nullptr != uiManager)
	{
		uiManager->OnUIHide.RemoveDynamic(this, &UGsUIWidgetBase::OnHide);
	}

	Super::NativeDestruct();
	//GSLOG(Warning, TEXT("--------NativeDestruct"));
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

void UGsUIWidgetBase::OnHide(int32 InHideFlags)
{
	EGsUIHideFlags hideFlag = static_cast<EGsUIHideFlags>(InHideFlags);

	if (EnumHasAnyFlags(hideFlag, GetHideFlagType()))
	{
		// 중복 호출 상황 방지
		if (ESlateVisibility::Hidden != Visibility)
		{
			BackupVisibility = Visibility;
			SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		SetVisibility(BackupVisibility);
	}
}
