#pragma once

#include "CoreMinimal.h"

namespace TGsSpawn
{
	// ActorClass::StaticClass()�� �̿��� ����
	// Ÿ���� �������϶� ���
	template<typename ActorClass>
	ActorClass* StaticClass(UWorld* World, const FVector& SpawnLocation, const FRotator& SpawnRotation)
	{
		if (NULL == World) return NULL;
		return World->SpawnActor<ActorClass>(ActorClass::::StaticClass(), Pos, Rot);
	}
	/*
	static ConstructorHelpers::FClassFinder<UAnimInstance> WARR_ANIM(	TEXT("/Game/InfinityBladeWarriors/Character/WarriorAnimBP.WarriorAnimBP_C"));
	//ConstructorHelpers::FClassFinder  *���� �������Ʈ ���� _C�� �ٿ��� �˻��� (Ŭ���� �˻�) : WarriorAnimBP_C
	if (WARR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARR_ANIM.Class);
	}
	������ �帮�� ���� ��ġ.
	*/
	AActor* BPClass(UWorld* World, UClass* Class, const FVector& SpawnLocation, const FRotator& SpawnRotation)
	{
		if (NULL == World) return NULL;
		if (NULL == Class) return NULL;

		return World->SpawnActor(Class, &SpawnLocation, &SpawnRotation);
	}

	// ������ü�� ������� ��ġ �Ҷ� ����Ѵ�.
	template<typename ActorClass>
	ActorClass* Clone(ActorClass* ExistingActor, const FVector& SpawnLocation, const FRotator& SpawnRotation)
	{
		UWorld* World = ExistingActor->GetWorld();
		if (NULL == World) return NULL;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Template = ExistingActor;
		return World->SpawnActor<ActorClass>(ExistingActor->GetClass(), SpawnLocation, SpawnRotation, SpawnParams);
	}

	// ���� ���� ��ġ��
	template <typename ActorClass>
	static FORCEINLINE ActorClass* Detail(UWorld* World, UClass* Class,
		const FVector& SpawnLocation, const FRotator& SpawnRotation, const bool bNoCollisionFail = true,
		AActor* Owner = NULL, APawn* Instigator = NULL)
	{
		if (NULL == World) return NULL;
		if (NULL == Class) return NULL;

		FActorSpawnParameters SpawnParams;
		SpawnParams.bNoCollisionFail = bNoCollisionFail;
		SpawnParams.Owner = Owner;
		SpawnParams.Instigator = Instigator;
		SpawnParams.bDeferConstruction = false;
		return World->SpawnActor<ActorClass>(Class, SpawnLocation, SpawnRotation, SpawnParams);
	}
		
}
