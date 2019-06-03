// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GsUIWidgetBase.h"
#include "GsUINonDestructiveWidget.generated.h"

/**
 * 레벨이 바뀌어도 지우지 않을 위젯객체
 * 참고링크: https://irem.tistory.com/27
 */
UCLASS()
class GAMESERVICE_API UGsUINonDestructiveWidget : public UGsUIWidgetBase
{
	GENERATED_BODY()

public:
	UGsUINonDestructiveWidget(const FObjectInitializer& ObjectInitializer);

	virtual void RemoveFromParent() override;

	// 레벨 로드 시 파괴되지 않는 UI인가
	virtual bool IsNondestructiveWidget() const override { return true; }

private:
	// 주의: 생성자 안에서만 사용하는 함수입니다.
	void SetEnableAutoDestroy(bool bInEnableAutoDestroy);

private:
	bool bEnableAutoDestroy;
};
