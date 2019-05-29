// Fill out your copyright notice in the Description page of Project Settings.


#include "GsGameObjectBlueprintLibrary.h"
#include "GameObject/GsGameObjectManager.h"

void UGsGameObjectBlueprintLibrary::SpawnPlayer(TSubclassOf<ACharacter> ActorClass, const FVector& StartPos, const FVector& Offset)
{
	GGameObj()->SpawnPlayer(ActorClass.Get(), StartPos + Offset, FRotator(0.f, 0.f, 0.f));
}

AActor* UGsGameObjectBlueprintLibrary::SpawnOnGround(TSubclassOf<AActor> ActorClass, const FVector& StartPos, const FVector& Offset)
{
	return Cast<AActor>(GGameObj()->SpawnNpc(ActorClass.Get(), StartPos, FRotator(0.f, 0.f, 0.f)));
}

AActor* UGsGameObjectBlueprintLibrary::SpawnVehicle(TSubclassOf<AActor> ActorClass, const FVector& StartPos, const FVector& Offset)
{
	return Cast<AActor>(GGameObj()->SpawnVehicle(ActorClass.Get(), StartPos, FRotator(0.f, 0.f, 0.f)));
}

UGsGameObjectBase* UGsGameObjectBlueprintLibrary::FindGameObject(UObject* WorldContextObject, EGsGameObjectType ObjectType)
{
	return GGameObj()->FindObject(ObjectType);
}

TArray<UGsGameObjectBase*> UGsGameObjectBlueprintLibrary::FindGameObjects(UObject* WorldContextObject, EGsGameObjectType ObjectType)
{
    return GGameObj()->FindObjects(ObjectType);
}
