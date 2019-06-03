// Fill out your copyright notice in the Description page of Project Settings.


#include "GsGameObjectEventLocal.h"
#include "Message/GsMessageManager.h"
#include "GameObject/State/GsStateBase.h"
#include "GameObject/State/GsFSMManager.h"
#include "GameObject/ActorExtend/GsWheelVehicle.h"
#include "GameObject/ObjectClass/GsGameObjectWheelVehicle.h"
#include "GameObject/ObjectClass/GsGameObjectLocal.h"
#include "GameObject/ActorExtend/GsLocalCharacter.h"

FGsGameObjectEventLocal::FGsGameObjectEventLocal(UGsGameObjectBase* owner) : Super(owner)
{
}

FGsGameObjectEventLocal::~FGsGameObjectEventLocal()
{
}

void FGsGameObjectEventLocal::RegistEvent()
{
	Super::RegistEvent();

	GSCHECK(GMessage());
	GMessage()->GetGo().AddRaw(MessageGameObject::Action::VehicleRide, this, &FGsGameObjectEventLocal::OnVehicleRide);
}

GsGameObjectEventParamBase* FGsGameObjectEventLocal::CreateParam(MessageGameObject::Action Type)
{
	switch (Type)
	{
	case MessageGameObject::Action::VehicleRide:
		return new GsGameObjectEventParamVehicleRide();
	}

	return Super::CreateParam(Type);
}

void FGsGameObjectEventLocal::OnVehicleRide(const GsGameObjectEventParamBase& Param)
{
	auto cast = GsGameObjectEventParamBase::ConstCast<GsGameObjectEventParamVehicleRide>(Param);

	//ž�� ���� ������Ʈ�ΰ� Ȯ��
	if (false == cast->Passenger->IsObjectType(EGsGameObjectType::Player))
	{
		return;
	}

	//Attach
	auto passengerGo = Cast<UGsGameObjectPlayer>(cast->Passenger);
	auto vehicleGo = Cast<UGsGameObjectWheelVehicle>(cast->Target);
	auto fsm = passengerGo->GetBaseFSM();
	auto passengerActor = passengerGo->GetActor();
	auto vehicleActor = vehicleGo->GetWhellVehicle();

	//������ȯ : ���� ž�°� ������ �и��ϴ°� ���� ����
	if (fsm->IsState(EGsStateBase::Ride))
	{
		passengerGo->SetVehicle(NULL);
		vehicleGo->RemovePassenger(passengerGo);
		fsm->ProcessEvent(EGsStateBase::Idle);

		passengerActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
	else
	{
		passengerGo->SetVehicle(vehicleGo);
		vehicleGo->SetPassenger(passengerGo);
		fsm->ProcessEvent(EGsStateBase::Ride);
		
		//�ӽ� ���� ���� ���
		passengerActor->AttachToActor(vehicleActor, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WheelVehicleSlot01"));
	}
	
}

