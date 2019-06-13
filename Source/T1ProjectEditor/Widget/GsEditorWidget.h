// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GsEditorWidget.generated.h"

class UTextBlock;
//----------------------------------------------------------------
// ������ ����϶� ���� ����Ʈ���� ������ ������ ���� ����Ѵ�. 
//----------------------------------------------------------------

UCLASS()
class T1PROJECTEDITOR_API UGsEditorWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, Category = "GsEditorWidget", meta = (BindWidget))
		UTextBlock* _DescriptionText;
};
