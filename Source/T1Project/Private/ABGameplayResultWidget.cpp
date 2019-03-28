// Fill out your copyright notice in the Description page of Project Settings.

#include "ABGameplayResultWidget.h"
#include "Components/TextBlock.h"
#include "ABGameState.h"


void UABGameplayResultWidget::BindGameState(class AABGameState* GameState)
{
	T1CHECK(nullptr != GameState);
	CurrentGameState = GameState;
}

void UABGameplayResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	T1CHECK(CurrentGameState.IsValid());

	auto Result = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtResult")));
	T1CHECK(nullptr != Result);
	Result->SetText(FText::FromString(CurrentGameState->IsGameCleared() ? TEXT("Mission Complete") : TEXT("Mission Failed")));

	auto TotalScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtTotalScore")));
	T1CHECK(nullptr != TotalScore);
	TotalScore->SetText(FText::FromString(FString::FromInt(CurrentGameState->GetTotalGameScore())));
}