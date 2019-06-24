// Fill out your copyright notice in the Description page of Project Settings.


#include "CItem.h"
#include "ItemManager.h"
#include "../../UI/GsUIIcon.h"

UCItem::UCItem()
{
}

void UCItem::UpdateItemData(int64  In_ItemID , FGsItemTables* In_TableData)
{
	UE_LOG(LogTemp, Log, TEXT("Call UpdateItemData()"));

	ItemTID = In_TableData->ItemID;
	ItemGUID = 00001; // ObjID 현재는 없으므로 임시 설정.
	ItemName = In_TableData->ItemName;
	ItemBPpath = In_TableData->BP_Path;
	StorageType = In_TableData->ItemType;
	ItemTableData = In_TableData;

	//ItemBPpath = TEXT("/Game/UI/Texture/icon_potion_01.icon_potion_01");
}

void UCItem::UpdateItemStackCount(int32 In_StackCount)
{
	this->ItemStackCount = In_StackCount;
}

UCItem* UCItem::CreateItem(const int64 In_ItemTID, int32 In_ItemStackCount)
{
	// 인자로 받은 id 를 토대로 테이블에서 읽어와서 Find 및 Set 
	// CreateItem 은 여러가지 인자 타입이 생길 여지가 있음. (오버로딩 사용)
	/*
	Item tableData = ItemDic.Instance.Find(In_itemID);
	if (tableData == null)
	{
	var msg = string.Format("Not exist item info in the item table. ItemId:{0}", In_itemID);
	throw new Exception(msg);
	return null;
	}
	*/
	FGsItemTables* tableData = GItemManager()->GetFindTableData(In_ItemTID);
	if (nullptr == tableData)
	{
		UE_LOG(LogTemp, Log, TEXT("Not Exist ItemData : %d "), In_ItemTID);
		return nullptr;
	}

	UCItem* newItem = NewObject<UCItem>();
	if (nullptr != newItem)
	{
		newItem->UpdateItemData(In_ItemTID , tableData);
		newItem->ItemStackCount = In_ItemStackCount;
	}

	// 장착류 아이템

	// 장비에 박는 보석류?

	// 기타 아이템들



	return newItem;

}

int64 UCItem::GetItemTID()
{
	return ItemTID;
}

int64 UCItem::GetItemGUID()
{
	return ItemGUID;
}


int32 UCItem::GetItemStackCount()
{
	return ItemStackCount;
}

FString UCItem::GetItemName()
{
	return ItemName;
}

FString UCItem::GetItemBPpath()
{
	return ItemBPpath;
}

ItemStorageType UCItem::GetItemStorageType()
{
	return StorageType;
}