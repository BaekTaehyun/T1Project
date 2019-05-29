// Fill out your copyright notice in the Description page of Project Settings.

#include "GsStateLocal.h"
#include "GsFSMManager.h"
#include "GameObject/Skill/GsSKillLocal.h"
#include "GameObject/Movement/GsMovementBase.h"
#include "GameObject/Component/Animation/GsAnimInstanceState.h"


/// FStateSpawn ///
int FGsStateSpawn::GetStateID()
{
	return (int)EGsStateBase::Spawn;
}

FString FGsStateSpawn::Name()
{
	return TEXT("StateSpawn");
}

int FGsStateSpawn::GetAniRandomCount()
{
	return 2;
}

bool FGsStateSpawn::OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateBase::Idle:
		ChangeState<FGsStateIdle>(Owner->GetBaseFSM());
		break;
	default:
		return false;
	}

	return true;
}

void FGsStateSpawn::OnEnter(UGsGameObjectLocal* Owner)
{
	Super::OnEnter(Owner);

	//임시 처리
	ChangeDelayState<FGsStateIdle>(Owner->GetBaseFSM(), 1.5f);
}


/// FStateIdle ///
int FGsStateIdle::GetStateID()
{
	return (int)EGsStateBase::Idle;
}

FString FGsStateIdle::Name()
{
	return TEXT("StateIdle");
}

bool FGsStateIdle::OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateBase::ForwardWalk:
		ObjectBaseStateChange(FGsStateForwardWalk);
		break;
	case EGsStateBase::BackwardWalk:
		ObjectBaseStateChange(FGsStateBackwardWalk);
		break;
	case EGsStateBase::SideWalk:
		ObjectBaseStateChange(FGsStateSideWalk);
		break;
	case EGsStateBase::Run:
		ObjectBaseStateChange(FGsStateRun);
		break;
	case EGsStateBase::Ride:
		ObjectBaseStateChange(FGsStateRide);
		break;
	default:
		return false;
	}

	return true;
}

void FGsStateIdle::OnEnter(UGsGameObjectLocal* Owner)
{
	Super::OnEnter(Owner);
}


/// FStateForwardWalk ///
int FGsStateForwardWalk::GetStateID()
{
	return (int)EGsStateBase::ForwardWalk;
}

FString FGsStateForwardWalk::Name()
{
	return TEXT("StateForwardWalk");
}

bool FGsStateForwardWalk::OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateBase::Idle:
		ObjectBaseStateChange(FGsStateIdle);
		break;
	case EGsStateBase::BackwardWalk:
		ObjectBaseStateChange(FGsStateBackwardWalk);
		break;
	case EGsStateBase::SideWalk:
		ObjectBaseStateChange(FGsStateSideWalk);
		break;
	case EGsStateBase::Run:
		ObjectBaseStateChange(FGsStateRun);
		break;
	default:
		return false;
	}

	return true;
}

void FGsStateForwardWalk::OnEnter(UGsGameObjectLocal* Owner)
{
	Super::OnEnter(Owner);
}

void FGsStateForwardWalk::OnUpdate(UGsGameObjectLocal* Owner, float Delta)
{
}


/// FStateBackwardWalk ///
int FGsStateBackwardWalk::GetStateID()
{
	return (int)EGsStateBase::BackwardWalk;
}

FString FGsStateBackwardWalk::Name()
{
	return TEXT("StateBackwardWalk");
}

bool FGsStateBackwardWalk::OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateBase::Idle:
		ObjectBaseStateChange(FGsStateIdle);
		break;
	case EGsStateBase::ForwardWalk:
		ObjectBaseStateChange(FGsStateForwardWalk);
		break;
	case EGsStateBase::SideWalk:
		ObjectBaseStateChange(FGsStateSideWalk);
		break;
	case EGsStateBase::Run:
		ObjectBaseStateChange(FGsStateRun);
		break;
	default:
		return false;
	}

	return true;
}

void FGsStateBackwardWalk::OnEnter(UGsGameObjectLocal* Owner)
{
	Super::OnEnter(Owner);
}

void FGsStateBackwardWalk::OnUpdate(UGsGameObjectLocal* Owner, float Delta)
{
}


