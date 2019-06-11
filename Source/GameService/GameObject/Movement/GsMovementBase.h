// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameObject/Define/GsGameObjectDefine.h"
#include "GameObject/ObjectClass/GsGameObjectBase.h"

/**
 * 오브젝트 이동 처리 관련 Base 클래스
 * 아직 Data 구조가 확립되어있지 않기 때문에 테스트 코드 관련 함수,변수들이 많이 존재함
 */
class GAMESERVICE_API FGsMovementBase
{
public:
	FGsMovementBase();
	virtual ~FGsMovementBase();

    virtual void Initialize(UGsGameObjectBase* Owner);
	virtual void Finalize();
    virtual void Update(float Delta);

public:
    virtual bool IsMove();

    void Stop();
    void Move();
    void Move(FVector Dir);

	virtual void Move(FVector Dir, float Speed);

protected:
    virtual void OnStop();
    virtual void OnMove();

public:
    FORCEINLINE void SetMoveSpeed(float Val)			{ MoveSpeed = Val; }
    FORCEINLINE void SetDirection(const FVector& Val)	{ Direction = Val; }

protected:
    UGsGameObjectBase* Owner;
    UPathFollowingComponent* PathComponent;

    //
    FVector Direction;
    float MoveSpeed;
    //
};
