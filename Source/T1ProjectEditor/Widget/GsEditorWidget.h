// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GsEditorWidget.generated.h"

class UTextBlock;
//----------------------------------------------------------------
// 에디터 모드일때 레벨 뷰포트에서 액터의 설명을 위해 사용한다. 
//----------------------------------------------------------------

UCLASS()
class T1PROJECTEDITOR_API UGsEditorWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, Category = "GsEditorWidget", meta = (BindWidget))
		UTextBlock* _DescriptionText;
};
