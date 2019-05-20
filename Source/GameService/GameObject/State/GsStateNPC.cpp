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

bool FGsStateNpcSpawn::OnProcessEvent(UGsGameObjectNonPlayer* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	return true;
}

void FGsStateNpcSpawn::OnEnter(UGsGameObjectNonPlayer* Owner)
{
	Super::OnEnter(Owner);

    ChangeDelayState<FGsStateNpcIdle>(Owner->GetBaseFSM(), 1.5f);
}


int FGsStateNpcIdle::GetStateID()
{
	return (int)EGsStateBase::Idle;
}

FString FGsStateNpcIdle::Name()
{
	return TEXT("StateNpcIdle");
}

bool FGsStateNpcIdle::OnProcessEvent(UGsGameObjectNonPlayer* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateBase::ForwardWalk:
		ObjectBaseStateChange(FGsStateNpcWalk);
		break;
	case EGsStateBase::Beaten:
		ObjectBaseStateChange(FGsStateNpcBeaten);
		break;
	default:
		return false;
	}

	return true;
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

bool FGsStateNpcWalk::OnProcessEvent(UGsGameObjectNonPlayer* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateBase::Idle:
		ObjectBaseStateChange(FGsStateNpcIdle);
		break;
	case EGsStateBase::Beaten:
		ObjectBaseStateChange(FGsStateNpcBeaten);
		break;
	default:
		return false;
	}

	return true;
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

bool FGsStateNpcBeaten::OnProcessEvent(UGsGameObjectNonPlayer* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateBase::Idle:
		ObjectBaseStateChange(FGsStateNpcIdle);
		break;
	case EGsStateBase::ForwardWalk:
		ObjectBaseStateChange(FGsStateNpcWalk);
		break;
	default:
		return false;
	}

	return true;
}

void FGsStateNpcBeaten::OnEnter(UGsGameObjectNonPlayer* Owner)
{
	Super::OnEnter(Owner);

	//임의로 상태전환 시간 설정
	ChangeDelayPrevState(Owner->GetBaseFSM(), 2.f);
}
