// Fill out your copyright notice in the Description page of Project Settings.


#include "UIIcon.h"


UUIIcon::UUIIcon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UUIIcon::ConstructEventCall()
{

}

void UUIIcon::OnClickItemIconEvent()
{
	UE_LOG(LogTemp, Log, TEXT("OnClickItemIconEvent Call !!!"));
}

void UUIIcon::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (nullptr != Item_CountText)
	{
		FText testFText = FText::FromString("Change Value !!!");
		Item_CountText->SetText(testFText);
	}

}

void UUIIcon::SetItemImg()
{
	UE_LOG(LogTemp, Log, TEXT("Call SetItemImg !!"));

	UTexture2D* _tex = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/Texture/icon_main_home_Sprite.icon_main_home_Sprite"));
	if (nullptr != _tex)
	{
		ItemImg->SetBrushFromTexture(_tex);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("_tex is null !!!!!"));
	}

}