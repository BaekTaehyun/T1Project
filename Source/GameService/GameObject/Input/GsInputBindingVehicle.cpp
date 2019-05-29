// Fill out your copyright notice in the Description page of Project Settings.


#include "GsInputBindingVehicle.h"
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
