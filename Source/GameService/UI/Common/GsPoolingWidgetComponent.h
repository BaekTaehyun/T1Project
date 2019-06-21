// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GsPoolingWidgetComponent.generated.h"

/**
 * UWidgetComponent의 위젯을 풀에서 가져와 사용하도록 만든 컴포넌트
 */
UCLASS()
class GAMESERVICE_API UGsPoolingWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	virtual void InitWidget() override;
	virtual void ReleaseResources() override;
};
