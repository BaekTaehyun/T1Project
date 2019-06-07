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
 * local: World가 Owning하며, 레벨 전환시 파괴
 * global: GameInstance가 Owning하며, 파괴되지 않음
 *
 * ZOrder 우선순위
 * HUD < // HUD 전용
 * Window < 대화창 < Popup < Tray < // 여기까지 local
 * 로딩창 < 종료팝업 < System Popup < 재연결UI < System Tray
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

	// 값을 리턴받지 않기 위함
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Push(FName InKey, class UGsUIParameter* InParam = nullptr);

	// 대상 위젯 제거
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Pop(UGsUIWidgetBase* InWidget);

	// 주의: 한개 이상 만들어지는 UI는 쓰면 안됨
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void PopByKeyName(FName InKey);

	// FIX: 테스트용. 지울예정
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void TestGC();

	// 개선: 델리게이트에 등록되어 자동호출되면 좋을 것 같다.
	void OnChangeLevel();
	void ShowLoading();
	void HideLoading();

protected:
	UGsUIWidgetBase* PushInter(FName InKey, class UGsUIParameter* InParam);

	struct FGsTableUIPath* GetTableRow(FName InKey);

	UGsUIWidgetBase* CreateOrFind(TSubclassOf<UGsUIWidgetBase> InClass, bool bNotDestroy, class UGsUIParameter* InParam = nullptr);

protected:
	
	// 위젯 클래스 패스를 가진 테이블을 가진다
	UPROPERTY()
	UDataTable* WidgetClassTable;

	UPROPERTY()
	class UGsUIController* UIControllerNormal;
	
	UPROPERTY()
	class UGsUIControllerNotDestroy* UIControllerNotDestroy;

	// 로딩창. 자주 불리므로 약포인터로 관리
	TWeakObjectPtr<UGsUIWidgetBase> LoadingWidget;
};
