// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsSkillNotifyNodeBase.h"

/**
 * ������Ÿ�� Ÿ�� Actionó�� Ŭ����
 * 
 */
class GAMESERVICE_API FGsSKillNotifyNodeProjectile : public FGsSkillNotifyNodeBase
{
	using Super = FGsSkillNotifyNodeBase;

public:
	FGsSKillNotifyNodeProjectile(const FGsSkillNotifyDataBase& Data);

public:
	virtual void Process(UGsGameObjectBase* Owner) override;
	virtual void Action(UGsGameObjectBase* Owner) override;
};
