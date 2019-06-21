// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GsPoolingWidgetComponent.generated.h"

/**
 * UWidgetComponent�� ������ Ǯ���� ������ ����ϵ��� ���� ������Ʈ
 */
UCLASS()
class GAMESERVICE_API UGsPoolingWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	virtual void InitWidget() override;
	virtual void ReleaseResources() override;
};
