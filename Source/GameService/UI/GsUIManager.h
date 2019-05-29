// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GsUIWidgetBase.h"
#include "Engine/DataTable.h"
#include "GsUIManager.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API AGsUIManager : public AHUD
{
	GENERATED_BODY()
	
public:
	AGsUIManager(const FObjectInitializer& ObjectInitializer);

	static AGsUIManager* GetUIManager(class APlayerController* InOwner);

	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaSeconds) override;

	/** 위젯 띄우기 */
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void PushByKeyName(FName InKey, class UGsUIParameter* InParam = nullptr);
	
	/** 위젯 띄우기 */
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Push(TSubclassOf<UGsUIWidgetBase> InClass, class UGsUIParameter* InParam = nullptr);

	/** 대상 위젯 제거 */
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Pop(UGsUIWidgetBase* InWidget);

	/** 가장 윗 스택을 반환. 현재 보고있는 UI라고 봐도 된다 */
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	UGsUIWidgetBase* StackPeek(); // Current

	UFUNCTION(BlueprintCallable, Category = "GsUI")
	UGsUIWidgetBase* GetCachedWidget(FString InPathName);	

	//UFUNCTION(BlueprintCallable, Category = "GsUI") // TEST: 블루프린트에 노출시키지 않을 것
	void RemoveAllStack();

	//UFUNCTION(BlueprintCallable, Category = "GsUI") // TEST: 블루프린트에 노출시키지 않을 것
	void RemoveAllTray();

	/** 캐싱한 정보까지 지운다 */
	//UFUNCTION(BlueprintCallable, Category = "GsUI") // TEST: 블루프린트에 노출시키지 않을 것
	void RemoveAll();

	//UFUNCTION(BlueprintCallable, Category = "GsUI") // TEST:
	//void TestForceGC();

	/** UIPathTable 에서 해당 키의 Row를 찾아 WidgetClass를 리턴한다 */
	TSubclassOf<UGsUIWidgetBase> GetWidgetClass(FName InKey);

protected:
	void PushStack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters = nullptr);
	void PushUnstack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters = nullptr);
	void PopStack(UGsUIWidgetBase* InWidget);
	void PopUnstack(UGsUIWidgetBase* InWidget);

	struct FGsTableUIPath* GetTableRow(FName InKey);
	void AddToViewport(UGsUIWidgetBase* InWidget);

protected:
	/** FName에 GetPathName 넣어서 관리 */
	UPROPERTY()
	TMap<FName, UGsUIWidgetBase*> CachedWidgets; // Key: PathName

	UPROPERTY()
	TArray<UGsUIWidgetBase*> StackedWidgets;

	UPROPERTY()
	TArray<UGsUIWidgetBase*> UnstackedWidgets;  // 인스턴스 목록 있으면 필요없을듯한데

	UPROPERTY()
	UDataTable* WidgetClassTable;
};
