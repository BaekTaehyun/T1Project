// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "GsUINonDestructiveWidget.h"
#include "GsGlobalUIManager.generated.h"

/**
 * ���� ��ȯ���� �ı����� ���� UI �Ŵ���. ���� ������ ����...
 * ��: �ε�â, ������ UI, �ý��� �޽���...
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
