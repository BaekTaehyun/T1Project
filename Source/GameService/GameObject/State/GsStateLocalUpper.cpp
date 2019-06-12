// Fill out your copyright notice in the Description page of Project Settings.


#include "GsStateLocalUpper.h"
#include "GameObject/ActorExtend/GsLocalCharacter.h"
#include "GameObject/ObjectClass/GsGameObjectLocal.h"
#include "GameObject/Skill/GsSKillLocal.h"

//----------------------------------------------------------------------------------------
// FStateUpperIdle
//----------------------------------------------------------------------------------------
uint8 FGsStateLocalUpperIdle::GetStateID()
{
	return static_cast<uint8>(EGsStateBase::Idle);
}

FString FGsStateLocalUpperIdle::Name()
{
	return TEXT("StateUpperIdle");
}

bool FGsStateLocalUpperIdle::ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID)
{
	if (false == Super::ProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (static_cast<EGsStateBase>(StateID))
	{
	case EGsStateBase::Attack:
		ObjectUpperStateChange(FGsStateLocalAttack);
		break;
	default:
		return false;
	}

	return true;
}

void FGsStateLocalUpperIdle::Enter(UGsGameObjectBase* Owner)
{
	Super::Enter(Owner);

	auto my = Cast<UGsGameObjectLocal>(Owner);
	//상체 애니 재생 정지
	auto skillMgr = my->GetSkill();
	if (skillMgr->CurrentSkillData)
	{
		auto anim = my->GetLocalCharacter()->GetAnim();
		anim->StopUpperAni(skillMgr->CurrentSkillData->GetAni());
	}
}

//----------------------------------------------------------------------------------------
// FStateAttack
//----------------------------------------------------------------------------------------
uint8 FGsStateLocalAttack::GetStateID()
{
	return static_cast<uint8>(EGsStateBase::Attack);
}

FString FGsStateLocalAttack::Name()
{
	return TEXT("StateAttack");
}

bool FGsStateLocalAttack::ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID)
{
	if (false == Super::ProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (static_cast<EGsStateBase>(StateID))
	{
	case EGsStateBase::Idle:
		ObjectUpperStateChange(FGsStateLocalUpperIdle);
		break;
	default:
		return false;
	}

	return true;
}

void FGsStateLocalAttack::Enter(UGsGameObjectBase* Owner)
{
	Super::Enter(Owner);

	auto my = Cast<UGsGameObjectLocal>(Owner);
	auto skillMgr = my->GetSkill();
	if (skillMgr->CurrentSkillData)
	{
		auto anim = my->GetLocalCharacter()->GetAnim();
		anim->PlayUpperAni(skillMgr->CurrentSkillData->GetAni());
		skillMgr->OnSKillNode();
	}
}

void FGsStateLocalAttack::Update(UGsGameObjectBase* Owner, float Delta)
{
	Super::Update(Owner, Delta);

	auto my = Cast<UGsGameObjectLocal>(Owner);
	auto skillMgr = my->GetSkill();
	skillMgr->RunSkillNode(Delta);
}
