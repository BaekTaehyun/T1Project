// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemManager.h"
#include "UObject/ConstructorHelpers.h"
#include "../../Message/GsMessageManager.h"

template<>
FItemManager* FGsItemMgr::_instance = nullptr;

FItemManager::FItemManager()
{
	UE_LOG(LogTemp, Log, TEXT("Call FItemManager() !!!"));
	Items = new FCItemBuffers();

	UDataTable* _tmp = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/Data/ItemDataTable.ItemDataTable'") , nullptr, LOAD_None, nullptr);
	if (nullptr != _tmp)
	{
		LoadTableData = _tmp;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("LoadObject Data Load Failed !!!"));
	}
	/*
	static ConstructorHelpers::FObjectFinder<UDataTable> _tmp(TEXT("DataTable'/Game/Data/ItemDataTable.ItemDataTable'"));
	if (_tmp.Succeeded())
	{
		LoadTableData = _tmp.Object;
		UE_LOG(LogTemp, Log, TEXT("ItemDataTable Load Succeeded !!!"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("ItemDataTable Load Failed !!!"));
	}
	*/
}

FGsItemTables* FItemManager::GetFindTableData(int64 In_ItemTID)
{
	if (nullptr != LoadTableData)
	{
		FName _key = FName(*FString::FromInt(In_ItemTID));
		FGsItemTables* tableRow = LoadTableData->FindRow<FGsItemTables>(_key, TEXT(""));
		if (nullptr == tableRow)
		{
			UE_LOG(LogTemp, Log, TEXT("GetFindTableData() - Not Exist ItemData : %d "), In_ItemTID);
			return nullptr;
		}
		
		return tableRow;
	}

	return nullptr;
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

void FItemManager::UseItem(UCItem& In_useItem) const
{
	UE_LOG(LogTemp, Log, TEXT("Call UseItem - itemID : %d"), In_useItem.GetItemTID());
	// Item Check



	// Send UseItem Packet

}

/////////////// Find Item //////////////////
UCItem* FItemManager::FindItem(int64 In_ItemTID, ItemStorageType In_StorageType)
{
	UCItem* ownItem = Items->Find(In_ItemTID, In_StorageType);
	if (nullptr != ownItem)
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
// ���� �������� �����ִ� ��ŶŬ������ ���� �ؾ���.
UCItem* FItemManager::AddItem(UCItem& In_addItem)
{

	return nullptr;
}

// �ӽ÷� ���. (Ŭ������ �϶���.)
UCItem* FItemManager::AddItem(int64 In_ItemTID, ItemStorageType In_StorageType, int32 In_StackCount)
{
	UCItem* _item = FindItem(In_ItemTID, In_StorageType);
	if (nullptr != _item) // �̹� �������� ��.
	{
		UE_LOG(LogTemp, Log, TEXT("Call AddItem() - already exist!!!"));
		// �̹� �������̶�� �����͸� Update
		// ���� Item Data �� �������� �׶� ��� Update ��Ű�� �ɷ�..
		_item->UpdateItemStackCount(In_StackCount);
		GMessage()->GetItem().SendMessage(MessageItem::ItemAction::UPDATEITEM, *_item);
	}
	else // new item
	{
		UE_LOG(LogTemp, Log, TEXT("Call AddItem() - new item !!!"));
		// �������� �ƴ϶�� �ű� Add
		_item = UCItem::CreateItem(In_ItemTID, In_StackCount);
		if (nullptr != _item)
		{
			AddItem(_item);
			GMessage()->GetItem().SendMessage(MessageItem::ItemAction::ADDITEM, *_item);
		}
	}

	

	return _item;
}

void FItemManager::AddItem(UCItem* In_newItem)
{
	// newItem Add ��Ű��, Listener class �鿡�� msg ��ε�ĳ���� �ʿ�.
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

// �ӽ÷� ���. (Ŭ������ �϶���.)
UCItem* FCItemBuffers::Find(int64 In_ItemTID, ItemStorageType In_StorageType)
{
	if (true == _items.Contains((uint8)In_StorageType))
	{
		TArray<UCItem*>* _itemList = _items.Find((uint8)In_StorageType);
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
		UE_LOG(LogTemp, Log, TEXT("Check _items Keys : %d"), (uint8)In_StorageType);
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

