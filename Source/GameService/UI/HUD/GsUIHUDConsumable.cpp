// Fill out your copyright notice in the Description page of Project Settings.


#include "GsUIHUDConsumable.h"
#include "../CItem.h"

void UGsUIHUDConsumable::CreateConsumeItem()
{
	UE_LOG(LogTemp, Log, TEXT("CreateConsumeItem Call !!!"));

	//ItemIconSelector = CreateDefaultSubobject<UItemIconSelector>(TEXT("ItemImgRoot"));	
	if (nullptr != ItemIconSelector)
	{
		// 현재 DataSheet 가 없기에 임시 Create.
		UCItem* createItem = UCItem::CreateItem(1000001, 500);
		UWorld* world = GetWorld();
		ItemIconSelector->CreateItemIcon(UItemIconSelector::eItemIconSize::Large, ItemImgRoot , world);
		ItemIconSelector->SetItemIcon(createItem);
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