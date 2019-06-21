// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GsBlueprintFunctionLibraryUI.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API UGsBlueprintFunctionLibraryUI : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UIHelper")
	static class UGsUIManager* GetUIManager(class UWorld* InWorld);

	UFUNCTION(BlueprintCallable, Category = "UIHelper")
	static class UGsWidgetPoolManager* GetWidgetPoolManager(class UWorld* InWorld);
	
	UFUNCTION(BlueprintCallable, Category = "UIHelper")
	static class UGsUIDealScrollWidget* AddDealScroll(class UWorld* InWorld, FName InKey);

	//UFUNCTION(BlueprintCallable, Category = "UIHelper")
	//static void ReleaseDealScroll(class UWorld* InWorld, class UGsUIDealScrollWidget* InWidget);
};
