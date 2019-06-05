// Fill out your copyright notice in the Description page of Project Settings.

#include "GsStateLocal.h"
#include "GsFSMManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameObject/Skill/GsSKillLocal.h"
#include "GameObject/Movement/GsMovementBase.h"
#include "GameObject/ObjectClass/GsGameObjectWheelVehicle.h"
#include "GameObject/ActorExtend/GsWheelVehicle.h"

template <class tstate, typename tStateType>
void FGsStateSingleLocal<tstate, tStateType>::Enter(UGsGameObjectBase* Owner)
{
	auto my = Cast<UGsGameObjectLocal>(Owner);
	if (auto anim = my->GetLocalCharacter()->GetAnim())
	{
		anim->ChangeState(GetStateID(), 0, GetAniRandomCount());
	}
}
/// FStateSpawn ///
uint8 FGsStateLocalSpawn::GetStateID()
{
	return static_cast<uint8>(EGsStateBase::Spawn);
}

FString FGsStateLocalSpawn::Name()
{
	return TEXT("StateSpawn");
}

int FGsStateLocalSpawn::GetAniRandomCount()
{
	return 2;
}

bool FGsStateLocalSpawn::OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateBase::Idle:
		ObjectBaseStateChange(FGsStateLocalIdle);
		break;
	default:
		return false;
	}

	return true;
}

void FGsStateLocalSpawn::Enter(UGsGameObjectBase* Owner)
{
	Super::Enter(Owner);

	//임시 처리
	auto my = Cast<UGsGameObjectLocal>(Owner);
	ChangeDelayState<FGsStateLocalIdle>(my->GetBaseFSM(), 1.5f);
}

/// FStateIdle ///
uint8 FGsStateLocalIdle::GetStateID()
{
	return static_cast<uint8>(EGsStateBase::Idle);
}

FString FGsStateLocalIdle::Name()
{
	return TEXT("StateIdle");
}

bool FGsStateLocalIdle::OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateBase::ForwardWalk:
		ObjectBaseStateChange(FGsStateLocalForwardWalk);
		break;
	case EGsStateBase::BackwardWalk:
		ObjectBaseStateChange(FGsStateLocalBackwardWalk);
		break;
	case EGsStateBase::SideWalk:
		ObjectBaseStateChange(FGsStateLocalSideWalk);
		break;
	case EGsStateBase::Run:
		ObjectBaseStateChange(FGsStateLocalRun);
		break;
	case EGsStateBase::Ride:
		ObjectBaseStateChange(FGsStateLocalRide);
		break;
	default:
		return false;
	}

	return true;
}

/// FStateForwardWalk ///
uint8 FGsStateLocalForwardWalk::GetStateID()
{
	return static_cast<uint8>(EGsStateBase::ForwardWalk);
}

FString FGsStateLocalForwardWalk::Name()
{
	return TEXT("StateForwardWalk");
}

bool FGsStateLocalForwardWalk::OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateBase::Idle:
		ObjectBaseStateChange(FGsStateLocalIdle);
		break;
	case EGsStateBase::BackwardWalk:
		ObjectBaseStateChange(FGsStateLocalBackwardWalk);
		break;
	case EGsStateBase::SideWalk:
		ObjectBaseStateChange(FGsStateLocalSideWalk);
		break;
	case EGsStateBase::Run:
		ObjectBaseStateChange(FGsStateLocalRun);
		break;
	default:
		return false;
	}

	return true;
}


/// FStateBackwardWalk ///
uint8 FGsStateLocalBackwardWalk::GetStateID()
{
	return static_cast<uint8>(EGsStateBase::BackwardWalk);
}

FString FGsStateLocalBackwardWalk::Name()
{
	return TEXT("StateBackwardWalk");
}

