// Fill out your copyright notice in the Description page of Project Settings.

#include "GsSkillActionNodeBase.h"
#include "GameObject/Skill/Data/GsSkillDataBase.h"

FGsSkillActionNodeBase::FGsSkillActionNodeBase(const FGsSkillActionDataBase& Data)
{
	Rate = Data.Rate;
}

FGsSkillActionNodeBase::~FGsSkillActionNodeBase()
{
}

float FGsSkillActionNodeBase::GetRate()
{
	return Rate;
}
