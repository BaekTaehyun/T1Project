// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemIconSelector.h"

UUIIcon* UItemIconSelector::CreateItemIcon()
{
	UE_LOG(LogTemp, Log, TEXT("CreateItemIcon() Call  !!"));

	// ItemIcon Size 받아야할듯 - Small , Medium , Large , ExtarLarge

	UBlueprintGeneratedClass* _isLoaded = LoadObject<UBlueprintGeneratedClass>(nullptr, TEXT("/Game/UI/Item/ItemIconL.ItemIconL_C"));
	if (nullptr != GetWorld() && nullptr != _isLoaded)
	{
		UUserWidget* _createWidget = CreateWidget(GetWorld()->GetFirstPlayerController(), _isLoaded);
		//ItemImgRoot->AddChild(_createWidget);
		UUIIcon* _Icon = Cast<UUIIcon>(_createWidget);
		if (nullptr == _Icon)
		{
			UE_LOG(LogTemp, Log, TEXT("_Icon is nullptr !!"));
		}
		else
		{
			_Icon->SetItemImg();
		}

		if (nullptr == _createWidget)
		{
			UE_LOG(LogTemp, Log, TEXT("_createWidget is nullptr !!"));
		}
		else
		{
			_createWidget->AddToViewport();
		}
	}

	return nullptr;
}


UUIIcon* UItemIconSelector::SetItemIcon(const UCItem* In_CItem)
{

	return nullptr;
}