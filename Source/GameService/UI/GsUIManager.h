// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GsUIWidgetBase.h"
#include "GsUIPathTable.h"
#include "Engine/DataTable.h"
#include "GsUIManager.generated.h"


/**
 * UGsUIController
 * local: World�� Owning�ϸ�, ���� ��ȯ�� �ı�
 * global: GameInstance�� Owning�ϸ�, �ı����� ����
 *
 * ZOrder �켱����
 * HUD < // HUD ����
 * Window < ��ȭâ < Popup < Tray < // ������� local
 * �ε�â < �����˾� < System Popup < �翬��UI < System Tray
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

	// ���� ���Ϲ��� �ʱ� ����
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Push(FName InKey, class UGsUIParameter* InParam = nullptr);

	// ��� ���� ����
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Pop(UGsUIWidgetBase* InWidget);

	// ����: �Ѱ� �̻� ��������� UI�� ���� �ȵ�
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void PopByKeyName(FName InKey);

	// FIX: �׽�Ʈ��. ���￹��
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void TestGC();

	// ����: ��������Ʈ�� ��ϵǾ� �ڵ�ȣ��Ǹ� ���� �� ����.
	void OnChangeLevel();
	void ShowLoading();
	void HideLoading();

protected:
	UGsUIWidgetBase* PushInter(FName InKey, class UGsUIParameter* InParam);

	struct FGsTableUIPath* GetTableRow(FName InKey);

	UGsUIWidgetBase* CreateOrFind(TSubclassOf<UGsUIWidgetBase> InClass, bool bNotDestroy, class UGsUIParameter* InParam = nullptr);

protected:
	
	// ���� Ŭ���� �н��� ���� ���̺��� ������
	UPROPERTY()
	UDataTable* WidgetClassTable;

	UPROPERTY()
	class UGsUIController* UIControllerNormal;
	
	UPROPERTY()
	class UGsUIControllerNotDestroy* UIControllerNotDestroy;

	// �ε�â. ���� �Ҹ��Ƿ� �������ͷ� ����
	TWeakObjectPtr<UGsUIWidgetBase> LoadingWidget;
};
