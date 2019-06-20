// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemManager.h"

template<>
FItemManager* FGsItemMgr::_instance = nullptr;

FItemManager::FItemManager()
{
	UE_LOG(LogTemp, Log, TEXT("Call FItemManager() !!!"));

	Items = new FCItemBuffers();
}

FItemManager::~FItemManager()
{
	Items->ClearItems();
}

void FItemManager::Finalize()
{
	TGsSingleton::RemoveInstance();
}

void FItemManager::Initialize()
{
	TGsSingleton::InitInstance(this);
}

/////////////// Find Item //////////////////
UCItem* FItemManager::FindItem(int64 In_ItemTID, ItemStorageType In_StorageType)
{
	UCItem* ownItem = Items->Find(In_ItemTID, In_StorageType);
	if(nullptr != ownItem)
	{
		return ownItem;
	}
	return nullptr;
}


UCItem* FItemManager::FindItem(int64 In_ItemGUID)
{
	//Items->Find(ItemStorageType::Consumable)
	
	return nullptr;
}


/////////////// Add Item //////////////////
// 추후 서버에서 내려주는 패킷클래스로 변경 해야함.
UCItem* FItemManager::AddItem(UCItem& In_addItem)
{
	return nullptr;
}

// 임시로 사용. (클라전용 일때만.)
UCItem* FItemManager::AddItem(int64 In_ItemTID , ItemStorageType In_StorageType , int32 In_StackCount)
{
	UCItem* _item = FindItem(In_ItemTID , In_StorageType);
	if(nullptr != _item) // 이미 보유중인 템.
	{
		UE_LOG(LogTemp, Log, TEXT("Call AddItem() - already exist!!!"));
		// 이미 보유중이라면 데이터만 Update
		// 차후 Item Data 가 정해지면 그때 모두 Update 시키는 걸로..
		_item->UpdateItemStackCount(In_StackCount);
	}
	else // new item
	{
		UE_LOG(LogTemp, Log, TEXT("Call AddItem() - new item !!!"));
		// 보유중이 아니라면 신규 Add
		_item = UCItem::CreateItem(In_ItemTID , In_StackCount);
		if (nullptr != _item)
		{
			AddItem(_item);
		}
	}

	return _item;
}

void FItemManager::AddItem(UCItem* In_newItem)
{
	// newItem Add 시키고, Listener class 들에게 msg 브로드캐스팅 필요.
	if (nullptr != In_newItem)
	{
		Items->AddItems(In_newItem);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////FCItemBuffers///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
FCItemBuffers::FCItemBuffers()
{
	UE_LOG(LogTemp, Log, TEXT("Call FCItemBuffers() !!!"));

	ClearItems();
	
	for (uint8 i = 0; i < (uint8)ItemStorageType::Max; ++i)
	{
		//UCItem* aa = NewObject<UCItem>();
		_items.Add(i, TArray<UCItem*>());
	}
}

// 임시로 사용. (클라전용 일때만.)
UCItem* FCItemBuffers::Find(int64 In_ItemTID , ItemStorageType In_StorageType)
{
	if (true == _items.Contains((uint8)In_StorageType))
	{
		TArray<UCItem*>* _itemList = _items.Find( (uint8)In_StorageType);
		if (nullptr != _itemList)
		{
			for (auto it = _itemList->CreateIterator(); it; ++it)
			{
				UCItem* item = *it;
				if (item->GetItemTID() == In_ItemTID)
				{
					return *it;
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Check _items Keys : %d") , (uint8)In_StorageType);
	}

	return nullptr;
}

UCItem* FCItemBuffers::Find(int64 In_ItemGUID, TArray<ItemStorageType> In_arrStorageType)
{

	return nullptr;
}

void FCItemBuffers::AddItems(UCItem* In_addItem)
{
	if (nullptr != In_addItem)
	{
		_items[(uint8)In_addItem->GetItemStorageType()].Insert(In_addItem, 0);
	}
}

void FCItemBuffers::ClearItems()
{
	if (_items.Num() > 0)
	{
		_items.Empty();
	}
}

