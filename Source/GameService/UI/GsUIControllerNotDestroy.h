// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GsUIController.h"
#include "GsUIControllerNotDestroy.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API UGsUIControllerNotDestroy : public UGsUIController
{
	GENERATED_BODY()

public:
	virtual UGsUIWidgetBase* CreateOrFind(class UGameInstance* InOwner, TSubclassOf<UGsUIWidgetBase> InClass) override;
	virtual void RemoveAll() override;

protected:
	virtual void AddToViewport(UGsUIWidgetBase* InWidget) override;
	virtual void RemoveUsingWidget(UGsUIWidgetBase* InWidget) override;
	
};
