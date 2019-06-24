// Fill out your copyright notice in the Description page of Project Settings.


#include "GsCreatureWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void UGsCreatureWidget::NativeConstruct()
{
	Super::NativeConstruct();

	NameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Name")));
	HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
}

void UGsCreatureWidget::SetNameText(const FText& InName)
{
	if (nullptr != NameText)
	{
		NameText->Text = InName;
	}
}

void UGsCreatureWidget::SetHpPercent(float InPercent)
{
	if (nullptr != HPBar)
	{
		HPBar->SetPercent(InPercent);
	}
}