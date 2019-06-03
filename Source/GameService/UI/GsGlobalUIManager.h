// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "GsUINonDestructiveWidget.h"
#include "GsGlobalUIManager.generated.h"

/**
 * 레벨 전환에도 파괴되지 않을 UI 매니저. 많이 만들지 말자...
 * 예: 로딩창, 재접속 UI, 시스템 메시지...
 */
UCLASS()
class GAMESERVICE_API UGsGlobalUIManager : public UObject
{
	GENERATED_BODY()

public:
	//UGsGlobalUIManager();
	UGsGlobalUIManager(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "GlobalUI")
	void ShowLoading(bool InVisible);

private:
	UPROPERTY(EditDefaultsOnly, Category = "GlobalUI")
	TSubclassOf<class UGsUINonDestructiveWidget> LoadingWidgetClass;

	UPROPERTY()
	class UGsUINonDestructiveWidget* LoadingWidget;

	void OnLevelLoadStart();
	void OnLevelLoadComplete();
};
