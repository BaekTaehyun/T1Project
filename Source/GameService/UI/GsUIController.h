// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GsUIWidgetBase.h"
#include "GsUIController.generated.h"

/**
 * ���� ���� Ŭ����. 
 * ���� ��ȯ �� �ı����� �ʴ� �������� �� Ŭ������ ����� UGsUIControllerNotDestroy �̿�
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

	// ���� ȭ�鿡 �߰�
	void AddWidget(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters = nullptr);	
	
	// ���� ȭ�鿡�� ���� 
	void RemoveWidget(UGsUIWidgetBase* InWidget);
	void RemoveWidget(FName InKey);

	// �ȵ���̵� ��Űó���� ���� �������̽�
	void Back();

	// ĳ�õ� �׸���� ���� ����
	virtual void RemoveAll();

protected:
	void PushStack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters = nullptr);
	void PopStack(UGsUIWidgetBase* InWidget);
	UGsUIWidgetBase* StackPeek();

	virtual void AddToViewport(UGsUIWidgetBase* InWidget);
	virtual void RemoveUsingWidget(UGsUIWidgetBase* InWidget);

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
