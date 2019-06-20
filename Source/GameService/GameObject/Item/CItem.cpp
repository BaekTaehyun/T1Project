// Fill out your copyright notice in the Description page of Project Settings.


#include "CItem.h"
#include "../../UI/GsUIIcon.h"

UCItem::UCItem(const int64 In_ItemTID)
{
}

void UCItem::UpdateItemData(int64  In_ItemID)
{
	//UE_LOG(LogTemp, Log, TEXT("UpdateItemData : %d"), In_ItemID);
	UE_LOG(LogTemp, Log, TEXT("Call UpdateItemData()"));

	// 아직 table data 가 없으므로, 물약 data 하드코딩 (임시).
	ItemTID = In_ItemID;
	ItemGUID = 101;
	ItemName = TEXT("체력회복 물약");
	ItemBPpath = TEXT("/Game/UI/Texture/icon_potion_01.icon_potion_01");
}

UCItem* UCItem::CreateItem(const int64 In_ItemTID, int32 In_ItemStackCount)
{
	// 인자로 받은 id 를 토대로 테이블에서 읽어와서 Find 및 Set 
	// CreateItem 은 여러가지 인자 타입이 생길 여지가 있음. (오버로딩 사용
	// 로블꺼 가져옴(임시)
	/*
	Item tableData = ItemDic.Instance.Find(In_itemID);
	if (tableData == null)
	{
		var msg = string.Format("Not exist item info in the item table. ItemId:{0}", In_itemID);
		throw new Exception(msg);
		return null;
	}
	*/
	UCItem* newItem = NewObject<UCItem>();
	if (nullptr != newItem)
	{
		newItem->UpdateItemData(In_ItemTID);
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
