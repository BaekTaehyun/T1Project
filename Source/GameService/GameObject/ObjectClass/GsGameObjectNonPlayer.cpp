// Fill out your copyright notice in the Description page of Project Settings.

#include "GsGameObjectNonPlayer.h"
#include "GameObject/ActorExtend/GsNpcPawn.h"
#include "GameObject/ObjectClass/GsGameObjectNonPlayer.h"
#include "GameObject/Movement/GsMovementBase.h"
#include "GameObject/Movement/GsMovementNpc.h"
#include "GameObject/State/GsFSMManager.h"
#include "GameObject/State/GsStateNPC.h"

EGsGameObjectType	UGsGameObjectNonPlayer::GetObjectType() const   { return EGsGameObjectType::NonPlayer; }
AActor*				UGsGameObjectNonPlayer::GetActor() const        { return GetNpcPawn(); }
AGsNpcPawn*			UGsGameObjectNonPlayer::GetNpcPawn() const		{ return (Actor->IsValidLowLevel()) ? Actor : NULL; }

UGsGameObjectNonPlayer::~UGsGameObjectNonPlayer()
{
}

void UGsGameObjectNonPlayer::Initialize()
{
	Super::Initialize();

    SET_FLAG_TYPE(ObjectType, UGsGameObjectNonPlayer::GetObjectType());	
}

void UGsGameObjectNonPlayer::Finalize()
{
	Super::Finalize();
}

void UGsGameObjectNonPlayer::OnHit(UGsGameObjectBase* Target)
{
	Super::OnHit(Target);

	Fsm->ProcessEvent(EGsStateBase::Beaten);
}

void UGsGameObjectNonPlayer::ActorSpawned(AActor* Spawn)
{
	Super::ActorSpawned(Spawn);

	if (Spawn)
	{
		//액터 저장
		Actor = Cast<AGsNpcPawn>(Spawn);

        Movement = new FGsMovementNpc();
        Movement->Initialize(this);

		Fsm = new FGsFSMManager();
		Fsm->Initialize<FGsStateNpcSpawn>(this);
	}
}
