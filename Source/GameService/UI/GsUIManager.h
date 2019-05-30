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
	
	// ���̺� Ű�� ���� ���� ����
	TWeakObjectPtr<UGsUIWidgetBase> PushByKeyName(FName InKey, class UGsUIParameter* InParam = nullptr);

	// ���� ����
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Push(TSubclassOf<UGsUIWidgetBase> InClass, class UGsUIParameter* InParam = nullptr);

	// ��� ���� ����
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Pop(UGsUIWidgetBase* InWidget);

	// �ȵ���̵� ��Ű ó��
	void Back();

	// ��� ���� ���� ����. ĳ�õ� ���� ������ ����
	void RemoveAllStack();
	void RemoveAllNoStack();
	
	// ĳ�õ� �������� ���� ����
	void RemoveAll();	

	// UIPathTable ���� �ش� Ű�� Row�� ã�� WidgetClass�� ����
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

	// ���� �� ������ ��ȯ
	TWeakObjectPtr<UGsUIWidgetBase> StackPeek();

protected:
	// ���� Ŭ���� �н��� ���� ���̺��� ������
	UPROPERTY()
	UDataTable* WidgetClassTable;

	// ���ǰ� �ִ� ����(Stack, NoStack�� ����ִ� ����). �ߺ� ����� ���.
	UPROPERTY()
	TArray<UGsUIWidgetBase*> UsingWidgets;
	
	// �� ���� ĳ���Ѵ�. ���ɿ� ū ���� ������ ĳ���� ���ص� �� �� ����.
	UPROPERTY()
	TMap<FName, UGsUIWidgetBase*> CachedWidgets; // Key: PathName	
		
	TArray<TWeakObjectPtr<UGsUIWidgetBase>> Stack;
	TArray<TWeakObjectPtr<UGsUIWidgetBase>> NoStack;
};
