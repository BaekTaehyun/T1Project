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
 * - ZOrder 우선순위
 *   : HUD < Window < 대화창 < Popup < 튜토리얼창 < Tray < 로딩창 < 종료팝업 < System Popup < 재연결UI < System Tray
 * 
 * - Normal 
 *  : 레벨전환 시 파괴, OwningObject는 UWorld. Window ~ Tray.
 * - NotDestroy
 *  : 레벨전환해도 파괴하지 않음. OwningObject는 UGameInstance. 로딩창 ~ System Tray.
 *  : 주의: RemoveFromViewport가 일어나지 않아 뎁스관리가 되지 않는다. 명시적으로 depth를 박거나, 중복허용 할 것.
 */
UCLASS()
class GAMESERVICE_API UGsUIManager : public UObject
{
	GENERATED_BODY()

public:
	// UI 감추기/복구 이벤트. 추후에 Flag값을 넘겨서 필요한 것만 끄도록 수정해도 좋을 것.
	FGsUIHideDelegate OnUIHide;
	

private:
	// 위젯 클래스 패스를 가진 테이블
	UPROPERTY()
	UDataTable* WidgetClassTable;

	// 파괴되는 위젯 관리
	UPROPERTY()
	class UGsUIController* UIControllerNormal;

	// 파괴되지 않는 위젯 관리
	UPROPERTY()
	class UGsUIControllerNotDestroy* UIControllerNotDestroy;

	UPROPERTY(Transient)
	class UGsWidgetPoolManager* WidgetPoolManager;

	UPROPERTY(Transient)
	class UGsDealScrollManager* DealScrollManager;

	// 로딩창. 원본은 UIControllerNotDestroy 에 있음.
	TWeakObjectPtr<UGsUIWidgetBase> LoadingWidget;

	// 현재 사용중인 HUD. 원본은 UIControllerNormal 에 있음.
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

	// 푸시하며 값을 얻어옴
	TWeakObjectPtr<UGsUIWidgetBase> PushAndGetWidget(const FName& InKey, class UGsUIParameter* InParam = nullptr);

	// 값을 리턴받지 않음
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Push(FName InKey, class UGsUIParameter* InParam = nullptr);

	// 대상 위젯 제거
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Pop(UGsUIWidgetBase* InWidget);

	// 이름으로 대상 위젯 제거.
	// 주의: 한개 이상 만들 수 있는 위젯(bCanMultipleInstance == true)은 사용하면 안된다.
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void PopByKeyName(const FName& InKey);

	// 개선: 델리게이트에 등록되어 자동호출되면 좋을 것 같다.
	void OnChangeLevel();
	void ShowLoading();
	void HideLoading();

	// HUD는 그리 자주 생성하는 항목이 아니므로 별도 
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
