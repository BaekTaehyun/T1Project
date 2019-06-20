// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GsUIWidgetBase.h"
#include "GsUIController.generated.h"

/**
 * ���� ���� Ŭ����. 
 * ���� ��ȯ �� �ı����� �ʴ� �������� �� Ŭ������ ����� UGsUIControllerNotDestroy �̿�
 * ����: UGsUIControllerNotDestroy ���� �����ϴ� Viewport ���� ������ ����
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

	// ����, ĳ��	
	UGsUIWidgetBase* CreateOrFind(class UGameInstance* InOwner, TSubclassOf<UGsUIWidgetBase> InClass, const FName& InKey);

	// ���� ȭ�鿡 �߰�
	void AddWidget(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters = nullptr);	
	
	// ���� ȭ�鿡�� ���� 
	void RemoveWidget(UGsUIWidgetBase* InWidget);
	void RemoveWidget(const FName& InKey);

	// �ȵ���̵� ��Űó���� ���� �������̽�
	bool Back();

	// ���ÿ� �ִ� ���� ����. ĳ�õ� ������ �������� ����
	void ClearStack();

	// ĳ�õ� �׸���� ���� ����
	virtual void RemoveAll();

	// ĳ�õ� UI ��������. ���� ��뿩�� üũ �Ķ����.
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
	// ������� ������
	UPROPERTY()
	TArray<UGsUIWidgetBase*> UsingWidgetArray;

	// ��κ� ��Ȱ����� �ʴ� â�̹Ƿ� �� ���� �����Ͽ� ��Ȱ���Ѵ�. UGsUIWidgetBase::IsCachedWidget().
	// �� �� ���ĺ��� ������ �ν��Ͻ����� UsingWidgetArray���� ������ �ı��ȴ�.
	UPROPERTY()
	TMap<FName, UGsUIWidgetBase*> CachedWidgetMap; // Key: FileName

	UPROPERTY()
	TArray<UGsUIWidgetBase*> StackableArray;
};
