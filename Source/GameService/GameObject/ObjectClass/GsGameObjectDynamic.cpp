// Fill out your copyright notice in the Description page of Project Settings.

#include "GsGameObjectDynamic.h"
#include "GameObject/State/GsFSMManager.h"
#include "GameObject/Parts/GsPartsBase.h"
#include "GameObject/Movement/GsMovementBase.h"

EGsGameObjectType UGsGameObjectDynamic::GetObjectType() const   { return EGsGameObjectType::Dynamic; }
AActor*		      UGsGameObjectDynamic::GetActor() const    	{ return NULL; }
FGsFSMManager*    UGsGameObjectDynamic::GetBaseFSM() const  	{ return Fsm; }
FGsPartsBase*     UGsGameObjectDynamic::GetParts() const		{ return Parts; }
FGsMovementBase*  UGsGameObjectDynamic::GetMovement() const     { return Movement; }

void UGsGameObjectDynamic::Initialize()
{
	Super::Initialize();

    SET_FLAG_TYPE(ObjectType, UGsGameObjectDynamic::GetObjectType());
}

void UGsGameObjectDynamic::Finalize()
{
	Super::Finalize();

	if (Fsm)	    { delete Fsm; }
	if (Parts)	    { delete Parts; }
    if (Movement)   { delete Movement; }
}

void UGsGameObjectDynamic::Update(float Delta)
{
	Super::Update(Delta);

	if (Fsm)        { Fsm->Update(this, Delta); }
    if (Movement)   { Movement->Update(Delta); }
}