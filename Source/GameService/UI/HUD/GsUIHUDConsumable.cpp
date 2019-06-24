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
		UCItem* items = GItemManager()->AddItem(1001 , ItemStorageType::Consumable, 0);
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

	// Event Add
	//GMessage()->GetItem().AddUObject(MessageItem::ItemAction::ADDITEM, this, &UGsUIHUDConsumable::UpdateConsumeItem);
	GMessage()->GetItem().AddUObject(MessageItem::ItemAction::UPDATEITEM, this, &UGsUIHUDConsumable::UpdateConsumeItem);
	GMessage()->GetItem().AddUObject(MessageItem::ItemAction::USEITEM, this, &UGsUIHUDConsumable::OnClickConsumeItem);
}

void UGsUIHUDConsumable::UpdateConsumeItem(UCItem& in_Item)
{
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

void UGsUIHUDConsumable::OnClickConsumeItem(UCItem& in_Item)
{
	UE_LOG(LogTemp, Log, TEXT("Call UGsUIHUDConsumable::OnClickConsumeItem() !!! "));
	GItemManager()->UseItem(in_Item);
}