bool FGsStateLocalBackwardWalk::OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateBase::Idle:
		ObjectBaseStateChange(FGsStateLocalIdle);
		break;
	case EGsStateBase::ForwardWalk:
		ObjectBaseStateChange(FGsStateLocalForwardWalk);
		break;
	case EGsStateBase::SideWalk:
		ObjectBaseStateChange(FGsStateLocalSideWalk);
		break;
	case EGsStateBase::Run:
		ObjectBaseStateChange(FGsStateLocalRun);
		break;
	default:
		return false;
	}

	return true;
}

/// FStateSideWalk ///
uint8 FGsStateLocalSideWalk::GetStateID()
{
	return static_cast<uint8>(EGsStateBase::SideWalk);
}

FString FGsStateLocalSideWalk::Name()
{
	return TEXT("StateSideWalk");
}

bool FGsStateLocalSideWalk::OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateBase::Idle:
		ObjectBaseStateChange(FGsStateLocalIdle);
		break;
	case EGsStateBase::ForwardWalk:
		ObjectBaseStateChange(FGsStateLocalForwardWalk);
		break;
	case EGsStateBase::BackwardWalk:
		ObjectBaseStateChange(FGsStateLocalBackwardWalk);
		break;
	default:
		return false;
	}

	return true;
}

/// FStateRun ///
uint8 FGsStateLocalRun::GetStateID()
{
	return static_cast<uint8>(EGsStateBase::Run);
}

FString FGsStateLocalRun::Name()
{
	return TEXT("StateRun");
}

bool FGsStateLocalRun::OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateBase::Idle:
		ObjectBaseStateChange(FGsStateLocalIdle);
		break;
	case EGsStateBase::ForwardWalk:
		ObjectBaseStateChange(FGsStateLocalForwardWalk);
		break;
	default:
		break;
	}

	return true;
}


///FStateUpperRide///
uint8 FGsStateLocalRide::GetStateID()
{
	return static_cast<uint8>(EGsStateBase::Ride);
}

FString FGsStateLocalRide::Name()
{
	return TEXT("StateRide");
}

bool FGsStateLocalRide::OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateBase::Idle:
		ObjectBaseStateChange(FGsStateLocalIdle);
		break;
	}

	return true;
}

void FGsStateLocalRide::Enter(UGsGameObjectBase* Owner)
{
	Super::Enter(Owner);

	//피직 정보 끄기
	auto my = Cast<UGsGameObjectLocal>(Owner);
	my->GetLocalCharacter()->DisableCollision();

	//컨트롤러 변경
	if (auto controller = UGameplayStatics::GetPlayerController(Owner->GetWorld(), 0))
	{
		controller->UnPossess();
		controller->Possess(my->GetVehicle()->GetWhellVehicle());
	}
}

void FGsStateLocalRide::Exit(UGsGameObjectBase* Owner)
{
	Super::Exit(Owner);

	//컨트롤러 원복
	auto my = Cast<UGsGameObjectLocal>(Owner);
	if (auto controller = UGameplayStatics::GetPlayerController(Owner->GetWorld(), 0))
	{
		controller->UnPossess();
		controller->Possess(my->GetLocalCharacter());
	}

	my->GetLocalCharacter()->EnableCollision();
}

///FStateUpperIdle///
uint8 FGsStateLocalUpperIdle::GetStateID()
{
	return static_cast<uint8>(EGsStateUpperBase::Idle);
}

FString FGsStateLocalUpperIdle::Name()
{
	return TEXT("StateUpperIdle");
}

bool FGsStateLocalUpperIdle::OnProcessEvent(UGsGameObjectBase* Owner, EGsStateUpperBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateUpperBase::Attack:
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

/// FStateAttack ///
uint8 FGsStateLocalAttack::GetStateID()
{
	return static_cast<uint8>(EGsStateUpperBase::Attack);
}

FString FGsStateLocalAttack::Name()
{
	return TEXT("StateAttack");
}

bool FGsStateLocalAttack::OnProcessEvent(UGsGameObjectBase* Owner, EGsStateUpperBase StateID)
{
	if (false == Super::OnProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (StateID)
	{
	case EGsStateUpperBase::Idle:
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