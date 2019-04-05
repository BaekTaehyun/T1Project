// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "T1PlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class T1PROJECT_API UT1PlayerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindPlayerStat(class UT1PlayerStatComponent* NewPlayerStat);

protected:
	virtual void NativeConstruct() override;
	void UPdateHPWidget();
	
private:
	TWeakObjectPtr<class UT1PlayerStatComponent> CurrentPlayerStat;

	UPROPERTY()
	class UProgressBar* HPProgressBar;
};
