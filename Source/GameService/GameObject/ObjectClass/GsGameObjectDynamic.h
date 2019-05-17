// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsGameObjectBase.h"
//#include "GameObject/State/FSMManager.h" //헤더 선언으로 error 02143발생  추후 확인해볼것
#include "GsGameObjectDynamic.generated.h"

/**
 * 운동 능력이 있는 오브젝트 Base클래스
 * FSM, Parts, Movment 등의 기본 기능 처리
 */
UCLASS()
class GAMESERVICE_API UGsGameObjectDynamic : public UGsGameObjectBase
{
	GENERATED_BODY()
	
public:
	virtual void Initialize() override;
	virtual void Finalize() override;
    virtual void Update(float Delta) override;

    virtual EGsGameObjectType GetObjectType() const override;
	virtual AActor* GetActor() const override;

public:	
	virtual class FGsPartsBase* GetParts() const;
	virtual class FGsFSMManager* GetBaseFSM() const;
    virtual class FGsMovementBase* GetMovement() const;

protected:
	class FGsFSMManager* Fsm;
	class FGsPartsBase* Parts;
    class FGsMovementBase* Movement;
};
