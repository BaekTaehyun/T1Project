#pragma once

#include "CoreMinimal.h"

namespace TGsSpawn
{
	// ActorClass::StaticClass()를 이용한 스폰
	// 타입이 고정적일때 사용
	template<typename ActorClass>
	ActorClass* StaticClass(UWorld* World, const FVector& SpawnLocation, const FRotator& SpawnRotation)
	{
		if (NULL == World) return NULL;
		return World->SpawnActor<ActorClass>(ActorClass::::StaticClass(), Pos, Rot);
	}
	/*
	static ConstructorHelpers::FClassFinder<UAnimInstance> WARR_ANIM(	TEXT("/Game/InfinityBladeWarriors/Character/WarriorAnimBP.WarriorAnimBP_C"));
	//ConstructorHelpers::FClassFinder  *주위 블루프린트 끝에 _C를 붙여서 검색함 (클래스 검색) : WarriorAnimBP_C
	if (WARR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARR_ANIM.Class);
	}
	데이터 드리븐 형식 배치.
	*/
	AActor* BPClass(UWorld* World, UClass* Class, const FVector& SpawnLocation, const FRotator& SpawnRotation)
	{
		if (NULL == World) return NULL;
		if (NULL == Class) return NULL;

		return World->SpawnActor(Class, &SpawnLocation, &SpawnRotation);
	}

	// 기존객체를 복사생성 배치 할때 사용한다.
	template<typename ActorClass>
	ActorClass* Clone(ActorClass* ExistingActor, const FVector& SpawnLocation, const FRotator& SpawnRotation)
	{
		UWorld* World = ExistingActor->GetWorld();
		if (NULL == World) return NULL;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Template = ExistingActor;
		return World->SpawnActor<ActorClass>(ExistingActor->GetClass(), SpawnLocation, SpawnRotation, SpawnParams);
	}

	// 세부 설정 배치용
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
