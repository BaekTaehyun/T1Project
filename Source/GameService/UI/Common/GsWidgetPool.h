// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprint/UserWidget.h"
#include "GsWidgetPool.generated.h"

/**
 * FUserWidgetPool을 참고하여 편의에 맞게 수정
 * AddToViewport, RemoveFromParent는 여기서 수행하지 않고 받은곳에서 각자 진행한다. 
 * 따라서 반납 전에 RemoveFromParent 호출에 유의
 * 일단 블루프린트에서 사용하는 상황은 배제하고 만든다.
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
