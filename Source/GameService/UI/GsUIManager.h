// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GsUIWidgetBase.h"
#include "GsUIPathTable.h"
#include "Engine/DataTable.h"
#include "GsUIManager.generated.h"


/**
 * UGsUIManager
 * - ZOrder �켱����
 *   : HUD < Window < ��ȭâ < Popup < Ʃ�丮��â < Tray < �ε�â < �����˾� < System Popup < �翬��UI < System Tray
 * 
 * - Normal 
 *  : ������ȯ �� �ı�, OwningObject�� UWorld. Window ~ Tray.
 * - NotDestroy
 *  : ������ȯ�ص� �ı����� ����. OwningObject�� UGameInstance. �ε�â ~ System Tray.
 */
UCLASS()
class GAMESERVICE_API UGsUIManager : public UObject
{
	GENERATED_BODY()

public:
	UGsUIManager(const FObjectInitializer& ObjectInitializer);

	virtual void BeginDestroy() override;
	void Initialize();
	void RemoveAll();

	// Ǫ���ϸ� ���� ����
	TWeakObjectPtr<UGsUIWidgetBase> PushAndGetWidget(FName InKey, class UGsUIParameter* InParam = nullptr);

	// ���� ���Ϲ��� ����
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Push(FName InKey, class UGsUIParameter* InParam = nullptr);

	// ��� ���� ����
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Pop(UGsUIWidgetBase* InWidget);

	// �̸����� ��� ���� ����.
	// ����: �Ѱ� �̻� ���� �� �ִ� ����(bCanMultipleInstance == true)�� ����ϸ� �ȵȴ�.
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void PopByKeyName(FName InKey);

	// ����: ��������Ʈ�� ��ϵǾ� �ڵ�ȣ��Ǹ� ���� �� ����.
	void OnChangeLevel();
	void ShowLoading();
	void HideLoading();

protected:
	UGsUIWidgetBase* PushInter(FName InKey, class UGsUIParameter* InParam);
	UGsUIWidgetBase* CreateOrFind(TSubclassOf<UGsUIWidgetBase> InClass, bool bNotDestroy, class UGsUIParameter* InParam = nullptr);	
	struct FGsTableUIPath* GetTableRow(FName InKey);

protected:	
	// ���� Ŭ���� �н��� ���� ���̺�
	UPROPERTY()
	UDataTable* WidgetClassTable;

	UPROPERTY()
	class UGsUIController* UIControllerNormal;
	
	UPROPERTY()
	class UGsUIControllerNotDestroy* UIControllerNotDestroy;

	// �ε�â. ���� �Ҹ��Ƿ� �������ͷ� ����
	TWeakObjectPtr<UGsUIWidgetBase> LoadingWidget;

	/*
	// TEST
public:
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void TestGC();
	*/
};
