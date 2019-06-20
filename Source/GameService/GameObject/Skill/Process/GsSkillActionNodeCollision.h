// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsSkillActionNodeBase.h"

/**
 * 
 */
class GAMESERVICE_API FGsSkillActionNodeCollision : public FGsSkillActionNodeBase
{
	using Super = FGsSkillActionNodeBase;

private:
	int CollistionID;

public:
	FGsSkillActionNodeCollision(const FGsSkillActionDataBase& Data);
	virtual ~FGsSkillActionNodeCollision();

public:
	virtual void Process(UGsGameObjectBase* Owner) override;
	virtual void Action(UGsGameObjectBase* Owner) override;
};
