// Fill out your copyright notice in the Description page of Project Settings.


#include "GsGameObjectBlueprintLibrary.h"
#include "GameObject/GsObjectSpawner.h"

void UGsGameObjectBlueprintLibrary::SpawnPlayer(TSubclassOf<ACharacter> ActorClass, const FVector& StartPos, const FVector& Offset)
{
	GSpawner()->SpawnPlayer(ActorClass.Get(), StartPos + Offset, FRotator(0.f, 0.f, 0.f));
}

AActor* UGsGameObjectBlueprintLibrary::SpawnOnGround(TSubclassOf<AActor> ActorClass, const FVector& StartPos, const FVector& Offset)
{
	auto obj = GSpawner()->SpawnNpc(ActorClass.Get(), StartPos, FRotator(0.f, 0.f, 0.f));
	return  obj->GetActor();
}

UGsGameObjectBase* UGsGameObjectBlueprintLibrary::FindGameObject(UObject* WorldContextObject, EGsGameObjectType ObjectType)
{
	return GSpawner()->FindObject(ObjectType);
}

TArray<UGsGameObjectBase*> UGsGameObjectBlueprintLibrary::FindGameObjects(UObject* WorldContextObject, EGsGameObjectType ObjectType)
{
    return GSpawner()->FindObjects(ObjectType);
}
