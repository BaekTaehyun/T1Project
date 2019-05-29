// Fill out your copyright notice in the Description page of Project Settings.


#include "GsGameObjectBlueprintLibrary.h"
#include "GameObject/GsSpawnComponent.h"
#include "GameObject/ObjectClass/GsGameObjectLocal.h"
#include "GameObject/ObjectClass/GsGameObjectNonPlayer.h"

void UGsGameObjectBlueprintLibrary::SpawnPlayer(TSubclassOf<ACharacter> ActorClass, const FVector& StartPos, const FVector& Offset)
{
	GGameObj()->SpawnObject<UGsGameObjectLocal>(ActorClass.Get(), StartPos + Offset, FRotator(0.f, 0.f, 0.f));
}

AActor* UGsGameObjectBlueprintLibrary::SpawnOnGround(TSubclassOf<AActor> ActorClass, const FVector& StartPos, const FVector& Offset)
{
	return Cast<AActor>(GGameObj()->SpawnObject<UGsGameObjectNonPlayer>(ActorClass.Get(), StartPos, FRotator(0.f, 0.f, 0.f)));
}

UGsGameObjectBase* UGsGameObjectBlueprintLibrary::FindGameObject(UObject* WorldContextObject, EGsGameObjectType ObjectType)
{
	return GGameObj()->FindObject(ObjectType);
}

TArray<UGsGameObjectBase*> UGsGameObjectBlueprintLibrary::FindGameObjects(UObject* WorldContextObject, EGsGameObjectType ObjectType)
{
    return GGameObj()->FindObjects(ObjectType);
}
