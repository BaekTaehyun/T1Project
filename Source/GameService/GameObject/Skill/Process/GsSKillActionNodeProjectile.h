// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsSkillActionNodeBase.h"

/**
 * 프로젝타일 타입 Action처리 클래스
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
