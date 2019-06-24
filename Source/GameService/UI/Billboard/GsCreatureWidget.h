// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GsUIWidgetBase.h"
#include "GsCreatureWidget.generated.h"

/**
 * 크리쳐 기본 빌보드. 위젯 컴포넌트에서 사용되는 객체.
 */
UCLASS()
class GAMESERVICE_API UGsCreatureWidget : public UGsUIWidgetBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	class UTextBlock* NameText;

	UPROPERTY()
	class UProgressBar* HPBar;

protected:
	virtual void NativeConstruct() override;
	virtual EGsUIHideFlags GetHideFlagType() const override { return EGsUIHideFlags::UI_HIDE_BILLBOARD; }

public:
	void SetNameText(const FText& InName);
	void SetHpPercent(float InPercent); // 0 ~ 1 사이의 값
};
