// Fill out your copyright notice in the Description page of Project Settings.

#include "GsSkillNotifyNodeBase.h"
#include "Engine/AssetManager.h"
#include "GameObject/Skill/Data/GsSkillDataBase.h"

FGsSkillNotifyNodeBase::FGsSkillNotifyNodeBase(const FGsSkillNotifyDataBase& Data) :
	Rate(Data.Rate)
{
}

FGsSkillNotifyNodeBase::~FGsSkillNotifyNodeBase()
{
}

void FGsSkillNotifyNodeBase::Process(class UGsGameObjectBase* Owner)
{
}

float FGsSkillNotifyNodeBase::GetRate()
{
	return Rate;
}
