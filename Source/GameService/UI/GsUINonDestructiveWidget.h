// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GsUIWidgetBase.h"
#include "GsUINonDestructiveWidget.generated.h"

/**
 * ������ �ٲ� ������ ���� ������ü
 * ����ũ: https://irem.tistory.com/27
 */
UCLASS()
class GAMESERVICE_API UGsUINonDestructiveWidget : public UGsUIWidgetBase
{
	GENERATED_BODY()

public:
	UGsUINonDestructiveWidget(const FObjectInitializer& ObjectInitializer);

	virtual void RemoveFromParent() override;

	// ���� �ε� �� �ı����� �ʴ� UI�ΰ�
	virtual bool IsNondestructiveWidget() const override { return true; }

private:
	// ����: ������ �ȿ����� ����ϴ� �Լ��Դϴ�.
	void SetEnableAutoDestroy(bool bInEnableAutoDestroy);

private:
	bool bEnableAutoDestroy;
};
