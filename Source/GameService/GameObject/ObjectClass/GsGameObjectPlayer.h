// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsGameObjectDynamic.h"
#include "GsGameObjectPlayer.generated.h"

class FGsFSMManager;
class FGsSkillBase;
class UGsGameObjectWheelVehicle;

/**
 * Player류에 해당 하는 base클래스
 * SKill 기능 추가, FSM의 상태는 두단계이상으로 분류 가능
 * 탈것 타입은 이 클래스를 상속받은 클래스들로만 구성(임의) @see : UGsGameObjectWheelVehicle* Vehicle;
 */
UCLASS()
class GAMESERVICE_API UGsGameObjectPlayer : public UGsGameObjectDynamic
{
	GENERATED_BODY()
	
public:
	virtual ~UGsGameObjectPlayer();

	virtual void Initialize() override;
	virtual void Finalize() override;
    virtual void Update(float Delta) override;

public:    
    virtual EGsGameObjectType GetObjectType() const override;
	virtual AActor* GetActor() const override;
	virtual FGsFSMManager* GetUpperFSM() const;
	virtual FGsSkillBase* GetSkill() const;
	UGsGameObjectWheelVehicle* GetVehicle() const;
	void SetVehicle(UGsGameObjectWheelVehicle* Vehicle);

protected:
	FGsFSMManager* UpperFsm;          //상체 Fsm
	FGsSkillBase* Skill;

	//탈것 정보
	UGsGameObjectWheelVehicle* Vehicle;
};
