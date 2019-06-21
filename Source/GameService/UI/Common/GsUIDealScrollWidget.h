// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GsUIWidgetBase.h"
#include "GsUIDealScrollWidget.generated.h"

/**
 * µô½ºÅ©·Ñ À§Á¬(¹Ì¿Ï)
 */
UCLASS()
class GAMESERVICE_API UGsUIDealScrollWidget : public UGsUIWidgetBase
{
	GENERATED_BODY()

protected:
	TMap<FName, class UWidgetAnimation*> AnimationMap; // key: FName + _INST
	class UTextBlock* TextBlockMain;
	class UTextBlock* TextBlockSub;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void BeginDestroy() override;

public:
	virtual EGsUIHideFlags GetHideFlagType() const override { return EGsUIHideFlags::UI_HIDE_BILLBOARD; }
	virtual void Close() override;

public:
	void SetData(const struct FGsTableDealScroll& InData);

protected:
	// Fill the AnimationMap
	void InitAnaimationMap();

	void SetTextBoxSetting(class UTextBlock* InTextBlock, const struct FGsTextBoxSetting& InData);
	void SetTransform(const struct FGsTableDealScroll& InData);
};
