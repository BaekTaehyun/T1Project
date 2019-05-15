// Fill out your copyright notice in the Description page of Project Settings.

#include "GsInputBindingLocalPlayer.h"
#include "GameObject/ActorExtend/GsLocalCharacter.h"
#include "GameObject/Parts/GsPartsLocal.h"
#include "GameObject/Skill/GsSKillLocal.h"
#include "GameObject/State/GsFSMManager.h"
#include "GameObject/State/GsStateLocal.h"
#include "GameObject/Movement/GsMovementLocal.h"

//[Todo]타겟 클래스 설정 방식에 좀더 좋은 구조를 생각해볼것
void UGsInputBindingLocalPlayer::Initialize()
{
	//error
}

void UGsInputBindingLocalPlayer::Initialize(UGsGameObjectLocal* target)
{
	Target = target;
}

void UGsInputBindingLocalPlayer::SetBinding(UInputComponent* input)
{
	Super::SetBinding(input);

	input->BindAction<FOnAttachParts>("AttachPartsH", IE_Released, this, &UGsInputBindingLocalPlayer::OnAttachParts, EGsPartsType::HEAD);
	input->BindAction<FOnAttachParts>("AttachPartsB", IE_Released, this, &UGsInputBindingLocalPlayer::OnAttachParts, EGsPartsType::BODY);
	input->BindAction<FOnAttachParts>("AttachPartsG", IE_Released, this, &UGsInputBindingLocalPlayer::OnAttachParts, EGsPartsType::GLOVE);
	input->BindAction<FOnAttachParts>("AttachPartsL", IE_Released, this, &UGsInputBindingLocalPlayer::OnAttachParts, EGsPartsType::LEG);
	input->BindAction<FOnAttachParts>("AttachPartsHair", IE_Released, this, &UGsInputBindingLocalPlayer::OnAttachParts, EGsPartsType::HAIR);
	input->BindAction<FOnAttachParts>("AttachPartsFace", IE_Released, this, &UGsInputBindingLocalPlayer::OnAttachParts, EGsPartsType::FACE);

	//key
	input->BindAction<FOnAttack1>("Attack1", IE_Released, this, &UGsInputBindingLocalPlayer::OnAttack1, 1);
	input->BindAction<FOnAttack1>("Attack2", IE_Released, this, &UGsInputBindingLocalPlayer::OnAttack1, 2);
	input->BindAction<FOnAttack1>("Attack3", IE_Released, this, &UGsInputBindingLocalPlayer::OnAttack1, 3);

	//Movement
	input->BindAction("MoveForward", IE_Pressed, this, &UGsInputBindingLocalPlayer::OnMoveForward);
	input->BindAction("MoveBackward", IE_Pressed, this, &UGsInputBindingLocalPlayer::OnMoveBackward);
	input->BindAction("MoveLeft", IE_Pressed, this, &UGsInputBindingLocalPlayer::OnMoveLeft);
	input->BindAction("MoveRight", IE_Pressed, this, &UGsInputBindingLocalPlayer::OnMoveRight);
	input->BindAction("MoveStop", IE_Released, this, &UGsInputBindingLocalPlayer::OnMoveStop);

	input->BindAxis("MoveRotate", this, &UGsInputBindingLocalPlayer::OnMoveRotateYaw);
	input->BindAxis("Turn", this, &UGsInputBindingLocalPlayer::OnMoveRotateYaw);
	input->BindAxis("LookUp", this, &UGsInputBindingLocalPlayer::OnMoveRotatePitch);
}

void UGsInputBindingLocalPlayer::OnAttachParts(EGsPartsType Type)
{
	if (auto parts = Target->GetParts())
	{
		parts->IsEquip(Type) ? parts->Detach(Type) : parts->Attach(Type);
		Target->GetActor()->GetActorRotation();
	}
}

void UGsInputBindingLocalPlayer::OnAttack1(int32 slot)
{
	if (auto skill = Target->GetSkill())
	{
		skill->UseSKill(slot);
	}
}

void UGsInputBindingLocalPlayer::OnMoveStop()
{
    if (auto movement = Target->GetMovement())
    {
        movement->Stop();
    }
}

void UGsInputBindingLocalPlayer::OnMoveForward()
{
    if (auto movement = Target->GetMovement())
    {
        FVector dir = FRotationMatrix(Target->GetLocal()->Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
        movement->Move(dir, EGsGameObjectMoveDirType::Forward, 10.0f);
    }
}

void UGsInputBindingLocalPlayer::OnMoveBackward()
{
    if (auto movement = Target->GetMovement())
    {
        FVector dir = FRotationMatrix(Target->GetLocal()->Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
        movement->Move(dir, EGsGameObjectMoveDirType::Backward, -5.f);
    }
}

void UGsInputBindingLocalPlayer::OnMoveLeft()
{
    if (auto movement = Target->GetMovement())
    {
        FVector dir = FRotationMatrix(Target->GetLocal()->Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
        movement->Move(dir, EGsGameObjectMoveDirType::SideStep, -5.f);
    }
}

void UGsInputBindingLocalPlayer::OnMoveRight()
{
    if (auto movement = Target->GetMovement())
    {
        FVector dir = FRotationMatrix(Target->GetLocal()->Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
        movement->Move(dir, EGsGameObjectMoveDirType::SideStep, 5.f);
    }
}

void UGsInputBindingLocalPlayer::OnMoveRotate(float Value)
{
	FVector dir = FRotationMatrix(Target->GetLocal()->GetControlRotation()).GetScaledAxis(EAxis::Y);
	Target->GetLocal()->AddMovementInput(dir, Value);
}

void UGsInputBindingLocalPlayer::OnMoveRotateYaw(float Value)
{
	Target->GetLocal()->AddControllerYawInput(Value);
}

void UGsInputBindingLocalPlayer::OnMoveRotatePitch(float Value)
{
	Target->GetLocal()->AddControllerPitchInput(Value);
}
