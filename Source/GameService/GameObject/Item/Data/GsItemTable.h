// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "../../Define/GsGameObjectDefine.h"
#include "GsItemTable.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API UGsItemTable : public UObject
{
	GENERATED_BODY()
	
};


USTRUCT(BlueprintType)
struct FGsItemTables : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FGsItemTables() : ItemID(0), ItemType(ItemStorageType::Max), ItemName(""), BP_Path(""), IntegerValue(0) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		int32 ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		ItemStorageType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		FString BP_Path;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		int32 IntegerValue;
		
};
