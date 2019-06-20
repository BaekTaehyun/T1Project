// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../Class/GsSingleton.h"
#include "../../Class/GsManager.h"
#include "../../GameObject/Item/CItem.h"
#include "GameObject/Define/GsGameObjectDefine.h"

class FCItemBuffers;

class FItemManager :
	public TGsSingleton<FItemManager>,
	public IGsManager
{

private:
	FCItemBuffers* Items;

public:
	FItemManager();
	virtual ~FItemManager();

public:
	//IGsManager
	virtual void Initialize() override;
	virtual void Finalize() override;

public:
	// Find Item
	// StorageType , TID  아님 GUID 로 Find 하면 좋을듯.
	UCItem* FindItem(int64 In_ItemTID , ItemStorageType In_StorageType);
	UCItem* FindItem(int64 In_ItemGUID);

public:
	// Add Item
	UCItem* AddItem(UCItem& In_addItem);
	UCItem* AddItem(int64 In_ItemTID, ItemStorageType In_StorageType, int32 In_StackCount); // 임시

private:
	void AddItem(UCItem* In_newItem);

};

typedef TGsSingleton<FItemManager> FGsItemMgr;
#define GItemManager() FGsItemMgr::Instance()


/*
 *  저장 위치별 Item Buffer Class
 */

class FCItemBuffers
{

	// key : ItemStorageType  ///  value : TArray<UCItem>
private:
	TMap<uint8, TArray<UCItem*>> _items;

public:
	FCItemBuffers();
	~FCItemBuffers() = default;
	
public:
	void ClearItems();
	UCItem* Find(int64 In_ItemTID, ItemStorageType In_StorageType);
	UCItem* Find(int64 In_ItemGUID , TArray<ItemStorageType> In_arrStorageType);

public:
	void AddItems(UCItem* In_addItem);

};
