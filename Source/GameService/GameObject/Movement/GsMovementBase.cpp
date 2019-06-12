// Fill out your copyright notice in the Description page of Project Settings.


#include "GsMovementBase.h"

FGsMovementBase::FGsMovementBase()
{
}

FGsMovementBase::~FGsMovementBase()
{
}

void FGsMovementBase::Initialize(UGsGameObjectBase* owner)
{
    Owner = owner;
}

void FGsMovementBase::Finalize()
{
}

void FGsMovementBase::Update(float Delta)
{

}

bool FGsMovementBase::IsMove()
{
	return false;
}

void FGsMovementBase::Stop()
{
    OnStop();
}

void FGsMovementBase::Move()
{
    OnMove();
}

void FGsMovementBase::Move(FVector Dir)
{
    SetDirection(Dir);
    Move();
}

void FGsMovementBase::Move(FVector Dir, float Speed)
{
	SetDirection(Dir);
	SetMoveSpeed(Speed);
	Move();
}

void FGsMovementBase::OnStop()
{
}

void FGsMovementBase::OnMove()
{
}