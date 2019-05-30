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

	/** ���� ���� */
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void PushByKeyName(FName InKey, class UGsUIParameter* InParam = nullptr);
	
	/** ���� ���� */
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Push(TSubclassOf<UGsUIWidgetBase> InClass, class UGsUIParameter* InParam = nullptr);

	/** ��� ���� ���� */
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	void Pop(UGsUIWidgetBase* InWidget);

	/** ���� �� ������ ��ȯ. ���� �����ִ� UI��� ���� �ȴ� */
	UFUNCTION(BlueprintCallable, Category = "GsUI")
	UGsUIWidgetBase* StackPeek(); // Current

	UFUNCTION(BlueprintCallable, Category = "GsUI")
	UGsUIWidgetBase* GetCachedWidget(FString InPathName);	

	//UFUNCTION(BlueprintCallable, Category = "GsUI") // TEST: �������Ʈ�� �����Ű�� ���� ��
	void RemoveAllStack();

	//UFUNCTION(BlueprintCallable, Category = "GsUI") // TEST: �������Ʈ�� �����Ű�� ���� ��
	void RemoveAllTray();

	/** ĳ���� �������� ����� */
	//UFUNCTION(BlueprintCallable, Category = "GsUI") // TEST: �������Ʈ�� �����Ű�� ���� ��
	void RemoveAll();

	//UFUNCTION(BlueprintCallable, Category = "GsUI") // TEST:
	//void TestForceGC();

	/** UIPathTable ���� �ش� Ű�� Row�� ã�� WidgetClass�� �����Ѵ� */
	TSubclassOf<UGsUIWidgetBase> GetWidgetClass(FName InKey);

protected:
	void PushStack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters = nullptr);
	void PushUnstack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters = nullptr);
	void PopStack(UGsUIWidgetBase* InWidget);
	void PopUnstack(UGsUIWidgetBase* InWidget);

	struct FGsTableUIPath* GetTableRow(FName InKey);
	void AddToViewport(UGsUIWidgetBase* InWidget);

protected:
	/** FName�� GetPathName �־ ���� */
	UPROPERTY()
	TMap<FName, UGsUIWidgetBase*> CachedWidgets; // Key: PathName

	UPROPERTY()
	TArray<UGsUIWidgetBase*> StackedWidgets;

	UPROPERTY()
	TArray<UGsUIWidgetBase*> UnstackedWidgets;  // �ν��Ͻ� ��� ������ �ʿ�������ѵ�

	UPROPERTY()
	UDataTable* WidgetClassTable;
};
