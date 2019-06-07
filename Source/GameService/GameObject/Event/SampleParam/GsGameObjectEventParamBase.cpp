// Fill out your copyright notice in the Description page of Project Settings.


#include "GsGameObjectEventParamBase.h"

GsGameObjectEventParamBase::GsGameObjectEventParamBase()
{
}

GsGameObjectEventParamBase::~GsGameObjectEventParamBase()
{
}

void GsGameObjectEventParamBase::Clear()
{
	Target = NULL;
}

GsGameObjectEventParamVehicleRide::GsGameObjectEventParamVehicleRide()
{
	ActionType = MessageGameObject::Action::VehicleRide;
}

void GsGameObjectEventParamVehicleRide::Clear()
{
	Super::Clear();
	Passenger = NULL;
}