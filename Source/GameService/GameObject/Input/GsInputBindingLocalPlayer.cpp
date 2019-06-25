﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "GsInputBindingLocalPlayer.h"
#include "GameObject/ActorExtend/GsLocalCharacter.h"
#include "GameObject/Parts/GsPartsLocal.h"
#include "GameObject/Skill/GsSKillLocal.h"
#include "GameObject/State/GsFSMManager.h"
#include "GameObject/State/GsStateLocal.h"
#include "GameObject/Movement/GsMovementLocal.h"
#include "GameObject/GsSpawnComponent.h"
#include "GameObject/ObjectClass/GsGameObjectWheelVehicle.h"
#include "Message/GsMessageManager.h"

#include "../../Camera/GsCameraModeManager.h"
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

	input->BindAction<FOnAttachParts>("LocalAttachPartsH", IE_Released, this, &UGsInputBindingLocalPlayer::OnAttachParts, EGsPartsType::HEAD);
	input->BindAction<FOnAttachParts>("LocalAttachPartsB", IE_Released, this, &UGsInputBindingLocalPlayer::OnAttachParts, EGsPartsType::BODY);
	input->BindAction<FOnAttachParts>("LocalAttachPartsG", IE_Released, this, &UGsInputBindingLocalPlayer::OnAttachParts, EGsPartsType::GLOVE);
	input->BindAction<FOnAttachParts>("LocalAttachPartsL", IE_Released, this, &UGsInputBindingLocalPlayer::OnAttachParts, EGsPartsType::LEG);
	input->BindAction<FOnAttachParts>("LocalAttachPartsHair", IE_Released, this, &UGsInputBindingLocalPlayer::OnAttachParts, EGsPartsType::HAIR);
	input->BindAction<FOnAttachParts>("LocalAttachPartsFace", IE_Released, this, &UGsInputBindingLocalPlayer::OnAttachParts, EGsPartsType::FACE);

	//key
	input->BindAction<FOnAttack1>("LocalAttack1", IE_Released, this, &UGsInputBindingLocalPlayer::OnAttack1, 1);
	input->BindAction<FOnAttack1>("LocalAttack2", IE_Released, this, &UGsInputBindingLocalPlayer::OnAttack1, 2);
	input->BindAction<FOnAttack1>("LocalAttack3", IE_Released, this, &UGsInputBindingLocalPlayer::OnAttack1, 3);
	input->BindAction("LocalAction", IE_Released, this, &UGsInputBindingLocalPlayer::OnAction);

	//Movement
	input->BindAxis("LocalMoveForward", this, &UGsInputBindingLocalPlayer::OnMoveForward);
	//input->BindAxis("LocalMoveBackward",this, &UGsInputBindingLocalPlayer::OnMoveBackward);
	//input->BindAxis("LocalMoveLeft", this, &UGsInputBindingLocalPlayer::OnMoveLeft);
	input->BindAxis("LocalMoveRight", this, &UGsInputBindingLocalPlayer::OnMoveRight);
	//input->BindAxis("LocalMoveStop", IE_Released, this, &UGsInputBindingLocalPlayer::OnMoveStop);

	input->BindAxis("LocalMoveRotate", this, &UGsInputBindingLocalPlayer::OnMoveRotateYaw);
	input->BindAxis("LocalTurn", this, &UGsInputBindingLocalPlayer::OnMoveRotateYaw);
	input->BindAxis("LocalLookUp", this, &UGsInputBindingLocalPlayer::OnMoveRotatePitch);


	input->BindAction(TEXT("ZoomIn"),
		EInputEvent::IE_Pressed,
		this,
		&UGsInputBindingLocalPlayer::OnZoomIn);

	input->BindAction(TEXT("ZoomOut"),
		EInputEvent::IE_Pressed,
		this,
		&UGsInputBindingLocalPlayer::OnZoomOut);

	input->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed,
		this, &UGsInputBindingLocalPlayer::OnViewChange);

#ifdef OLD_TOUCH_INPUT
	// yjchoung : BindTouch 추가하면서 막았습니다. 정리부탁드립니다.
	// LocalLookUP/LocalTurn의 마우스 X, Y 인풋 삭제했습니다.
	input->BindAction(TEXT("TouchOff"), EInputEvent::IE_Released, this,
		&UGsInputBindingLocalPlayer::OnTouchRelease);
	input->BindAction(TEXT("TouchOn"), EInputEvent::IE_Pressed, this,
		&UGsInputBindingLocalPlayer::OnTouchPress);
#else //OLD_TOUCH_INPUT
	input->BindTouch(EInputEvent::IE_Pressed, this, &UGsInputBindingLocalPlayer::OnTouchPress);
	input->BindTouch(EInputEvent::IE_Released, this, &UGsInputBindingLocalPlayer::OnTouchRelease);
	input->BindTouch(EInputEvent::IE_Repeat, this, &UGsInputBindingLocalPlayer::OnTouchMove);	
#endif //OLD_TOUCH_INPUT
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

