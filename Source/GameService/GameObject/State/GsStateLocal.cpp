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

void FGsStateSpawn::OnEnter(UGsGameObjectLocal* Owner)
{
	Super::OnEnter(Owner);

	Owner->GetBaseFSM()->ChangeDelayState<FGsStateIdle>(1.5f);
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

bool FGsStateRun::IsChange(int StateID)
{
	switch (StateID)
	{
	case (int)EGsStateBase::ForwardWalk:
		return true;
	}
	return false;
}

void FGsStateRun::OnEnter(UGsGameObjectLocal* Owner)
{
	Super::OnEnter(Owner);
}

void FGsStateRun::OnUpdate(UGsGameObjectLocal* Owner, float Delta)
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

void FGsStateUpperIdle::OnEnter(UGsGameObjectLocal* Owner)
{
	//상체 애니 재생 정지
	auto skillMgr = Owner->GetSkill();
	if (skillMgr->CurrentSkillData)
	{
		auto anim = Owner->GetLocal()->GetAnim();
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

void FGsStateAttack::OnEnter(UGsGameObjectLocal* Owner)
{
	Super::OnEnter(Owner);

	auto skillMgr = Owner->GetSkill();
	if (skillMgr->CurrentSkillData)
	{
		auto anim = Owner->GetLocal()->GetAnim();
		anim->PlayUpperAni(skillMgr->CurrentSkillData->GetAni());
		skillMgr->OnSKillNode();
	}
}

void FGsStateAttack::OnUpdate(UGsGameObjectLocal* Owner, float Delta)
{
	auto skillMgr = Owner->GetSkill();
	skillMgr->RunSkillNode(Delta);
}