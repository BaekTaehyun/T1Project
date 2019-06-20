// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Define/GsGameObjectDefine.h"
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

public:
	int64 GetItemTID();
	int64 GetItemGUID();
	int32 GetItemStackCount();
	FString GetItemName();
	FString GetItemBPpath();
	ItemStorageType GetItemStorageType();

protected:
	int64 ItemTID = 0;
	int64 ItemGUID = 0;
	int32 ItemStackCount = 0;
	FString ItemName = "";
	FString ItemBPpath = "";
	ItemStorageType StorageType = ItemStorageType::Max;

public:
	static UCItem* CreateItem(const int64 In_ItemTID, int32 In_ItemStackCount = 0);

public:
	void UpdateItemData(int64 In_ItemID);
	void UpdateItemStackCount(int32 In_StackCount);


};
