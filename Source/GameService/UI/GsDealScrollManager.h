// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UI/Table/GsDealScrollTable.h"
#include "GsDealScrollManager.generated.h"

/**
 * 딜스크롤 출력 관리(미완)
 */
UCLASS()
class GAMESERVICE_API UGsDealScrollManager : public UObject
{
	GENERATED_BODY()
	
private:
	TSubclassOf<class UGsUIDealScrollWidget> WidgetClass;

	UPROPERTY()
	UDataTable* DealScrollDataTable;

	TWeakObjectPtr<class UGsWidgetPool> WidgetPool;

public:
	UGsDealScrollManager(const FObjectInitializer& ObjectInitializer);

	UGsUIDealScrollWidget* AddWidget(const FName& InKey);
	void ReleaseWidget(UGsUIDealScrollWidget* InWidget);

private:
	const struct FGsTableDealScroll* GetTableRow(const FName& InKey);
	class UGsWidgetPool* GetWidgetPool();
};
