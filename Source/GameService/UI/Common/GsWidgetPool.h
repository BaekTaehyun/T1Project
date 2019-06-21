// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprint/UserWidget.h"
#include "GsWidgetPool.generated.h"

/**
 * FUserWidgetPool�� �����Ͽ� ���ǿ� �°� ����
 * AddToViewport, RemoveFromParent�� ���⼭ �������� �ʰ� ���������� ���� �����Ѵ�. 
 * ���� �ݳ� ���� RemoveFromParent ȣ�⿡ ����
 * �ϴ� �������Ʈ���� ����ϴ� ��Ȳ�� �����ϰ� �����.
 */
UCLASS()
class GAMESERVICE_API UGsWidgetPool : public UObject
{
	GENERATED_BODY()
	
protected:
	//UPROPERTY()
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY()
	TArray<UUserWidget*> ActiveArray;
	
	UPROPERTY()
	TArray<UUserWidget*> InactiveArray;

public:
	void Initialize(TSubclassOf<UUserWidget> InWidgetClass, int32 InWarmUpCount = 0);
	void Release(UUserWidget* InWidget);
	void ReleaseAll();
	void WarmUp(int32 InWarmupCount = 0);
	void Empty();

	template <typename UserWidgetT = UUserWidget>
	UserWidgetT* Clame()
	{
		UUserWidget* widgetInstance = CreateOrGetInstance();

		return Cast<UserWidgetT>(widgetInstance);
	}

private:	
	UUserWidget* CreateOrGetInstance();
	UUserWidget* CreateWidgetInternal();
};
