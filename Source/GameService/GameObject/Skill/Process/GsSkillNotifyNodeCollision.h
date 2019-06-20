// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsSkillNotifyNodeBase.h"

/**
 * 
 */
class GAMESERVICE_API FGsSkillNotifyNodeCollision : public FGsSkillNotifyNodeBase
{
	using Super = FGsSkillNotifyNodeBase;

private:
	int CollistionID;

public:
	FGsSkillNotifyNodeCollision(const FGsSkillNotifyDataBase& Data);
	virtual ~FGsSkillNotifyNodeCollision();

public:
	virtual void Process(UGsGameObjectBase* Owner) override;
	virtual void Action(UGsGameObjectBase* Owner) override;
};
