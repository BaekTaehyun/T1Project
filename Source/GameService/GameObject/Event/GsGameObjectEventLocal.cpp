// Fill out your copyright notice in the Description page of Project Settings.


#include "GsGameObjectEventLocal.h"
#include "Kismet/GameplayStatics.h"
#include "Message/GsMessageManager.h"
#include "GameObject/State/GsStateBase.h"
#include "GameObject/State/GsFSMManager.h"
#include "GameObject/ActorExtend/GsWheelVehicle.h"
#include "GameObject/ObjectClass/GsGameObjectWheelVehicle.h"
#include "GameObject/ObjectClass/GsGameObjectLocal.h"

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

	//Attach
	AActor* passengerActor = cast->Passenger->GetActor();
	AWheeledVehicle* vehicleActor = Cast<UGsGameObjectWheelVehicle>(cast->Target)->GetWhellVehicle();
	passengerActor->AttachToActor(vehicleActor, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WheelVehicleSlot01"));

	//���� ž��
	if (cast->Passenger->IsObjectType(EGsGameObjectType::LocalPlayer))
	{
		//���� ��ȯ
		Cast<UGsGameObjectLocal>(Owner)->GetBaseFSM()->ProcessEvent(EGsStateBase::Ride);

		//��Ʈ�ѷ� ����
		if (auto controller = UGameplayStatics::GetPlayerController(cast->Passenger->GetActor()->GetWorld(), 0))
		{
			controller->UnPossess();
			controller->Possess(vehicleActor);
		}
	}
}

