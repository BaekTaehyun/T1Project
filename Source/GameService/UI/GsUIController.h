// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GsUIWidgetBase.h"
#include "GsUIController.generated.h"

/**
 * 위젯 관리 클래스. 
 * 레벨 전환 시 파괴되지 않는 위젯들은 이 클래스를 상속한 UGsUIControllerNotDestroy 이용
 */
UCLASS()
class GAMESERVICE_API UGsUIController : public UObject
{
	GENERATED_BODY()

public:
	virtual void BeginDestroy() override;

	// 생성, 캐싱
	UGsUIWidgetBase* CreateOrFind(class UWorld* InOwner, TSubclassOf<UGsUIWidgetBase> InClass);
	virtual UGsUIWidgetBase* CreateOrFind(class UGameInstance* InOwner, TSubclassOf<UGsUIWidgetBase> InClass);

	// 위젯 화면에 추가
	void AddWidget(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters = nullptr);	
	
	// 위젯 화면에서 삭제 
	void RemoveWidget(UGsUIWidgetBase* InWidget);
	void RemoveWidget(FName InKey);

	// 안드로이드 백키처리를 위한 인터페이스
	void Back();

	// 캐시된 항목까지 전부 삭제
	virtual void RemoveAll();

protected:
	void PushStack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters = nullptr);
	void PopStack(UGsUIWidgetBase* InWidget);
	UGsUIWidgetBase* StackPeek();

	virtual void AddToViewport(UGsUIWidgetBase* InWidget);
	virtual void RemoveUsingWidget(UGsUIWidgetBase* InWidget);

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
