// Fill out your copyright notice in the Description page of Project Settings.


#include "GsCheatManager.h"
#include "GameMode/GsGameModeWorld.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "GameObject/GsSpawnComponent.h"
#include "GameObject/ObjectClass/GsGameObjectWheelVehicle.h"
#include "GameObject/ObjectClass/GsGameObjectLocal.h"
#include "GameObject/Item/ItemManager.h"

void UGsCheatManager::TeleportPlayer(FString in_tag)
{	
	UWorld* world = GetWorld();

	if (world)
	{		
		AGameModeBase* mode = world->GetAuthGameMode();

		if (mode)
		{
			AGsGameModeWorld* gameMode = Cast<AGsGameModeWorld>(mode);
			
			if (gameMode)
			{
				gameMode->TeleportPlayer(in_tag);
			}
		}		
	}	
}

void UGsCheatManager::SpawnGameObject(EGsGameObjectType Type, TSubclassOf<AActor> ActorClass, int SpawnCount, const FVector& StartPos)
{
	check(GSpawner());

	FVector spawnPos = StartPos;
	FVector offset = FVector::ZeroVector;

	if (auto findobj = GSpawner()->FindObject(EGsGameObjectType::LocalPlayer))

	for (int i = 0; i < SpawnCount; ++i)
	{
		auto myActor = Cast<UGsGameObjectLocal>(findobj)->GetActor();
		FVector dir = myActor->GetActorForwardVector();
		spawnPos = dir * 1000.f + myActor->GetActorLocation() + offset;

		GSpawner()->SpawnObject(Type, ActorClass.Get(), spawnPos, FRotator::ZeroRotator);

		offset.X = FMath::FRandRange(1.f, 1000.f);
		offset.Y = FMath::FRandRange(1.f, 1000.f);
	}
}


void UGsCheatManager::AddItem(FString in_itemID , FString in_Count)
{
	UE_LOG(LogTemp, Log, TEXT("Call AddItem() !!! itemID : %s"), *in_itemID);
	UE_LOG(LogTemp, Log, TEXT("Call AddItem() !!! Count : %s"), *in_Count);

	int64 _itemID = FCString::Atoi(*in_itemID);
	int32 _count = FCString::Atoi(*in_Count);
	_count = _count > 999999 ? 999999 : _count;
	FGsItemTables* _tableData = GItemManager()->GetFindTableData(_itemID);
	if (nullptr != _tableData)
	{
		GItemManager()->AddItem(_tableData->ItemID , _tableData->ItemType, _count);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Call AddItem() - Not Exist ItemID : %d"), _itemID);
	}
}