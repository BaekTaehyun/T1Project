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
	TWeakObjectPtr<UGsUIWidgetBase> PushAndGetWidget(FName InKey, class UGsUIParameter* InParam = nullptr);

	// 값을 리턴받지 않음
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Push(FName InKey, class UGsUIParameter* InParam = nullptr);

	// 대상 위젯 제거
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Pop(UGsUIWidgetBase* InWidget);

	// 이름으로 대상 위젯 제거.
	// 주의: 한개 이상 만들 수 있는 위젯(bCanMultipleInstance == true)은 사용하면 안된다.
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void PopByKeyName(FName InKey);

	// 개선: 델리게이트에 등록되어 자동호출되면 좋을 것 같다.
	void OnChangeLevel();
	void ShowLoading();
	void HideLoading();

protected:
	UGsUIWidgetBase* PushInter(FName InKey, class UGsUIParameter* InParam);
	UGsUIWidgetBase* CreateOrFind(TSubclassOf<UGsUIWidgetBase> InClass, bool bNotDestroy, class UGsUIParameter* InParam = nullptr);	
	struct FGsTableUIPath* GetTableRow(FName InKey);

protected:	
	// 위젯 클래스 패스를 가진 테이블
	UPROPERTY()
	UDataTable* WidgetClassTable;

	UPROPERTY()
	class UGsUIController* UIControllerNormal;
	
	UPROPERTY()
	class UGsUIControllerNotDestroy* UIControllerNotDestroy;

	// 로딩창. 자주 불리므로 약포인터로 관리
	TWeakObjectPtr<UGsUIWidgetBase> LoadingWidget;

	/*
	// TEST
public:
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void TestGC();
	*/
};
