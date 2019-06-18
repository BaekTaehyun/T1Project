// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemIconSelector.h"
#include "Overlay.h"
#include "CanvasPanel.h"


//UBlueprintGeneratedClass* _isLoaded = LoadObject<UBlueprintGeneratedClass>(nullptr, TEXT("/Game/UI/Item/ItemIconL.ItemIconL_C"));
UGsUIIcon* UItemIconSelector::CreateItemIcon(eItemIconSize In_eIconSize, UPanelWidget* In_parentWidget, const UWorld* In_worldObj)
{
	UE_LOG(LogTemp, Log, TEXT("CreateItemIcon() Call  !!"));

	if (nullptr == In_parentWidget || nullptr == In_worldObj)
		return nullptr;

	if (nullptr != UIIcon)
	{
		return UIIcon;
	}

	// size 에 따른 블프 경로 table 로 빼야함.
	const FString path = GetPathbyIconSize(In_eIconSize);
	UBlueprintGeneratedClass* bpLoaded = LoadObject<UBlueprintGeneratedClass>(nullptr, *path);
	if (nullptr != In_worldObj && nullptr != bpLoaded)
	{
		UUserWidget* createWidget = CreateWidget(In_worldObj->GetFirstPlayerController(), bpLoaded);
		if (nullptr == createWidget)
		{
			UE_LOG(LogTemp, Log, TEXT("_createWidget is nullptr !!"));
			return nullptr;
		}

		UGsUIIcon* uiIcon = Cast<UGsUIIcon>(createWidget);
		if (nullptr == uiIcon)
		{
			UE_LOG(LogTemp, Log, TEXT("uiIcon is nullptr !!"));
			return nullptr;
		}

		// 현재는 이미지만.
		uiIcon->SetDefaultItemImg();
		createWidget->AddToViewport();

		In_parentWidget->AddChild(createWidget);
		UIIcon = uiIcon;
	}

	return UIIcon;
}

const FString UItemIconSelector::GetPathbyIconSize(eItemIconSize In_eSize)
{
	FString path = "";
	switch (In_eSize)
	{
	case UItemIconSelector::Small:
		path = TEXT("/Game/UI/Item/ItemIconL.ItemIconS_C");
		break;
	case UItemIconSelector::Medium:
		path = TEXT("/Game/UI/Item/ItemIconL.ItemIconM_C");
		break;
	case UItemIconSelector::Large:
		path = TEXT("/Game/UI/Item/ItemIconL.ItemIconL_C");
		break;
	case UItemIconSelector::ExtraLarge:
		path = TEXT("/Game/UI/Item/ItemIconL.ItemIconL_C");
		break;
	default:
		path = TEXT("/Game/UI/Item/ItemIconL.ItemIconL_C");
		break;
	}

	return path;
}

UGsUIIcon* UItemIconSelector::SetItemIcon(UCItem* In_CItem)
{
	/*
	if (nullptr == In_CItem || nullptr == UIIcon)
		return nullptr;
		*/
	if (nullptr == UIIcon)
	{
		UE_LOG(LogTemp, Log, TEXT("UIIcon is null  !!"));
		return nullptr;
	}

	UIIcon->SetItem(In_CItem);
	return UIIcon;
}