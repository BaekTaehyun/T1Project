// Fill out your copyright notice in the Description page of Project Settings.


#include "GsMovementBase.h"

void FGsMovementBase::Initialize(UGsGameObjectBase* owner)
{
    Owner = owner;
}

void FGsMovementBase::DeInitialize()
{
}

void FGsMovementBase::Update(float Delta)
{
}

bool FGsMovementBase::IsStop()
{
    return MoveType == static_cast<uint8>(EGsGameObjectMoveType::None);
}

bool FGsMovementBase::IsMove()
{
    uint8 moveflag;
    SET_FLAG_TYPE(moveflag, EGsGameObjectMoveType::Run);
    SET_FLAG_TYPE(moveflag, EGsGameObjectMoveType::Walk);
    SET_FLAG_TYPE(moveflag, EGsGameObjectMoveType::Interpolation);
    return CHECK_FLAG_TYPE(MoveType, moveflag);
}

void FGsMovementBase::SetMoveType(EGsGameObjectMoveType Type)
{
    switch (Type)
    {
    case EGsGameObjectMoveType::None:
        MoveSpeed = 0.f;
        CLEAR_FLAG_TYPE(MoveType);
        return;                             //정지 상태 
    case EGsGameObjectMoveType::Walk:
        REMOVE_FLAG_TYPE(MoveType, EGsGameObjectMoveType::Run);
        break;
    case EGsGameObjectMoveType::Run:
        REMOVE_FLAG_TYPE(MoveType, EGsGameObjectMoveType::Walk);
        break;
    case EGsGameObjectMoveType::Interpolation:
        break;
    case EGsGameObjectMoveType::Jump:
        break;
    }

    SET_FLAG_TYPE(MoveType, Type);
}

void FGsMovementBase::Stop()
{
    CLEAR_FLAG_TYPE(MoveType);
    OnStop();
}

void FGsMovementBase::Move()
{
    OnMove();
}

void FGsMovementBase::Move(EGsGameObjectMoveDirType Type)
{
    SetMoveDirType(Type);
    Move();
}

void FGsMovementBase::Move(FVector Dir)
{
    SetDirection(Dir);
    Move();
}

void FGsMovementBase::Move(FVector Dir, EGsGameObjectMoveDirType Type)
{
    SetDirection(Dir);
    SetMoveDirType(Type);
    Move();
}

void FGsMovementBase::Move(FVector Dir, EGsGameObjectMoveDirType Type, float Speed)
{
    SetDirection(Dir);
    SetMoveDirType(Type);
    SetMoveSpeed(Speed);
    Move();
}

void FGsMovementBase::OnStop()
{
}

void FGsMovementBase::OnMove()
{
}