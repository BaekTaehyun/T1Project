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
 * - ZOrder 우선순위
 *   : HUD < Window < 대화창 < Popup < 튜토리얼창 < Tray < 로딩창 < 종료팝업 < System Popup < 재연결UI < System Tray
 * 
 * - Normal 
 *  : 레벨전환 시 파괴, OwningObject는 UWorld. Window ~ Tray.
 * - NotDestroy
 *  : 레벨전환해도 파괴하지 않음. OwningObject는 UGameInstance. 로딩창 ~ System Tray.
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
	
	template<class T>
	T* GetHUD() const
	{
		return Cast<T>(CurrentHUDWidget.Get());
	}

	TWeakObjectPtr<UGsUIWidgetBase> GetCachedWidget(const FName& InKey, bool InActiveCheck);

protected:
	UGsUIWidgetBase* PushInter(const FName& InKey, class UGsUIParameter* InParam);
	UGsUIWidgetBase* CreateOrFind(TSubclassOf<UGsUIWidgetBase> InClass, bool bNotDestroy, const FName& InKey, class UGsUIParameter* InParam = nullptr);
	struct FGsTableUIPath* GetTableRow(const FName& InKey);

protected:	
	// 위젯 클래스 패스를 가진 테이블
	UPROPERTY()
	UDataTable* WidgetClassTable;

	// 파괴되는 위젯 관리
	UPROPERTY()
	class UGsUIController* UIControllerNormal;
	
	// 파괴되지 않는 위젯 관리
	UPROPERTY()
	class UGsUIControllerNotDestroy* UIControllerNotDestroy;

	// 로딩창. 원본은 UIControllerNotDestroy 에 있음.
	TWeakObjectPtr<UGsUIWidgetBase> LoadingWidget;

	// 현재 사용중인 HUD. 원본은 UIControllerNormal 에 있음.
	TWeakObjectPtr<UGsUIWidgetBase> CurrentHUDWidget;

	/*
	// TEST
public:
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void TestGC();
	*/
};
