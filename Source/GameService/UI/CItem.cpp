// Fill out your copyright notice in the Description page of Project Settings.


#include "CItem.h"


UCItem::UCItem(const int64 In_ItemTID)
{
	
}


void UCItem::UpdateItemData(const int64  In_ItemID)
{
	
	UE_LOG(LogTemp, Log, TEXT("UpdateItemData : %d"), In_ItemID);
}


UCItem* UCItem::CreateItem(const int64 In_ItemTID, int32 In_ItemStackCount)
{
	// ���ڷ� ���� id �� ���� ���̺��� �о�ͼ� Find �� Set 
	// CreateItem �� �������� ���� Ÿ���� ���� ������ ����. (�����ε� ���
	// �κ� ������(�ӽ�)
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

	// ������ ������

	// ��� �ڴ� ������?

	// ��Ÿ �����۵�
	

	
	return newItem;

}


