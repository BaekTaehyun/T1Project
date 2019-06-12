// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CItem.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API UCItem : public UObject
{
	GENERATED_BODY()

public:
	UCItem() = default;

protected:
	UCItem(const int64 In_ItemTID);
	//UCItem(const UCItem&) = delete;
	//UCItem(UCItem&&) = delete;
	

protected:
	int64 ItemTID = 0;
	int64 ItemGUID = 0;
	FString ItemName = "";
	int32 ItemStackCount = 0;

public:
	static UCItem* CreateItem(const int64 In_ItemTID, int32 In_ItemStackCount = 0);

public :
	void UpdateItemData(const int64 In_ItemID);

	
};
