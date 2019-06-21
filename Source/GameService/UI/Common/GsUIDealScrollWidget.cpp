// Fill out your copyright notice in the Description page of Project Settings.


#include "GsUIDealScrollWidget.h"
#include "Components/TextBlock.h"
#include "UI/Table/GsDealScrollTable.h"
#include "BPFunction/UI/GsBlueprintFunctionLibraryUI.h"
#include "UI/GsUIManager.h"
#include "UI/GsDealScrollManager.h"


void UGsUIDealScrollWidget::NativeConstruct()
{
	InitAnaimationMap();

	TextBlockMain = Cast<UTextBlock>(GetWidgetFromName(TEXT("TEXT_Main")));
	TextBlockSub = Cast<UTextBlock>(GetWidgetFromName(TEXT("TEXT_Sub")));

	Super::NativeConstruct();
}

void UGsUIDealScrollWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	// FIX: 개발 미완. 타겟 위치이동에 따른 처리 추가

	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UGsUIDealScrollWidget::BeginDestroy()
{
	AnimationMap.Empty();
	TextBlockMain = nullptr;
	TextBlockSub = nullptr;
	 
	Super::BeginDestroy();
}

void UGsUIDealScrollWidget::InitAnaimationMap()
{
	AnimationMap.Empty();

	UWidgetBlueprintGeneratedClass* genClass = GetWidgetTreeOwningClass();

	if (nullptr != genClass)
	{
		for (UWidgetAnimation* anim : genClass->Animations)
		{
			AnimationMap.Add(anim->GetFName(), anim);
		}
	}
}

void UGsUIDealScrollWidget::SetData(const struct FGsTableDealScroll& InData)
{
	SetTransform(InData);

	// 폰트세팅
	SetTextBoxSetting(TextBlockMain, InData.MainTextSetting);
	SetTextBoxSetting(TextBlockSub, InData.SubTextSetting);

	// 애니메이션 재생
	// 블루프린트에서 보이는 이름에 _INST 가 붙는 것에 유의
	FString animName = InData.AnimationName.ToString();
	animName.Append(TEXT("_INST"));

	UWidgetAnimation** anim = AnimationMap.Find(FName(*animName));
	if (nullptr != anim)
	{
		PlayAnimation(*anim);
	}
}

void UGsUIDealScrollWidget::SetTransform(const struct FGsTableDealScroll& InData)
{
	// FIX: 개발 미완. 위치계산 로직 추가
	FVector worldLocation = FVector::ZeroVector;
	FVector2D screenLocation = FVector2D::ZeroVector;

	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	if (nullptr != playerController)
	{
		playerController->ProjectWorldLocationToScreen(worldLocation, screenLocation);
	}

	SetPositionInViewport(screenLocation);
}

void UGsUIDealScrollWidget::SetTextBoxSetting(UTextBlock* InTextBlock, const FGsTextBoxSetting& InData)
{
	if (nullptr == InTextBlock)
	{
		return;
	}

	FSlateColor color = FSlateColor(InData.Color);
	InTextBlock->SetColorAndOpacity(color);

	// 개선: 폰트를 가져와서 SetFont 해야하는가, 바로 바꿔도 되는가?
	FSlateFontInfo fontInfo = InTextBlock->Font;
	fontInfo.Size = InData.FontSize;
	fontInfo.TypefaceFontName = InData.TypefaceFontName;
	fontInfo.OutlineSettings.OutlineSize = InData.OutlineSize;
	fontInfo.OutlineSettings.OutlineColor = InData.OutlineColor;
	InTextBlock->SetFont(fontInfo);

	InTextBlock->SetShadowOffset(InData.ShadowOffset);
	InTextBlock->SetShadowColorAndOpacity(InData.ShadowColorAndOpacity);
}

void UGsUIDealScrollWidget::Close()
{
	if (nullptr != GetUIManager())
	{
		UGsDealScrollManager* dealScrollManager = GetUIManager()->GetDealScrollManager();
		if (nullptr != dealScrollManager)
		{
			dealScrollManager->ReleaseWidget(this);
		}
	}
}
