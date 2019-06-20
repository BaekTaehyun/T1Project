// Fill out your copyright notice in the Description page of Project Settings.


#include "GsUIHUDConsumable.h"
#include "../../GameObject/Item/Citem.h"
#include "../../GameObject/Item/ItemManager.h"

void UGsUIHUDConsumable::CreateConsumeItem()
{
	UE_LOG(LogTemp, Log, TEXT("CreateConsumeItem Call !!!"));

	//ItemIconSelector = CreateDefaultSubobject<UItemIconSelector>(TEXT("ItemImgRoot"));	
	if (nullptr != ItemIconSelector)
	{
		// ���� DataSheet �� ���⿡ �ӽ� Create.
		//GItemManager()->FindItem(123123);
		//UCItem* createItem = UCItem::CreateItem();

		UCItem* items = GItemManager()->AddItem(1, ItemStorageType::Consumable, 1);
		UWorld* world = GetWorld();
		ItemIconSelector->CreateItemIcon(UItemIconSelector::eItemIconSize::Large, ItemImgRoot , world);
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

}