/// FStateSideWalk ///
int FGsStateSideWalk::GetStateID()
{
	return (int)EGsStateBase::SideWalk;
}

FString FGsStateSideWalk::Name()
{
	return TEXT("StateSideWalk");
}

bool FGsStateSideWalk::OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateBase::Idle:
		ObjectBaseStateChange(FGsStateIdle);
		break;
	case EGsStateBase::ForwardWalk:
		ObjectBaseStateChange(FGsStateForwardWalk);
		break;
	case EGsStateBase::BackwardWalk:
		ObjectBaseStateChange(FGsStateBackwardWalk);
		break;
	default:
		return false;
	}

	return true;
}

void FGsStateSideWalk::OnEnter(UGsGameObjectLocal* Owner)
{
	Super::OnEnter(Owner);
}

void FGsStateSideWalk::OnUpdate(UGsGameObjectLocal* Owner, float Delta)
{
}

/// FStateRun ///
int FGsStateRun::GetStateID()
{
	return (int)EGsStateBase::Run;
}

FString FGsStateRun::Name()
{
	return TEXT("StateRun");
}

bool FGsStateRun::OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateBase::Idle:
		ObjectBaseStateChange(FGsStateIdle);
		break;
	case EGsStateBase::ForwardWalk:
		ObjectBaseStateChange(FGsStateForwardWalk);
		break;
	default:
		break;
	}

	return true;
}

void FGsStateRun::OnEnter(UGsGameObjectLocal* Owner)
{
	Super::OnEnter(Owner);
}

void FGsStateRun::OnUpdate(UGsGameObjectLocal* Owner, float Delta)
{
}


///FStateUpperRide///
int FGsStateRide::GetStateID()
{
	return (int)EGsStateBase::Ride;
}

FString FGsStateRide::Name()
{
	return TEXT("StateRide");
}

bool FGsStateRide::OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateBase::Idle:
		ObjectBaseStateChange(FGsStateIdle);
		break;
	}

	return true;
}

void FGsStateRide::OnEnter(UGsGameObjectLocal* Owner)
{
	//피직 정보 끄기
	Owner->GetLocalCharacter()->DisableComponentsSimulatePhysics();
}

void FGsStateRide::OnUpdate(UGsGameObjectLocal* Owner, float Delta)
{

}

///FStateUpperIdle///
int FGsStateUpperIdle::GetStateID()
{
	return (int)EGsStateUpperBase::Idle;
}

FString FGsStateUpperIdle::Name()
{
	return TEXT("StateUpperIdle");
}

bool FGsStateUpperIdle::OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateUpperBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateUpperBase::Attack:
		ObjectUpperStateChange(FGsStateAttack);
		break;
	default:
		return false;
	}
	
	return true;
}

void FGsStateUpperIdle::OnEnter(UGsGameObjectLocal* Owner)
{
	//상체 애니 재생 정지
	auto skillMgr = Owner->GetSkill();
	if (skillMgr->CurrentSkillData)
	{
		auto anim = Owner->GetLocalCharacter()->GetAnim();
		anim->StopUpperAni(skillMgr->CurrentSkillData->GetAni());
	}
}

/// FStateAttack ///
int FGsStateAttack::GetStateID()
{
	return (int)EGsStateUpperBase::Attack;
}

FString FGsStateAttack::Name()
{
	return TEXT("StateAttack");
}

bool FGsStateAttack::OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateUpperBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateUpperBase::Idle:
		ObjectUpperStateChange(FGsStateUpperIdle);
		break;
	default:
		return false;
	}

	return true;
}

void FGsStateAttack::OnEnter(UGsGameObjectLocal* Owner)
{
	Super::OnEnter(Owner);

	auto skillMgr = Owner->GetSkill();
	if (skillMgr->CurrentSkillData)
	{
		auto anim = Owner->GetLocalCharacter()->GetAnim();
		anim->PlayUpperAni(skillMgr->CurrentSkillData->GetAni());
		skillMgr->OnSKillNode();
	}
}

void FGsStateAttack::OnUpdate(UGsGameObjectLocal* Owner, float Delta)
{
	auto skillMgr = Owner->GetSkill();
	skillMgr->RunSkillNode(Delta);
}