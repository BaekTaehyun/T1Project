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

	// ����, ĳ��
	UGsUIWidgetBase* CreateOrFind(class UWorld* InOwner, TSubclassOf<UGsUIWidgetBase> InClass);
	virtual UGsUIWidgetBase* CreateOrFind(class UGameInstance* InOwner, TSubclassOf<UGsUIWidgetBase> InClass);

	// ����/���û����ϴ� ��ü
	void AddWidget(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters = nullptr);	
	
	// ��ü ȭ�鿡�� ����. 
	void RemoveWidget(UGsUIWidgetBase* InWidget);
	void RemoveWidget(FName InKey);

	// �ȵ���̵� ��Űó��
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

	virtual void RemoveUsingWidget(UGsUIWidgetBase* InWidget); // ��... �۷ι���ü�϶� ������ ����� �ڵ带 ���⿡
	UGsUIWidgetBase* StackPeek();


protected:
	// ������� (Stackable, Unstackable �� ���Ե�) ������. �ߺ����� ��� �� �׸���� ���������� ������
	UPROPERTY()
	TArray<UGsUIWidgetBase*> UsingWidgetArray;  // �÷��� ���ְ� ���� ������ �ʾƵ� �ɵ�?

	// �Ѱ��� ĳ����.
	UPROPERTY()
	TMap<FName, UGsUIWidgetBase*> CachedWidgetMap; // Key: FileName

	UPROPERTY()
	TArray<UGsUIWidgetBase*> StackableArray;
	
	//UPROPERTY()
	//TArray<UGsUIWidgetBase*> UnstackableArray; // ��� �ɵ�

	// Ư����ü ������ ����ִ°� ������. �ε�â/ HUD
};
