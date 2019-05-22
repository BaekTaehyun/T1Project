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
	
	void RemoveAllStack();
	void RemoveAllTray();

	/** ĳ���� �������� ����� */
	UFUNCTION(BlueprintCallable, Category = "GsUI") // TEST: �������Ʈ�� �����Ű�� ���� ��
	void RemoveAll();

protected:
	void PushStack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters = nullptr);
	void PushUnstack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters = nullptr);
	void PopStack(UGsUIWidgetBase* InWidget);
	void PopUnstack(UGsUIWidgetBase* InWidget);

protected:
	/** FName�� GetPathName �־ ���� */
	UPROPERTY()
	TMap<FName, UGsUIWidgetBase*> CachedWidgets; // Key: PathName

	TArray<UGsUIWidgetBase*> StackedWidgets;
	TArray<UGsUIWidgetBase*> UnstackedWidgets;
};
