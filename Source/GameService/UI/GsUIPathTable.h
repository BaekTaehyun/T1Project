// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "GsUIPathTable.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API UGsUIPathTable : public UObject
{
	GENERATED_BODY()

};


USTRUCT(BlueprintType)
struct FGsTableUIPath : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FGsTableUIPath()
		: WidgetClass(nullptr)
	{
	}

	// CSV 첫번째열 LookupKey

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSoftClassPtr<class UGsUIWidgetBase> WidgetClass;
};
