// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"
#include "TextBlock.h"
//#include "ConstructorHelpers.h"

UMyUserWidget::UMyUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	m_NumTest = 0;
}


void UMyUserWidget::SetIcon()
{

}


void UMyUserWidget::OnDisplay()
{
	AddToViewport();
	
}

void UMyUserWidget::BeginDestroy()
{
	Super::BeginDestroy();

	RemoveFromViewport();
	UE_LOG(LogTemp, Log, TEXT("Call RemoveFromViewport() !!! "));
}


void UMyUserWidget::OnClickTest()
{
	UE_LOG(LogTemp , Log , TEXT("Success Log ~~~~~"));
	//SetIcon();
	OnClickText();
}

void UMyUserWidget::OnClickText()
{
	UE_LOG(LogTemp, Log, TEXT("OnClickText !!!"));

	UTextBlock* _textblock = Cast<UTextBlock>(this->GetWidgetFromName("TextBox"));
	if (NULL != _textblock)
	{
		FTextBuilder _builder;
		FString _text;
		_text.AppendInt(m_NumTest);
		_builder.AppendLine(_text);
		_textblock->SetText(_builder.ToText());
		++m_NumTest;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("OnClickText --- _textblock is Null !!!!!"));
	}

	/*
	TArray<UObject*> objArray;
	GetDefaultSubobjects(objArray);
	
	UE_LOG(LogTemp, Log, TEXT("%d : "), objArray.Num());
	for (UObject* Obj : objArray)
	{
		UE_LOG(LogTemp, Log, TEXT("%s : "), *Obj->GetFullName());
	}
	*/

}