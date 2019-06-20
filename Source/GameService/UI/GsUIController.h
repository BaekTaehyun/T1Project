// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GsUIWidgetBase.h"
#include "GsUIController.generated.h"

/**
 * 위젯 관리 클래스. 
 * 레벨 전환 시 파괴되지 않는 위젯들은 이 클래스를 상속한 UGsUIControllerNotDestroy 이용
 * 주의: UGsUIControllerNotDestroy 에서 공유하는 Viewport 관련 로직에 주의
 * - IsInViewport()  
 * - AddToViewport()
 * - RemoveFromParent()
 */
UCLASS()
class GAMESERVICE_API UGsUIController : public UObject
{
	GENERATED_BODY()

public:
	virtual void BeginDestroy() override;

	// 생성, 캐싱	
	UGsUIWidgetBase* CreateOrFind(class UGameInstance* InOwner, TSubclassOf<UGsUIWidgetBase> InClass, const FName& InKey);

	// 위젯 화면에 추가
	void AddWidget(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters = nullptr);	
	
	// 위젯 화면에서 삭제 
	void RemoveWidget(UGsUIWidgetBase* InWidget);
	void RemoveWidget(const FName& InKey);

	// 안드로이드 백키처리를 위한 인터페이스
	bool Back();

	// 스택에 있는 내용 삭제. 캐시된 위젯은 삭제하지 않음
	void ClearStack();

	// 캐시된 항목까지 전부 삭제
	virtual void RemoveAll();

	// 캐시된 UI 가져오기. 현재 사용여부 체크 파라미터.
	UGsUIWidgetBase* GetCachedWidgetByName(FName InKey, bool InActiveCheck);

protected:
	void PushStack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters = nullptr);
	void PopStack(UGsUIWidgetBase* InWidget);
	UGsUIWidgetBase* StackPeek();
	void RemoveUsingWidget(UGsUIWidgetBase* InWidget);
	bool IsTopInStack(UGsUIWidgetBase* InWidget);

	virtual void AddToViewport(UGsUIWidgetBase* InWidget);
	virtual void RemoveFromParent(UGsUIWidgetBase* InWidget);	

protected:
	// 사용중인 위젯들
	UPROPERTY()
	TArray<UGsUIWidgetBase*> UsingWidgetArray;

	// 대부분 재활용되지 않는 창이므로 한 개만 저장하여 재활용한다. UGsUIWidgetBase::IsCachedWidget().
	// 한 개 이후부터 생성된 인스턴스들은 UsingWidgetArray에서 빠지면 파괴된다.
	UPROPERTY()
	TMap<FName, UGsUIWidgetBase*> CachedWidgetMap; // Key: FileName

	UPROPERTY()
	TArray<UGsUIWidgetBase*> StackableArray;
};
