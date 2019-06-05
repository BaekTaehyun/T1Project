// Fill out your copyright notice in the Description page of Project Settings.

#include "GsStateNPC.h"
#include "GsFSMManager.h"

template <class tstate>
void FGsStateSingleNpc<tstate>::Enter(UGsGameObjectBase* Owner)
{
	auto npc = Cast<UGsGameObjectNonPlayer>(Owner);
	if (auto anim = npc->GetNpcPawn()->GetAnim())
	{
		anim->ChangeState(GetStateID(), 0, GetAniRandomCount());
	}
}

uint8 FGsStateNpcSpawn::GetStateID()
{
    return static_cast<uint8>(EGsStateBase::Spawn);
}

FString FGsStateNpcSpawn::Name()
{
    return TEXT("StateNpcSpawn");
}

bool FGsStateNpcSpawn::OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	return true;
}

void FGsStateNpcSpawn::Enter(UGsGameObjectBase* Owner)
{
	Super::Enter(Owner);

	auto npc = Cast<UGsGameObjectNonPlayer>(Owner);
    ChangeDelayState<FGsStateNpcIdle>(npc->GetBaseFSM(), 1.5f);
}


uint8 FGsStateNpcIdle::GetStateID()
{
	return static_cast<uint8>(EGsStateBase::Idle);
}

FString FGsStateNpcIdle::Name()
{
	return TEXT("StateNpcIdle");
}

bool FGsStateNpcIdle::OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	auto npc = Cast<UGsGameObjectNonPlayer>(Owner);

	switch (StateID)
	{
	case EGsStateBase::ForwardWalk:
		ChangeState<FGsStateNpcWalk>(npc->GetBaseFSM());
		break;
	case EGsStateBase::Beaten:
		ChangeState<FGsStateNpcBeaten>(npc->GetBaseFSM());
		break;
	default:
		return false;
	}

	return true;
}

uint8 FGsStateNpcWalk::GetStateID()
{
    return static_cast<uint8>(EGsStateBase::ForwardWalk);
}

FString FGsStateNpcWalk::Name()
{
    return TEXT("StateNpcWalk");
}

bool FGsStateNpcWalk::OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	auto npc = Cast<UGsGameObjectNonPlayer>(Owner);
	switch (StateID)
	{
	case EGsStateBase::Idle:
		ChangeState<FGsStateNpcIdle>(npc->GetBaseFSM());
		break;
	case EGsStateBase::Beaten:
		ChangeState<FGsStateNpcBeaten>(npc->GetBaseFSM());
		break;
	default:
		return false;
	}

	return true;
}

uint8 FGsStateNpcBeaten::GetStateID()
{
	return static_cast<uint8>(EGsStateBase::Beaten);
}

FString FGsStateNpcBeaten::Name()
{
	return TEXT("StateNpcBeaten");
}

bool FGsStateNpcBeaten::OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	auto npc = Cast<UGsGameObjectNonPlayer>(Owner);
	switch (StateID)
	{
	case EGsStateBase::Idle:
		ChangeState<FGsStateNpcIdle>(npc->GetBaseFSM());
		break;
	case EGsStateBase::ForwardWalk:
		ChangeState<FGsStateNpcWalk>(npc->GetBaseFSM());
		break;
	default:
		return false;
	}

	return true;
}

void FGsStateNpcBeaten::Enter(UGsGameObjectBase* Owner)
{
	Super::Enter(Owner);

	//임의로 상태전환 시간 설정
	auto npc = Cast<UGsGameObjectNonPlayer>(Owner);
	ChangeDelayPrevState(npc->GetBaseFSM(), 2.f);
}