//테스트 탈것 처리
//근처에 탈것이 있는지 찾는다
#include "GameObject/Event/GsGameObjectEventLocal.h"
void UGsInputBindingLocalPlayer::OnAction()
{
	if (auto findVehicle = GSpawner()->FindObject(EGsGameObjectType::Vehicle))
	{
		//탑승 정보 Send
		GSCHECK(GMessage());

		//임시 데이터 생성
		auto sendParam = Target->GetEvent()->GetCastParam< GsGameObjectEventParamVehicleRide>(MessageGameObject::Action::VehicleRide);		
		sendParam->Target = Cast<UGsGameObjectWheelVehicle>(findVehicle);
		sendParam->Passenger = Target;
		GMessage()->GetGo().SendMessage(MessageGameObject::Action::VehicleRide, *sendParam);
	}
}

void UGsInputBindingLocalPlayer::OnMoveStop()
{
	if (FunctionMoveStop != nullptr)
	{
		FunctionMoveStop();
	}
}

void UGsInputBindingLocalPlayer::OnMoveForward(float Value)
{
#ifdef OLD_FUNCTION
	if (FunctionMoveForward != nullptr)
	{
		FunctionMoveForward(Value);
	}
#else
	if (FunctionMoveForwardBackward != nullptr)
	{
		FunctionMoveForwardBackward(Value);
	}
#endif
}

void UGsInputBindingLocalPlayer::OnMoveBackward(float Value)
{
#ifdef OLD_FUNCTION
	if (FunctionMoveBackward != nullptr)
	{
		FunctionMoveBackward(Value);
	}
#endif
}

void UGsInputBindingLocalPlayer::OnMoveLeft(float Value)
{
#ifdef OLD_FUNCTION
	if (FunctionMoveLeft != nullptr)
	{
		FunctionMoveLeft(Value);
	}
#endif
}

void UGsInputBindingLocalPlayer::OnMoveRight(float Value)
{
#ifdef OLD_FUNCTION
	if (FunctionMoveRight != nullptr)
	{
		FunctionMoveRight(Value);
	}
#else
	if (FunctionMoveLeftRight != nullptr)
	{
		FunctionMoveLeftRight(Value);
	}
#endif
}

void UGsInputBindingLocalPlayer::OnMoveRotate(float Value)
{
	FVector dir = FRotationMatrix(Target->GetLocalCharacter()->GetControlRotation()).GetScaledAxis(EAxis::Y);
	Target->GetLocalCharacter()->AddMovementInput(dir, Value);
}

void UGsInputBindingLocalPlayer::OnMoveRotateYaw(float Value)
{
#ifdef OLD_FUNCTION
	Target->GetLocalCharacter()->AddControllerYawInput(Value);
#else
	if (FunctionTurn != nullptr)
	{
		FunctionTurn(Value);
	}
#endif
}

void UGsInputBindingLocalPlayer::OnMoveRotatePitch(float Value)
{
#ifdef OLD_FUNCTION
	Target->GetLocalCharacter()->AddControllerPitchInput(Value);
#else
	if (FunctionLookUp != nullptr)
	{
		FunctionLookUp(Value);
	}
#endif
}

#ifdef OLD_TOUCH_INPUT
// 터치 시작(pc는 좌클릭)
void UGsInputBindingLocalPlayer::OnTouchPress()
{
	if (FunctionTouchPress != nullptr)
	{
		FunctionTouchPress();
	}

	GSLOG(Error, TEXT("OnTouchPress"));
}
// 터치 끝(pc는 좌클릭)
void UGsInputBindingLocalPlayer::OnTouchRelease()
{
	if (FunctionTouchRelease != nullptr)
	{
		FunctionTouchRelease();
	}

	GSLOG(Error, TEXT("OnTouchRelease"));
}
#else //OLD_TOUCH_INPUT
void UGsInputBindingLocalPlayer::OnTouchPress(ETouchIndex::Type FingerIndex, FVector Location)
{
	PrevTouchLocation = Location;

	if (FunctionTouchPress != nullptr)
	{
		FunctionTouchPress();
	}
}

void UGsInputBindingLocalPlayer::OnTouchRelease(ETouchIndex::Type FingerIndex, FVector Location)
{
	PrevTouchLocation = FVector::ZeroVector;

	if (FunctionTouchRelease != nullptr)
	{
		FunctionTouchRelease();
	}
}

void UGsInputBindingLocalPlayer::OnTouchMove(ETouchIndex::Type FingerIndex, FVector Location)
{
	FVector dir = Location - PrevTouchLocation;
	PrevTouchLocation = Location;

	float yaw = FMath::Clamp(dir.X / TouchMoveMax, -1.0f, 1.0f);
	float pitch = FMath::Clamp(dir.Y / TouchMoveMax, -1.0f, 1.0f);

	if (FunctionTurn != nullptr)
	{
		FunctionTurn(yaw);
	}

	if (FunctionLookUp != nullptr)
	{
		FunctionLookUp(pitch);
	}
}
#endif //OLD_TOUCH_INPUT

// 줌인
void UGsInputBindingLocalPlayer::OnZoomIn()
{
	if (FunctionZoomIn != nullptr)
	{
		FunctionZoomIn();
	}
}
// 줌아웃
void UGsInputBindingLocalPlayer::OnZoomOut()
{
	if (FunctionZoomOut != nullptr)
	{
		FunctionZoomOut();
	}
}
// 카메로 모드 변경
void UGsInputBindingLocalPlayer::OnViewChange()
{
	if (GCamera() != nullptr)
	{
		GCamera()->NextStep();
	}
}
