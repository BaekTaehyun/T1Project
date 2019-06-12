// Fill out your copyright notice in the Description page of Project Settings.

#include "GsGameObjectProjectile.h"
#include "GameObject/ActorExtend/GsProjectileActor.h"
#include "Runtime/Engine/Classes/GameFramework/ProjectileMovementComponent.h"

EGsGameObjectType	UGsGameObjectProjectile::GetObjectType() const  { return EGsGameObjectType::Projectile; }
AActor*				UGsGameObjectProjectile::GetActor() const       { return (Actor->IsValidLowLevel()) ? Actor : NULL; }

UGsGameObjectProjectile::~UGsGameObjectProjectile()
{
}

void UGsGameObjectProjectile::Initialize()
{
	Super::Initialize();

    SET_FLAG_TYPE(ObjectType, UGsGameObjectProjectile::GetObjectType());
}

void UGsGameObjectProjectile::Finalize()
{
	Super::Finalize();
}

void UGsGameObjectProjectile::ActorSpawned(AActor* Spawn)
{
	Super::ActorSpawned(Spawn);

	if (Spawn)
	{
		//액터 저장
		Actor = Cast<AGsProjectileActor>(Spawn);

		//방향 설정
		//필요 하려나??
		UProjectileMovementComponent* projectileComponent = Actor->FindComponentByClass<UProjectileMovementComponent>();
		if (projectileComponent)
		{
			projectileComponent->Velocity = Actor->GetActorForwardVector() * projectileComponent->InitialSpeed;
		}
	}
}