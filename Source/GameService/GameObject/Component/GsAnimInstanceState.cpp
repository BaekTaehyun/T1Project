// Fill out your copyright notice in the Description page of Project Settings.

#include "GsAnimInstanceState.h"

bool UGsAnimInstanceState::IsState(EGsStateBase State)
{
	return BaseStateType == State;
}

bool UGsAnimInstanceState::IsStates(const TArray<EGsStateBase>& States)
{
	for (auto el : States)
	{
		if (el == BaseStateType)
		{
			return true;
		}
	}
	return false;
	/*
	return States.Contains([=](const ECharacterStateBase& el)
	{
		return BaseStateType == el;
	});*/
}

bool UGsAnimInstanceState::IsUpperState(EGsStateUpperBase State)
{
	return UpperStateType == State;
}

bool UGsAnimInstanceState::IsUpperBlend()
{
	return IsMoveState();
}

bool UGsAnimInstanceState::IsMoveState()
{
	return BaseStateType == EGsStateBase::ForwardWalk ||
		BaseStateType == EGsStateBase::BackwardWalk ||
		BaseStateType == EGsStateBase::SideWalk ||
		BaseStateType == EGsStateBase::Run;
}

int UGsAnimInstanceState::GetRandomIndex()
{
	return RandomIndex;
}

void UGsAnimInstanceState::ChangeState(uint8 State, int Min, int Max)
{
	if (State >= (uint8)EGsStateUpperBase::None)
	{
		EGsStateUpperBase changeState = static_cast<EGsStateUpperBase>(State);
		if (UpperStateType != changeState)
		{
			UpperTimer = FApp::GetCurrentTime();
		}
		UpperStateType = changeState;
	}
	else
	{
		EGsStateBase changeState = static_cast<EGsStateBase>(State);
		if (BaseStateType != changeState)
		{
			Timer = FApp::GetCurrentTime();
		}
		BaseStateType = changeState;
	}

	//랜덤 인덱스 생성
	if (Max - Min > 0)
	{
		RandomIndex = FMath::RandRange(Min, Max);
	}
	else
	{
		RandomIndex = 0;
	}

	UE_LOG(LogTemp, Warning, TEXT("UAnimInstanceLocal ChangeState Lowwer : %d  Upper : %d"), (int)BaseStateType, (int)UpperStateType);
}

void UGsAnimInstanceState::PlayUpperAni(UAnimMontage* Res)
{
	Montage_Play(Res);
}

void UGsAnimInstanceState::StopUpperAni(UAnimMontage* Res)
{
	Montage_Stop(0.1f, Res);
}

float UGsAnimInstanceState::GetTimer()
{
	return FApp::GetCurrentTime() - Timer;
}

float UGsAnimInstanceState::GetUpperTimer()
{
	return FApp::GetCurrentTime() - UpperTimer;
}

