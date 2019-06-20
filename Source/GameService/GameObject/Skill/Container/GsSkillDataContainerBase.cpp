// Fill out your copyright notice in the Description page of Project Settings.

#include "GsSkillDataContainerBase.h"

const FGsSkillDataBase* UGsSkillDataContainerBase::FindData(int ID)
{
	return GetSkillData().FindByPredicate([=](const FGsSkillDataBase& el)
		{
			return el.ID == ID;
		});
}

const FGsSkillNotifyCollisionData* UGsSkillNotifyCollisionDataContainer::FindData(int ID)
{
	return GetCollisionData().FindByPredicate([=](const FGsSkillNotifyCollisionData& el)
		{
			return el.ID == ID;
		});
}