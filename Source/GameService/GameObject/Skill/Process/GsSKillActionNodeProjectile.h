// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsSkillActionNodeBase.h"

/**
 * 
 */
class GAMESERVICE_API FGsSKillActionNodeProjectile : public FGsSkillActionNodeBase
{
public:
	FGsSKillActionNodeProjectile(const FGsSkillActionDataBase& Data);

public:
	virtual void Process(UGsGameObjectBase* Owner) override;
	virtual void Action(UGsGameObjectBase* Owner) override;
};
