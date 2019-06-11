// Fill out your copyright notice in the Description page of Project Settings.


#include "GsMovementLocal.h"
#include "GameService.h"
#include "GameObject/ActorExtend/GsLocalCharacter.h"
#include "GameObject/ObjectClass/GsGameObjectLocal.h"
#include "GameObject/State/GsFSMManager.h"
#include "GameObject/State/GsStateLocal.h"

bool FGsMovementLocal::IsMove()
{
	return CharMovement->Velocity != FVector::ZeroVector;
}

float FGsMovementLocal::GetRateAccelerator()
{
	return RateAccelerator;
}

FGsMovementLocal::FGsMovementLocal()
{
	RateAccelerator = 1.f;
}

FGsMovementLocal::~FGsMovementLocal()
{
}

void FGsMovementLocal::Initialize(UGsGameObjectBase* owner)
{
	Super::Initialize(owner);

    Local = Cast<UGsGameObjectLocal>(owner);
    if (Local)
    {
        CharMovement = Local->GetLocalCharacter()->GetCharacterMovement();
    }
}

void FGsMovementLocal::Finalize()
{
	Super::Finalize();
}

void FGsMovementLocal::Update(float Delta)
{
	Super::Update(Delta);

    if (IsMove())
    {
		RateAccelerator = FMath::Min(RateAccelerator + Delta, 2.f);
	}
	else
	{
		Stop();
	}
}

void FGsMovementLocal::OnStop()
{
	Super::OnStop();

	RateAccelerator = 1.f;
	Local->GetBaseFSM()->ProcessEvent(EGsStateBase::Idle);
}

void FGsMovementLocal::OnMove()
{
	Super::OnMove();

	//임시 이동 구현 코드들...
	Local->GetLocalCharacter()->AddMovementInput(Direction, MoveSpeed * RateAccelerator);

    //상태 전환
	if (IsMove())
	{
		FGsFSMManager* fsm = Local->GetBaseFSM();
		if (RateAccelerator >= 2.f)
		{
			fsm->ProcessEvent(EGsStateBase::Run);
		}
		else
		{
			fsm->ProcessEvent(EGsStateBase::ForwardWalk);
		}
	}
}
