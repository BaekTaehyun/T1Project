// Fill out your copyright notice in the Description page of Project Settings.

#include "GsFSMManager.h"
#include "Util/GsText.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "GsStateBase.h"
#include "GameObject/State/GsStateBase.h"
#include "GameObject/ObjectClass/GsGameObjectBase.h"

void FGsFSMManager::Finalize()
{
}

bool FGsFSMManager::IsState(uint8 StateID) const
{
	return (nullptr != Current && Current->GetStateID() == StateID);
}

bool FGsFSMManager::IsState(EGsStateBase StateType) const
{
	return (nullptr != Current && Current->GetStateID() == static_cast<uint8>(StateType));
}

bool FGsFSMManager::IsState(EGsStateUpperBase StateType) const
{
	return (nullptr != Current && Current->GetStateID() == static_cast<uint8>(StateType));
}

void FGsFSMManager::Update(UGsGameObjectBase* owner, float Delta)
{
	if (Current)
	{
		Current->Update(owner, Delta);
	}
}

void FGsFSMManager::ProcessEvent(uint8 StateID, FGsStateChangeFailed const& FailDelegate)
{
	if (Current)
	{
		if (false == Current->ProcessEvent(Owner, StateID))
		{
			if (FailDelegate.IsBound())
			{
				FailDelegate.Execute(Current->GetStateID());
			}
#if WITH_EDITOR
			//스테이트 전환 실패 로그 메세지
			FString stateName = (StateID > static_cast<uint8>(EGsStateUpperBase::None)) ?
				EnumToString(EGsStateUpperBase, static_cast<EGsStateUpperBase>(StateID)) :
				EnumToString(EGsStateBase, static_cast<EGsStateBase>(StateID));
			UE_LOG(LogTemp, Warning, TEXT("[%s] State Changed failed!  Current State : [%s]"), *stateName, *Current->Name());
#endif
		}
	}
}

bool FGsFSMManager::ChangeState(IGsStateBase* State)
{
	check(State);
	if (nullptr != Current)
	{
		//[Todo] 애님 블루프린트의 FSM전환 가능 여부를 알수 있다면 여기서 검사
		//

		if (Current->GetStateID() != State->GetStateID())
		{
			//중복 상태 호출 확인
			if (Current->GetStateID() == State->GetStateID())
			{
				Current->ReEnter(Owner);
				UE_LOG(LogTemp, Warning, TEXT("%s State ReEnter"), *Current->Name());
				return true;
			}

			Current->Exit(Owner);
			UE_LOG(LogTemp, Warning, TEXT("%s State Eixt!! Prev(%s) Curr(%s)"), *Current->Name(), *Current->Name(), nullptr != Prev ? *Prev->Name() : TEXT("null"));
			Prev = Current;
		}
	}

	Current = State;
	Current->Enter(Owner);
	UE_LOG(LogTemp, Warning, TEXT("%s State Enter"), *Current->Name());

	return true;
}

void FGsFSMManager::ChangeDelayState(IGsStateBase* State, float Time)
{
	if (nullptr == State)
	{
		return;
	}

	if (UWorld* world = Owner->GetActor()->GetWorld())
	{
		FTimerHandle handle;
		world->GetTimerManager().SetTimer(handle, 
			FTimerDelegate::CreateRaw(this, &FGsFSMManager::CallbakChangeState, State), Time, false);
	}
}

void FGsFSMManager::ChangeDelayPrevState(float Time)
{
	if (Prev)
	{
		ChangeDelayState(Prev, Time);
	}
}

void FGsFSMManager::CallbakChangeState(IGsStateBase* State)
{
	ChangeState(State);
}
