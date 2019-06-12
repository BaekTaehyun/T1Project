// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsUIWidgetBase.h"
#include "GsUIPopup.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API UGsUIPopup : public UGsUIWidgetBase
{
	GENERATED_BODY()

public:
	/** ���ý�Ű�� UI�ΰ� */
	virtual bool IsStackUI() const final { return true; }

	/** ������Ÿ��(ȭ���� �� ������)�ΰ� */
	virtual bool IsWindow() const final { return false; }

protected:
	virtual int32 GetManagedDefaultZOrder() const override { return 100; }
};