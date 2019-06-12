// Fill out your copyright notice in the Description page of Project Settings.


#include "GsUIHUDConsumable.h"
#include "../CItem.h"


void UGsUIHUDConsumable::CreateConsumeItem()
{
	UE_LOG(LogTemp, Log, TEXT("CreateConsumeItem Call !!!"));

	// 현재 DataSheet 가 없기에 임시 Create.
	UCItem* createItem = UCItem::CreateItem(123, 100);
}