// Fill out your copyright notice in the Description page of Project Settings.

#include "GsStateLocal.h"
#include "GsFSMManager.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "GameObject/ActorExtend/GsLocalCharacter.h"
#include "GameObject/ObjectClass/GsGameObjectLocal.h"
#include "GameObject/Movement/GsMovementLocal.h"
#include "GameObject/ObjectClass/GsGameObjectWheelVehicle.h"
#include "GameObject/ActorExtend/GsWheelVehicle.h"

//----------------------------------------------------------------------------------------
// LocalState Base
//----------------------------------------------------------------------------------------
template <class tState>
bool FGsStateBaseSingleLocal<tState>::ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID)
{
	return true;
}

template <class tState>
void FGsStateBaseSingleLocal<tState>::Enter(UGsGameObjectBase* Owner)
{
	auto my = Cast<UGsGameObjectLocal>(Owner);
	if (auto anim = my->GetLocalCharacter()->GetAnim())
	{
		//AnimInstance에 상태값 알림
		anim->ChangeState(GetStateID(), 0, GetAniRandomCount());
	}
}

//----------------------------------------------------------------------------------------
// Local Movement State Base
//----------------------------------------------------------------------------------------
template<class tState>
void FGsStateLocalMoveBase<tState>::Enter(UGsGameObjectBase* Owner)
{
	Super::Enter(Owner);

	auto my = Cast<UGsGameObjectLocal>(Owner);
	if (auto anim = my->GetLocalCharacter()->GetAnim())
	{
		//AnimInstance에 이동 설정 알림
		anim->SetMoving(true);
	}
}

template<class tState>
void FGsStateLocalMoveBase<tState>::Update(UGsGameObjectBase* Owner, float Delta)
{
	Super::Update(Owner, Delta);

	//이동 Update 활성
	auto my = Cast<UGsGameObjectLocal>(Owner);
	my->GetMovement()->Update(Delta);
}

template<class tState>
void FGsStateLocalMoveBase<tState>::Exit(UGsGameObjectBase* Owner)
{
	Super::Exit(Owner);

	auto my = Cast<UGsGameObjectLocal>(Owner);
	if (auto anim = my->GetLocalCharacter()->GetAnim())
	{
		//AnimInstance에 이동 설정 알림
		anim->SetMoving(false);
	}
}

//----------------------------------------------------------------------------------------
// FStateSpawn
//----------------------------------------------------------------------------------------
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

bool FGsStateLocalSpawn::ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID)
{
	if (false == Super::ProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (static_cast<EGsStateBase>(StateID))
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

//----------------------------------------------------------------------------------------
// FStateIdle 
//----------------------------------------------------------------------------------------
uint8 FGsStateLocalIdle::GetStateID()
{
	return static_cast<uint8>(EGsStateBase::Idle);
}

FString FGsStateLocalIdle::Name()
{
	return TEXT("StateIdle");
}

bool FGsStateLocalIdle::ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID)
{
	if (false == Super::ProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (static_cast<EGsStateBase>(StateID))
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

//----------------------------------------------------------------------------------------
// FStateForwardWalk
//----------------------------------------------------------------------------------------
uint8 FGsStateLocalForwardWalk::GetStateID()
{
	return static_cast<uint8>(EGsStateBase::ForwardWalk);
}

FString FGsStateLocalForwardWalk::Name()
{
	return TEXT("StateForwardWalk");
}

bool FGsStateLocalForwardWalk::ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID)
{
	if (false == Super::ProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (static_cast<EGsStateBase>(StateID))
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


void FGsStateLocalForwardWalk::Update(UGsGameObjectBase* Owner, float Delta)
{
	Super::Update(Owner, Delta);

	auto my = Cast<UGsGameObjectLocal>(Owner);
	auto localmovement = static_cast<FGsMovementLocal*>(my->GetMovement());
	if (localmovement->GetRateAccelerator() >= 2.f)
	{
		FGsFSMManager* fsm = my->GetBaseFSM();
		fsm->ProcessEvent(EGsStateBase::Run);
	}

	/*auto skel = my->GetLocalCharacter()->GetMesh();
	if (auto anim2 = skel->GetSingleNodeInstance())
	{
		anim2->SetPlayRate(localmovement->GetRateAccelerator());
	}*/
}

//----------------------------------------------------------------------------------------
// FStateBackwardWalk
//----------------------------------------------------------------------------------------
uint8 FGsStateLocalBackwardWalk::GetStateID()
{
	return static_cast<uint8>(EGsStateBase::BackwardWalk);
}

FString FGsStateLocalBackwardWalk::Name()
{
	return TEXT("StateBackwardWalk");
}

bool FGsStateLocalBackwardWalk::ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID)
{
	if (false == Super::ProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (static_cast<EGsStateBase>(StateID))
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

//----------------------------------------------------------------------------------------
// FStateSideWalk
//----------------------------------------------------------------------------------------
uint8 FGsStateLocalSideWalk::GetStateID()
{
	return static_cast<uint8>(EGsStateBase::SideWalk);
}

FString FGsStateLocalSideWalk::Name()
{
	return TEXT("StateSideWalk");
}

bool FGsStateLocalSideWalk::ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID)
{
	if (false == Super::ProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (static_cast<EGsStateBase>(StateID))
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

//----------------------------------------------------------------------------------------
// FStateRun
//----------------------------------------------------------------------------------------
uint8 FGsStateLocalRun::GetStateID()
{
	return static_cast<uint8>(EGsStateBase::Run);
}

FString FGsStateLocalRun::Name()
{
	return TEXT("StateRun");
}

bool FGsStateLocalRun::ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID)
{
	if (false == Super::ProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (static_cast<EGsStateBase>(StateID))
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

//----------------------------------------------------------------------------------------
// FStateRide
//----------------------------------------------------------------------------------------
uint8 FGsStateLocalRide::GetStateID()
{
	return static_cast<uint8>(EGsStateBase::Ride);
}

FString FGsStateLocalRide::Name()
{
	return TEXT("StateRide");
}

bool FGsStateLocalRide::ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID)
{
	if (false == Super::ProcessEvent(Owner, StateID))
	{
		return false;
	}

	switch (static_cast<EGsStateBase>(StateID))
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

