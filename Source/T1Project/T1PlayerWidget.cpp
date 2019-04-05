// Fill out your copyright notice in the Description page of Project Settings.

#include "T1PlayerWidget.h"
#include "T1PlayerStatComponent.h"
#include "Components/ProgressBar.h"


void UT1PlayerWidget::BindPlayerStat(UT1PlayerStatComponent* newPlayerStat)
{
	T1CHECK(nullptr != newPlayerStat);

	CurrentPlayerStat = newPlayerStat;
	if (CurrentPlayerStat.IsValid())
	{
		newPlayerStat->OnHPIsChanged.AddUObject(this, &UT1PlayerWidget::UPdateHPWidget);
	}
}

void UT1PlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
	T1CHECK(nullptr != HPProgressBar);
	UPdateHPWidget();
}

void UT1PlayerWidget::UPdateHPWidget()
{
	if (CurrentPlayerStat.IsValid())
	{
		if (nullptr != HPProgressBar)
		{
			HPProgressBar->SetPercent(CurrentPlayerStat->GetHpRatio());
		}
	}
}


