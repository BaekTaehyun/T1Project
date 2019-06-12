// Fill out your copyright notice in the Description page of Project Settings.

#include "GsAnimInstanceState.h"

UGsAnimInstanceState::UGsAnimInstanceState()
{

}

UGsAnimInstanceState::~UGsAnimInstanceState()
{
}

bool UGsAnimInstanceState::IsState(EGsStateBase State)
{
	return StateType == State;
}

bool UGsAnimInstanceState::IsStates(const TArray<EGsStateBase>& States)
{
	//개선 필요
	for (auto el : States)
	{
		if (el == StateType)
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

bool UGsAnimInstanceState::IsUpperBlend()
{
	return IsMoveState();
}

bool UGsAnimInstanceState::IsMoveState()
{
	return Moving;
}

int UGsAnimInstanceState::GetRandomIndex()
{
	return RandomIndex;
}

void UGsAnimInstanceState::ChangeState(uint8 State, int Min, int Max)
{
	auto changeState = static_cast<EGsStateBase>(State);
	if (StateType != changeState)
	{
		Timer = FApp::GetCurrentTime();
	}
	StateType = changeState;

	//랜덤 인덱스 생성 (테스트 용)
	if (Max - Min > 0)
	{
		RandomIndex = FMath::RandRange(Min, Max);
	}
	else
	{
		RandomIndex = 0;
	}

	UE_LOG(LogTemp, Warning, TEXT("UAnimInstanceLocal ChangeState : %d"), (uint8)StateType);
}

void UGsAnimInstanceState::SetMoving(bool IsMove)
{
	Moving = IsMove;
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

