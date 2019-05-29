// Fill out your copyright notice in the Description page of Project Settings.

#include "GsFSMManager.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "GsStateBase.h"

void FGsFSMManager::DeInitialize()
{
}

bool FGsFSMManager::IsState(int StateID) const
{
	return (nullptr != Current && Current->GetStateID() == StateID);
}

void FGsFSMManager::Update(UGsGameObjectBase* owner, float Delta)
{
	if (Current)
	{
		Current->Update(owner, Delta);
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
