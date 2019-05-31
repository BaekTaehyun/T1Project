// Fill out your copyright notice in the Description page of Project Settings.

#include "GsGameObjectPlayer.h"
#include "GameObject/State/GsFSMManager.h"
#include "GameObject/Skill/GsSkillBase.h"
#include "GameObject/ObjectClass/GsGameObjectWheelVehicle.h"

EGsGameObjectType UGsGameObjectPlayer::GetObjectType() const    { return EGsGameObjectType::Player; }
AActor*		      UGsGameObjectPlayer::GetActor() const		    { return NULL; }
FGsFSMManager*    UGsGameObjectPlayer::GetUpperFSM() const	    { return UpperFsm; }
FGsSkillBase*     UGsGameObjectPlayer::GetSkill() const		    { return Skill; }
UGsGameObjectWheelVehicle* UGsGameObjectPlayer::GetVehicle() const { return Vehicle; }

void UGsGameObjectPlayer::SetVehicle(class UGsGameObjectWheelVehicle* vehicle) { Vehicle = vehicle; }

void UGsGameObjectPlayer::Initialize()
{
	Super::Initialize();

	SET_FLAG_TYPE(ObjectType, UGsGameObjectPlayer::GetObjectType());
}

void UGsGameObjectPlayer::Finalize()
{
	Super::Finalize();

	if (UpperFsm)	
	{ 
		UpperFsm->Finalize();
		delete UpperFsm; 
	}
	if (Skill)		
	{ 
		Skill->Finalize();
		delete Skill; 
	}
}

void UGsGameObjectPlayer::Update(float Delta)
{
	Super::Update(Delta);

	if (UpperFsm)	{ UpperFsm->Update(this, Delta); }
	if (Skill)		{ Skill->Update(Delta); }
}