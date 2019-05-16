// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsGameObjectDynamic.h"
#include "GsGameObjectPlayer.generated.h"

/**
 * Player류에 해당 하는 base클래스
 * SKill, FSM의 상태는 두단계이상으로 분류
 */
UCLASS()
class GAMESERVICE_API UGsGameObjectPlayer : public UGsGameObjectDynamic
{
	GENERATED_BODY()
	
public:
	virtual void Initialize() override;
	virtual void DeInitialize() override;
    virtual void Update(float Delta) override;

public:    
    virtual EGsGameObjectType GetObjectType() const override;
	virtual AActor* GetActor() const override;
	virtual class FGsFSMManager* GetUpperFSM() const;
	virtual class FGsSkillBase* GetSkill() const;

protected:
	FGsFSMManager* UpperFsm;          //상체 Fsm
	FGsSkillBase* Skill;
};
