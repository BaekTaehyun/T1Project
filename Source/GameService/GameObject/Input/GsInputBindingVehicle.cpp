// Fill out your copyright notice in the Description page of Project Settings.


#include "GsInputBindingVehicle.h"
#include "Message/GsMessageManager.h"
#include "GameObject/GsSpawnComponent.h"
#include "GameObject/Event/GsGameObjectEventBase.h"
#include "GameObject/ObjectClass/GsGameObjectWheelVehicle.h"
#include "GameObject/ActorExtend/GsWheelVehicle.h"
#include "PhysXVehicles/Public/WheeledVehicleMovementComponent.h"

void UGsInputBindingVehicle::Initialize()
{
	//error
}

void UGsInputBindingVehicle::Initialize(UGsGameObjectWheelVehicle* target)
{
	Target = target;
	MovementComponent = Target->GetWhellVehicle()->GetVehicleMovementComponent();
}

void UGsInputBindingVehicle::SetBinding(UInputComponent* input)
{
	Super::SetBinding(input);

	input->BindAction("LocalMoveForward", IE_Pressed, this, &UGsInputBindingVehicle::OnMoveForward);
	input->BindAction("LocalMoveBackward", IE_Pressed, this, &UGsInputBindingVehicle::OnMoveBackward);
	input->BindAxis("LocalMoveRotate", this, &UGsInputBindingVehicle::OnMoveRight);
	input->BindAction("LocalAction", IE_Released, this, &UGsInputBindingVehicle::OnAction);
}

void UGsInputBindingVehicle::OnMoveForward()
{
	MovementComponent->SetThrottleInput(1.f);
}

void UGsInputBindingVehicle::OnMoveBackward()
{
	MovementComponent->SetThrottleInput(0.f);
}

void UGsInputBindingVehicle::OnMoveRight(float Val)
{
	MovementComponent->SetSteeringInput(Val);
}

//테스트 탈것 처리
//근처에 탈것이 있는지 찾는다
#include "GameObject/Event/GsGameObjectEventLocal.h"
#include "GameObject/ObjectClass/GsGameObjectLocal.h"
void UGsInputBindingVehicle::OnAction()
{
	GSCHECK(GMessage());

	if (auto my = GSpawner()->FindObject(EGsGameObjectType::LocalPlayer))
	{
		auto sendParam = Cast<UGsGameObjectLocal>(my)->GetEvent()->GetCastParam<GsGameObjectEventParamVehicleRide>(MessageGameObject::Action::VehicleRide);
		sendParam->Target = Cast<UGsGameObjectWheelVehicle>(Target);
		sendParam->Passenger = my;
		GMessage()->GetGo().SendMessage(MessageGameObject::Action::VehicleRide, *sendParam);
	}
}
