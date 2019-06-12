// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GsUIController.h"
#include "GsUIControllerNotDestroy.generated.h"

/**
 * 레벨전환 시 파괴되지 않을 위젯 관리
 */
UCLASS()
class GAMESERVICE_API UGsUIControllerNotDestroy : public UGsUIController
{
	GENERATED_BODY()

public:
	virtual UGsUIWidgetBase* CreateOrFind(class UGameInstance* InOwner, TSubclassOf<UGsUIWidgetBase> InClass, const FName& InKey) override;
	virtual void RemoveAll() override;

protected:
	virtual void AddToViewport(UGsUIWidgetBase* InWidget) override;
	virtual void RemoveUsingWidget(UGsUIWidgetBase* InWidget) override;	
};
