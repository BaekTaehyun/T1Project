// Fill out your copyright notice in the Description page of Project Settings.


#include "GsMovementLocal.h"
#include "GameService.h"
#include "GameObject/ActorExtend/GsLocalCharacter.h"
#include "GameObject/ObjectClass/GsGameObjectLocal.h"
#include "GameObject/State/GsFSMManager.h"
#include "GameObject/State/GsStateLocal.h"

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

    if (false == IsStop())
    {
        //임시 이동 구현 코드들...
        //내캐릭터 이동방향은 컨트롤러의 회전에 따라 동적 변경
        Direction = FRotationMatrix(Local->GetLocalCharacter()->Controller->GetControlRotation()).GetScaledAxis(MoveDirType == EGsGameObjectMoveDirType::SideStep ? EAxis::Y : EAxis::X);
        Local->GetLocalCharacter()->AddMovementInput(Direction, MoveSpeed);

        if (MoveDirType == EGsGameObjectMoveDirType::Forward && CHECK_FLAG_TYPE(MoveType, EGsGameObjectMoveType::Walk))
        {
            //전방 뛰기시 임시 가속 처리
            MoveSpeed += 3.f * Delta;

            float walkSpeed = FVector::DotProduct(CharMovement->Velocity, Local->GetLocalCharacter()->GetActorRotation().Vector());
            if (walkSpeed > CharMovement->MaxWalkSpeed)
            {
                SetMoveType(EGsGameObjectMoveType::Run);
				Local->GetBaseFSM()->ProcessEvent(EGsStateBase::Run);
            }
        }
    }
}

void FGsMovementLocal::OnStop()
{
	Super::OnStop();

    SetMoveType(EGsGameObjectMoveType::None);
    CharMovement->SetMovementMode(MOVE_None);

	Local->GetBaseFSM()->ProcessEvent(EGsStateBase::Idle);
}

void FGsMovementLocal::OnMove()
{
	Super::OnMove();

    SetMoveType(EGsGameObjectMoveType::Walk);
    CharMovement->SetMovementMode(MOVE_Walking);

    //상태 전환
    FGsFSMManager* fsm = Local->GetBaseFSM();
    switch (MoveDirType)
    {
    case EGsGameObjectMoveDirType::Forward:
		Local->GetBaseFSM()->ProcessEvent(EGsStateBase::ForwardWalk);
        break;
    case EGsGameObjectMoveDirType::SideStep:
		Local->GetBaseFSM()->ProcessEvent(EGsStateBase::SideWalk);
        break;
    case EGsGameObjectMoveDirType::Backward:
		Local->GetBaseFSM()->ProcessEvent(EGsStateBase::BackwardWalk);
        break;
    }
}
