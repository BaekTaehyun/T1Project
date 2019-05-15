// Fill out your copyright notice in the Description page of Project Settings.

#include "GsFSMManager.h"
#include "Runtime/Engine/Public/TimerManager.h"

void FGsFSMManager::Initialize(UGsGameObjectBase* owner)
{
	Owner = owner;
	Current = nullptr;
	Prev = nullptr;
}

void FGsFSMManager::DeInitialize()
{
}

bool FGsFSMManager::ChangeState(IGsStateBase* State, FGsStateChangeFailed const& FailDelegate)
{
	check(State);
	if (nullptr != Current)
	{
		//체인지 가능 여부 검사
		if (false == State->IsChange(Current->GetStateID()))
		{			
			//
			//실패에 대한 델리게이트를 연결하여 호출 상황에 맞게 유동적인 대처를 유도한다.
			if (FailDelegate.IsBound())
			{
				FailDelegate.Execute(Current->GetStateID());
			}
			//

			UE_LOG(LogTemp, Warning, TEXT("%s State Change Failed! CurrState(%s)"), *State->Name(), *Current->Name());
			return false;
		}

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
