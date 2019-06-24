// Fill out your copyright notice in the Description page of Project Settings.


#include "GsUIHUDConsumable.h"
#include "../../GameObject/Item/Citem.h"
#include "../../GameObject/Item/ItemManager.h"
#include "../../Message/GsMessageItem.h"
#include "../../Message/GsMessageManager.h"


void UGsUIHUDConsumable::CreateConsumeItem()
{
	UE_LOG(LogTemp, Log, TEXT("CreateConsumeItem Call !!!"));

	//ItemIconSelector = CreateDefaultSubobject<UItemIconSelector>(TEXT("ItemImgRoot"));	
	if (nullptr != ItemIconSelector)
	{
		// ���� DataSheet �� ���⿡ �ӽ� Create.
		//GItemManager()->FindItem(123123);
		//UCItem* createItem = UCItem::CreateItem();

		UCItem* items = GItemManager()->AddItem(1001 , ItemStorageType::Consumable, 1);
		UWorld* world = GetWorld();
		ItemIconSelector->CreateItemIcon(UItemIconSelector::eItemIconSize::Large, ItemImgRoot, world);
		ItemIconSelector->SetItemIcon(items);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("ItemIconSelector is null !!!"));
	}
}

void UGsUIHUDConsumable::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ItemIconSelector = NewObject<UItemIconSelector>();

	GMessage()->GetItem().AddUObject(MessageItem::ItemAction::ADDITEM, this, &UGsUIHUDConsumable::UpdateConsumeItem);
	GMessage()->GetItem().AddUObject(MessageItem::ItemAction::UPDATEITEM, this, &UGsUIHUDConsumable::UpdateConsumeItem);
}

void UGsUIHUDConsumable::UpdateConsumeItem(UCItem& in_Item)
{
	UE_LOG(LogTemp, Log, TEXT("Call UpdateConsumeItem !!! "));

	UE_LOG(LogTemp, Log, TEXT("UpdateConsumeItem - ItemTID : %d"), in_Item.GetItemTID());
	UCItem* findItem = GItemManager()->FindItem(in_Item.GetItemTID() , in_Item.GetItemStorageType());
	if (nullptr != findItem)
	{
		ItemIconSelector->SetItemIcon(findItem);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("UpdateConsumeItem - Not Exist ItemTID : %d") , in_Item.GetItemTID() );
	}
}