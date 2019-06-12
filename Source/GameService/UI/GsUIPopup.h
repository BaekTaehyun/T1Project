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
	UGsUIPopup(const FObjectInitializer& ObjectInitializer);

	/** 스택시키는 UI인가 */
	virtual bool IsStackUI() const final { return true; }

	/** 윈도우타입(화면을 다 가리는)인가 */
	virtual bool IsWindow() const final { return false; }

	virtual int32 GetManagedDefaultZOrder() const override { return 100; }

	virtual bool CanMultipleInstance() const override { return bCanMultipleInstance; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "GsManaged")
	bool bCanMultipleInstance;
};
