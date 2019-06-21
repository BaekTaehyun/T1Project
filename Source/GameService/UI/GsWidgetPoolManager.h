// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GsWidgetPoolManager.generated.h"

class UUserWidget;
class UGsWidgetPool;

/**
 * UserWidget 풀 관리. key는 해당 위젯의 pathName
 */
UCLASS()
class GAMESERVICE_API UGsWidgetPoolManager : public UObject
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TMap<FName, UGsWidgetPool*> PoolMap; // key: PathName

public:
	static UGsWidgetPoolManager* GetManager(UWorld* InWorld);

public:
	virtual void BeginDestroy() override;

public:
	void Initialize();
	void RemoveAll();
	void ReleaseAll();

	UGsWidgetPool* CreateOrGetPool(TSubclassOf<UUserWidget> InWidgetClass);
	UUserWidget* CreateOrGetWidget(TSubclassOf<UUserWidget> InWidgetClass);
	void ReleaseWidget(TSubclassOf<UUserWidget> InWidgetClass, UUserWidget* Widget);
	UGsWidgetPool* FindPool(TSubclassOf<UUserWidget> InWidgetClass);

	//
	template <typename UserWidgetT = UUserWidget>
	UserWidgetT* CreateOrGetWidgetWithCast(TSubclassOf<UUserWidget> InWidgetClass)
	{
		UUserWidget* widget = CreateOrGetWidget(InWidgetClass);

		return Cast<UserWidgetT>(widget);
	}
};
