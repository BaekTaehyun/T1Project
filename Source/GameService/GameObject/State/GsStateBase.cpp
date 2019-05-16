// Fill out your copyright notice in the Description page of Project Settings.

#include "GsStateBase.h"

IGsStateBase::IGsStateBase()
{
}

IGsStateBase::~IGsStateBase()
{
}

bool IGsStateBase::IsChange(int StateID)
{
	//일단 기본적으로 동일 상태의 접근을 방지
	return !IsSameState(StateID);
}

bool IGsStateBase::IsSameState(int StateID)
{
	return (GetStateID() == StateID);
}