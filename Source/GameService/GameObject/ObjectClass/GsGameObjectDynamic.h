// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsGameObjectBase.h"
#include "GsGameObjectDynamic.generated.h"

class FGsPartsBase;
class FGsFSMManager;
class FGsMovementBase;

/**
 * 운동 능력이 있는 오브젝트 Base클래스
 * FSM, Parts, Movment 등의 기능 탑제
 */
UCLASS()
class GAMESERVICE_API UGsGameObjectDynamic : public UGsGameObjectBase
{
	GENERATED_BODY()
	
public:
	virtual ~UGsGameObjectDynamic();

	virtual void Initialize() override;
	virtual void Finalize() override;
    virtual void Update(float Delta) override;

    virtual EGsGameObjectType GetObjectType() const override;
	virtual AActor* GetActor() const override;

public:	
	virtual FGsPartsBase* GetParts() const;
	virtual FGsFSMManager* GetBaseFSM() const;
    virtual FGsMovementBase* GetMovement() const;

protected:
	FGsFSMManager* Fsm;
	FGsPartsBase* Parts;
    FGsMovementBase* Movement;
};
