// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Data/GsSkillDataBase.h"

/**
 * FSkillActionDataBase 데이터 기반으로 스킬 액션 처리 담당
 */
class GAMESERVICE_API FGsSkillNotifyNodeBase
{
protected:
	float Rate;

public:
	FGsSkillNotifyNodeBase(const FGsSkillNotifyDataBase& Data);
	virtual ~FGsSkillNotifyNodeBase();

public:
	virtual void Process(class UGsGameObjectBase* Owner);
	virtual void Action(class UGsGameObjectBase* Owner) {}

	//생명 주기 관장
	virtual bool Update(class UGsGameObjectBase* Owner) { return false;  }

public:
	float GetRate();
};
