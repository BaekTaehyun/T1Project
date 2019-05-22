// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GsUIWidgetBase.h"
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

	/** 위젯 띄우기 */
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Push(TSubclassOf<UGsUIWidgetBase> InClass, class UGsUIParameter* InParam = nullptr);

	/** 대상 위젯 제거 */
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Pop(UGsUIWidgetBase* InWidget);  // FIX: 네이밍. Remove 에 가깝다

	/** 가장 윗 스택을 반환. 현재 보고있는 UI라고 봐도 된다. */
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	UGsUIWidgetBase* StackPeek(); // Current

	UFUNCTION(BlueprintCallable, Category = "GsUI")
	UGsUIWidgetBase* GetCachedWidget(FString InPathName);
	
	void RemoveAllStack();
	void RemoveAllTray();

	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void RemoveAll(); // 캐싱한 정보까지 전부 지움

protected:
	void PushStack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters = nullptr); // 스택언스택 클래스 분리
	void PushUnstack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters = nullptr);
	void PopStack(UGsUIWidgetBase* InWidget);
	void PopUnstack(UGsUIWidgetBase* InWidget);

	/** 스택의 끝에서 Top Window 가 나올때까지의 UI를 보여주고, 그 이후부터는 가림*/
	void UpdateStack();

protected:
	
	// FName에 GetPathName 넣어서 관리. 코드에서도, 위젯에서도 사용가능하다.
	UPROPERTY()
	TMap<FName, UGsUIWidgetBase*> CachedWidgets; // Key: PathName

	TArray<UGsUIWidgetBase*> StackedWidgets;
	TArray<UGsUIWidgetBase*> UnstackedWidgets;
};
