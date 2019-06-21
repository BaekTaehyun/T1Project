// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GsUIWidgetBase.h"
#include "GsUIHUD.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API UGsUIHUD : public UGsUIWidgetBase
{
	GENERATED_BODY()

public:
	virtual int32 GetManagedDefaultZOrder() const override { return 1; }
	virtual EGsUIHideFlags GetHideFlagType() const { return EGsUIHideFlags::UI_HIDE_HUD; }
};
