// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"
//#include "ConstructorHelpers.h"

UMyUserWidget::UMyUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}


void UMyUserWidget::SetIcon()
{

}


void UMyUserWidget::OnDisplay()
{
	AddToViewport();
	
}


void UMyUserWidget::OnClickTest()
{
	UE_LOG(LogTemp , Log , TEXT("Success Log ~~~~~"));
	SetIcon();
}

