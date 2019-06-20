// Fill out your copyright notice in the Description page of Project Settings.

#include "GsSKillActionNodeProjectile.h"
#include "Engine/World.h"
#include "GSGameInstance.h"
#include "GameObject/ObjectClass/GsGameObjectProjectile.h"
#include "GameObject/GsSpawnComponent.h"

FGsSKillActionNodeProjectile::FGsSKillActionNodeProjectile(const FGsSkillActionDataBase& Data) :
	Super(Data)
{
}

void FGsSKillActionNodeProjectile::Process(UGsGameObjectBase* Owner)
{
}

void FGsSKillActionNodeProjectile::Action(UGsGameObjectBase* Owner)
{
	//임시 데이터 리소스 하드코딩
	FString path = TEXT("Blueprint'/Game/Blueprint/GameObject/BP_ProjectileActor.BP_ProjectileActor'");
	if (auto loadObject = StaticLoadObject(UObject::StaticClass(), nullptr, *path))
	{
	auto actor = Owner->GetActor();
	//현재는 임시로 캐릭터쪽 방향과 포지션을 적용
	FVector Pos = actor->GetActorLocation() + actor->GetActorForwardVector() * 100.f;
	FRotator Rot = actor->GetActorRotation();
		//블루 프린트로 캐스팅 필요 
		//이걸 하지않으면 AActor클래스 정보를 스폰시에 찾을수 없는듯 하다.
		if (UBlueprint* castBP = Cast<UBlueprint>(loadObject))
		{
			GSpawner()->SpawnObject<UGsGameObjectProjectile>(castBP->GeneratedClass, Pos, Rot);
		}
	}

	// (AActor) 타입이 다를수 있으니 확인해서 이런식으로 바꾸길 바람
	/*
	static ConstructorHelpers::FClassFinder<AActor> PROJ_C(TEXT("/Game/Blueprint/GameObject/BP_ProjectileActor.BP_ProjectileActor_C"));
	if (PROJ_C.Succeeded())
	{
		auto actor = Owner->GetActor();
		//현재는 임시로 캐릭터쪽 방향과 포지션을 적용
		FVector Pos = actor->GetActorLocation() + actor->GetActorForwardVector() * 100.f;
		FRotator Rot = actor->GetActorRotation();
		GSpawner()->SpawnObject<UGsGameObjectProjectile>(PROJ_C.Class, Pos, Rot);
	}*/
}