// Fill out your copyright notice in the Description page of Project Settings.


#include "GsGameObjectWheelVehicle.h"
#include "GameObject/ActorExtend/GsWheelVehicle.h"
#include "GameObject/Input/GsInputBindingVehicle.h"

EGsGameObjectType UGsGameObjectWheelVehicle::GetObjectType() const			{ return EGsGameObjectType::Vehicle; }
AActor* UGsGameObjectWheelVehicle::GetActor() const							{ return GetWhellVehicle(); }
AGsWheelVehicle* UGsGameObjectWheelVehicle::GetWhellVehicle() const			{ return (Actor->IsValidLowLevel()) ? Actor : NULL; }
TArray<UGsGameObjectPlayer*> UGsGameObjectWheelVehicle::GetPassengers() const { return ListPassengers; }

UGsGameObjectWheelVehicle::~UGsGameObjectWheelVehicle()
{
}

void UGsGameObjectWheelVehicle::SetPassenger(UGsGameObjectPlayer* Passenger)
{
	ListPassengers.AddUnique(Passenger);
}

void UGsGameObjectWheelVehicle::RemovePassenger(UGsGameObjectPlayer* Passenger)
{
	ListPassengers.Remove(Passenger);
}

void UGsGameObjectWheelVehicle::Initialize()
{
	Super::Initialize();

	SET_FLAG_TYPE(ObjectType, UGsGameObjectWheelVehicle::GetObjectType());
}

void UGsGameObjectWheelVehicle::Finalize()
{
	Super::Finalize();

	ListPassengers.Reset();
}

void UGsGameObjectWheelVehicle::ActorSpawned(AActor* Spawn)
{
	Super::ActorSpawned(Spawn);

	if (Spawn)
	{
		//액터 저장
		Actor = Cast<AGsWheelVehicle>(Spawn);

		//키입력 바인딩
		InputBinder = NewObject<UGsInputBindingVehicle>();
		InputBinder->Initialize(this);
		Actor->SetInputBinder(InputBinder);
	}
}