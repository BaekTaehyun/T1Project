// Fill out your copyright notice in the Description page of Project Settings.

#include "GsStateBase.h"
#include "GsFSMManager.h"

IGsStateBase::IGsStateBase()
{
}

IGsStateBase::~IGsStateBase()
{
}

/*bool IGsStateBase::IsChange(int StateID)
{
	//일단 기본적으로 동일 상태의 접근을 방지
	return !IsSameState(StateID);
}*/

bool IGsStateBase::IsSameState(uint8 StateID)
{
	return (GetStateID() == StateID);
}
void IGsStateBase::ChangeDelayPrevState(FGsFSMManager* Fsm, float Time)
{
	if (Fsm && Fsm->Prev) { Fsm->ChangeDelayPrevState(Time); }
}

