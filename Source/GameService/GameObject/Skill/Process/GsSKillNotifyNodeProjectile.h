// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsSkillNotifyNodeBase.h"

/**
 * 프로젝타일 타입 Action처리 클래스
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
