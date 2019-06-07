// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GsUIWidgetBase.h"
#include "GsUIController.generated.h"




/**
 * abstract
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

	// 스택/스택사용안하는 객체
	void AddWidget(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters = nullptr);	
	
	// 객체 화면에서 삭제. 
	void RemoveWidget(UGsUIWidgetBase* InWidget);
	void RemoveWidget(FName InKey);

	// 안드로이드 백키처리
	void Back();

	virtual void RemoveAll();
	//virtual void ClearStack();
	//virtual void ClearCache();
	//virtual void Hide();
	//virtual void UnHide();

protected:
	void PushStack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters = nullptr);

	void PopStack(UGsUIWidgetBase* InWidget);

	virtual void AddToViewport(UGsUIWidgetBase* InWidget);

	virtual void RemoveUsingWidget(UGsUIWidgetBase* InWidget); // 음... 글로벌객체일때 완전히 지우는 코드를 여기에
	UGsUIWidgetBase* StackPeek();


protected:
	// 사용중인 (Stackable, Unstackable 에 포함된) 위젯들. 중복생성 허용 된 항목들은 빠져나가며 삭제됨
	UPROPERTY()
	TArray<UGsUIWidgetBase*> UsingWidgetArray;  // 플래그 켜주고 들어가면 지우지 않아도 될듯?

	// 한개만 캐싱함.
	UPROPERTY()
	TMap<FName, UGsUIWidgetBase*> CachedWidgetMap; // Key: FileName

	UPROPERTY()
	TArray<UGsUIWidgetBase*> StackableArray;
	
	//UPROPERTY()
	//TArray<UGsUIWidgetBase*> UnstackableArray; // 없어도 될듯

	// 특수객체 포인터 들고있는게 좋을듯. 로딩창/ HUD
};
