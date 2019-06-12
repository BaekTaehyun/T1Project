﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "GsMovementNpc.h"
#include "AIModule/Classes/AIController.h"
#include "GameObject/ObjectClass/GsGameObjectNonPlayer.h"
#include "GameObject/ActorExtend/GsNpcPawn.h"
#include "GameObject/State/GsFSMManager.h"
#include "GameObject/State/GsStateNPC.h"

FGsMovementNpc::FGsMovementNpc()
{
}

FGsMovementNpc::~FGsMovementNpc()
{
}

void FGsMovementNpc::Initialize(UGsGameObjectBase* owner)
{
    Super::Initialize(owner);

    Npc = Cast<UGsGameObjectNonPlayer>(owner);
    if (Npc)
    {
		auto pawn = Npc->GetNpcPawn();
        PawnMovement = pawn->GetPawnMovement();
        auto controller = pawn->GetController();
        if (auto Ai = Cast< AAIController>(controller))
        {
            //이동 동기화용 델리게이트 연결
            PathComponent = Ai->FindComponentByClass<UPathFollowingComponent>();
            if (PathComponent)
            {
                PathComponent->PostProcessMove.BindRaw(this, &FGsMovementNpc::CallbackMove);
                PathComponent->OnRequestFinished.AddRaw(this, &FGsMovementNpc::CallbackFinish);
            }
        }
    }
}

void FGsMovementNpc::Finalize()
{
	Super::Finalize();
}

void FGsMovementNpc::Update(float Delta)
{
	Super::Update(Delta);

    //상태 동기화
    if (PathComponent)
    {
        EPathFollowingStatus::Type status = PathComponent->GetStatus();
        if (MoveStatus != status)
        {
            MoveStatus = status;

            UpdateState(MoveStatus);
        }
    }
}

void FGsMovementNpc::UpdateState(EPathFollowingStatus::Type Type)
{
    //상태 전환
    auto fsm = Npc->GetBaseFSM();
    switch (Type)
    {
    case EPathFollowingStatus::Idle:
		fsm->ProcessEvent(EGsStateBase::Idle);
        break;
    case EPathFollowingStatus::Waiting:
        break;
    case EPathFollowingStatus::Paused:
        break;
    case EPathFollowingStatus::Moving:
		fsm->ProcessEvent(EGsStateBase::ForwardWalk);
        break;
    }
}

//패스 컴퍼넌트 콜백 연결
void FGsMovementNpc::CallbackMove(UPathFollowingComponent* Path, FVector& Pos)
{
}

void FGsMovementNpc::CallbackFinish(FAIRequestID ID, const FPathFollowingResult& Result)
{
}
