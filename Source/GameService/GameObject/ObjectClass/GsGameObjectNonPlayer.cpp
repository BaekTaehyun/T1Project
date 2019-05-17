// Fill out your copyright notice in the Description page of Project Settings.

#include "GsGameObjectNonPlayer.h"
#include "GameObject/ActorExtend/GsNpcPawn.h"
#include "GameObject/ObjectClass/GsGameObjectNonPlayer.h"
#include "GameObject/Movement/GsMovementBase.h"
#include "GameObject/Movement/GsMovementNpc.h"
#include "GameObject/State/GsFSMManager.h"
#include "GameObject/State/GsStateNPC.h"

EGsGameObjectType	UGsGameObjectNonPlayer::GetObjectType() const   { return EGsGameObjectType::NonPlayer; }
AActor*				UGsGameObjectNonPlayer::GetActor() const        { return GetNpc(); }
AGsNpcPawn*			UGsGameObjectNonPlayer::GetNpc() const			{ return Actor; }

void UGsGameObjectNonPlayer::Initialize()
{
	Super::Initialize();

    SET_FLAG_TYPE(ObjectType, UGsGameObjectNonPlayer::GetObjectType());

	Fsm = new FGsFSMManager();
	Fsm->Initialize(this);
}

void UGsGameObjectNonPlayer::Finalize()
{
	Super::Finalize();
}

void UGsGameObjectNonPlayer::OnHit(UGsGameObjectBase* Target)
{
	Super::OnHit(Target);

	Fsm->ChangeState<FGsStateNpcBeaten>();
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

        //기본 상태 설정
        Fsm->ChangeState<FGsStateNpcSpawn>();
	}
}
