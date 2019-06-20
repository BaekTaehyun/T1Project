// Fill out your copyright notice in the Description page of Project Settings.

#include "GsSkillActionNodeBase.h"
#include "Engine/AssetManager.h"
#include "GameObject/Skill/Data/GsSkillDataBase.h"

FGsSkillActionNodeBase::FGsSkillActionNodeBase(const FGsSkillActionDataBase& Data) :
	Rate(Data.Rate)
{
}

FGsSkillActionNodeBase::~FGsSkillActionNodeBase()
{
}

void FGsSkillActionNodeBase::Process(class UGsGameObjectBase* Owner)
{
}

float FGsSkillActionNodeBase::GetRate()
{
	return Rate;
}
