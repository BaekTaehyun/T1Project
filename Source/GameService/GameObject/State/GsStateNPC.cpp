// Fill out your copyright notice in the Description page of Project Settings.

#include "GsStateNPC.h"
#include "GsFSMManager.h"

int FGsStateNpcSpawn::GetStateID()
{
    return (int)EGsStateBase::Spawn;
}

FString FGsStateNpcSpawn::Name()
{
    return TEXT("StateNpcSpawn");
}

void FGsStateNpcSpawn::OnEnter(UGsGameObjectNonPlayer* Owner)
{
	Super::OnEnter(Owner);

    Owner->GetBaseFSM()->ChangeDelayState<FGsStateNpcIdle>(1.5f);
}


int FGsStateNpcIdle::GetStateID()
{
	return (int)EGsStateBase::Idle;
}

FString FGsStateNpcIdle::Name()
{
	return TEXT("StateNpcIdle");
}

void FGsStateNpcIdle::OnEnter(UGsGameObjectNonPlayer* Owner)
{
	Super::OnEnter(Owner);
}

int FGsStateNpcWalk::GetStateID()
{
    return (int)EGsStateBase::ForwardWalk;
}

FString FGsStateNpcWalk::Name()
{
    return TEXT("StateNpcWalk");
}

void FGsStateNpcWalk::OnEnter(UGsGameObjectNonPlayer* Owner)
{
    Super::OnEnter(Owner);
}


int FGsStateNpcBeaten::GetStateID()
{
	return (int)EGsStateBase::Beaten;
}

FString FGsStateNpcBeaten::Name()
{
	return TEXT("StateNpcBeaten");
}

void FGsStateNpcBeaten::OnEnter(UGsGameObjectNonPlayer* Owner)
{
	Super::OnEnter(Owner);

	//임의로 상태전환 시간 설정
	Owner->GetBaseFSM()->ChangeDelayPrevState(2.f);
}
