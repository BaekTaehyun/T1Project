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
	
	// 테이블 키를 통해 위젯 띄우기
	TWeakObjectPtr<UGsUIWidgetBase> PushByKeyName(FName InKey, class UGsUIParameter* InParam = nullptr);

	// 위젯 띄우기
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Push(TSubclassOf<UGsUIWidgetBase> InClass, class UGsUIParameter* InParam = nullptr);

	// 대상 위젯 제거
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Pop(UGsUIWidgetBase* InWidget);

	// 안드로이드 백키 처리
	void Back();

	// 사용 중인 위젯 삭제. 캐시된 것은 지우지 않음
	void RemoveAllStack();
	void RemoveAllNoStack();
	
	// 캐시된 위젯까지 전부 지움
	void RemoveAll();	

	// UIPathTable 에서 해당 키의 Row를 찾아 WidgetClass를 리턴
	TSubclassOf<UGsUIWidgetBase> GetWidgetClass(FName InKey);

protected:
	UGsUIWidgetBase* PushInter(TSubclassOf<UGsUIWidgetBase> InClass, class UGsUIParameter* InParam = nullptr);
	void PushStack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters = nullptr);
	void PushNoStack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters = nullptr);
	void PopStack(UGsUIWidgetBase* InWidget);
	void PopNoStack(UGsUIWidgetBase* InWidget);

	struct FGsTableUIPath* GetTableRow(FName InKey);
	void AddToViewport(UGsUIWidgetBase* InWidget);
	UGsUIWidgetBase* CreateWidgetInter(TSubclassOf<UGsUIWidgetBase> InClass);

	// 가장 윗 스택을 반환
	TWeakObjectPtr<UGsUIWidgetBase> StackPeek();

protected:
	// 위젯 클래스 패스를 가진 테이블을 가진다
	UPROPERTY()
	UDataTable* WidgetClassTable;

	// 사용되고 있는 위젯(Stack, NoStack에 들어있는 위젯). 중복 사용을 허용.
	UPROPERTY()
	TArray<UGsUIWidgetBase*> UsingWidgets;
	
	// 한 개만 캐싱한다. 성능에 큰 문제 없으면 캐싱을 안해도 될 것 같다.
	UPROPERTY()
	TMap<FName, UGsUIWidgetBase*> CachedWidgets; // Key: PathName	
		
	TArray<TWeakObjectPtr<UGsUIWidgetBase>> Stack;
	TArray<TWeakObjectPtr<UGsUIWidgetBase>> NoStack;
};
