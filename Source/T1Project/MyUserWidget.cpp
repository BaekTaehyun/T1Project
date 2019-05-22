// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"
//#include "ConstructorHelpers.h"

UMyUserWidget::UMyUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UMyUserWidget::TestUI()
{
	AddToViewport();
}

void UMyUserWidget::OnClickTest()
{
	UE_LOG(LogTemp , Log , TEXT("Success Log ~~~~~"));
}

void UMyUserWidget::OnClickCastFailed()
{
	UE_LOG(LogTemp, Log, TEXT("Cast Failed Log !!!!!!!!!!"));
}
