// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GsUIWidgetBase.h"
#include "GsUIPathTable.h"
#include "Engine/DataTable.h"
#include "GsUIManager.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGsUIHideDelegate, int32, InHideFlags);

/**
 * UGsUIManager
 * - ZOrder �켱����
 *   : HUD < Window < ��ȭâ < Popup < Ʃ�丮��â < Tray < �ε�â < �����˾� < System Popup < �翬��UI < System Tray
 * 
 * - Normal 
 *  : ������ȯ �� �ı�, OwningObject�� UWorld. Window ~ Tray.
 * - NotDestroy
 *  : ������ȯ�ص� �ı����� ����. OwningObject�� UGameInstance. �ε�â ~ System Tray.
 *  : ����: RemoveFromViewport�� �Ͼ�� �ʾ� ���������� ���� �ʴ´�. ��������� depth�� �ڰų�, �ߺ���� �� ��.
 */
UCLASS()
class GAMESERVICE_API UGsUIManager : public UObject
{
	GENERATED_BODY()

public:
	// UI ���߱�/���� �̺�Ʈ. ���Ŀ� Flag���� �Ѱܼ� �ʿ��� �͸� ������ �����ص� ���� ��.
	FGsUIHideDelegate OnUIHide;
	

private:
	// ���� Ŭ���� �н��� ���� ���̺�
	UPROPERTY()
	UDataTable* WidgetClassTable;

	// �ı��Ǵ� ���� ����
	UPROPERTY()
	class UGsUIController* UIControllerNormal;

	// �ı����� �ʴ� ���� ����
	UPROPERTY()
	class UGsUIControllerNotDestroy* UIControllerNotDestroy;

	UPROPERTY(Transient)
	class UGsWidgetPoolManager* WidgetPoolManager;

	UPROPERTY(Transient)
	class UGsDealScrollManager* DealScrollManager;

	// �ε�â. ������ UIControllerNotDestroy �� ����.
	TWeakObjectPtr<UGsUIWidgetBase> LoadingWidget;

	// ���� ������� HUD. ������ UIControllerNormal �� ����.
	TWeakObjectPtr<UGsUIWidgetBase> CurrentHUDWidget;

	EGsUIHideFlags HideFlags = EGsUIHideFlags::UI_HIDE_NONE;

public:
	UGsUIManager(const FObjectInitializer& ObjectInitializer);

	virtual void BeginDestroy() override;	

public:
	void Initialize();
	void RemoveAll();

	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void ClearStack();

	// Ǫ���ϸ� ���� ����
	TWeakObjectPtr<UGsUIWidgetBase> PushAndGetWidget(const FName& InKey, class UGsUIParameter* InParam = nullptr);

	// ���� ���Ϲ��� ����
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Push(FName InKey, class UGsUIParameter* InParam = nullptr);

	// ��� ���� ����
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Pop(UGsUIWidgetBase* InWidget);

	// �̸����� ��� ���� ����.
	// ����: �Ѱ� �̻� ���� �� �ִ� ����(bCanMultipleInstance == true)�� ����ϸ� �ȵȴ�.
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void PopByKeyName(const FName& InKey);

	// ����: ��������Ʈ�� ��ϵǾ� �ڵ�ȣ��Ǹ� ���� �� ����.
	void OnChangeLevel();
	void ShowLoading();
	void HideLoading();

	// HUD�� �׸� ���� �����ϴ� �׸��� �ƴϹǷ� ���� 
	void ChangeHUD(const FName& InKey);

	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void HideAll();

	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void UnhideAll();

private:
	UGsUIWidgetBase* PushInter(const FName& InKey, class UGsUIParameter* InParam);
	struct FGsTableUIPath* GetTableRow(const FName& InKey);
	void SetHideFlags(EGsUIHideFlags InFlag);
	void ClearHideFlags(EGsUIHideFlags InFlag);

public:
	template<class T>
	T* GetHUD() const
	{
		return Cast<T>(CurrentHUDWidget.Get());
	}

	TWeakObjectPtr<UGsUIWidgetBase> GetCachedWidget(const FName& InKey, bool InActiveCheck);
	class UGsWidgetPoolManager* GetWidgetPoolManager() const { return WidgetPoolManager; }
	class UGsDealScrollManager* GetDealScrollManager() const { return DealScrollManager; }

	/*
	// TEST
public:
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void TestGC();

	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void RemoveNormal();

	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Back();
	*/
};
