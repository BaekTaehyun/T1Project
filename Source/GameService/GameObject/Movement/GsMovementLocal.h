// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameObject/Movement/GsMovementBase.h"
#include "GameObject/ObjectClass/GsGameObjectLocal.h"

/**
 * 내 Player 이동 처리 클래스
 */
class GAMESERVICE_API FGsMovementLocal : public FGsMovementBase
{
	typedef FGsMovementBase Super;

public:
	FGsMovementLocal();
	virtual ~FGsMovementLocal();

    virtual void Initialize(UGsGameObjectBase* Owner) override;
	virtual void Finalize() override;
    virtual void Update(float Delta) override;

public:
	virtual bool IsMove() override;
	float GetRateAccelerator();

    inline UCharacterMovementComponent* GetCharMovement() const { return CharMovement; }

protected:
    virtual void OnStop() override;
    virtual void OnMove() override;

protected:
    UGsGameObjectLocal* Local;
    UCharacterMovementComponent* CharMovement;

	//Run처리 관련 임시 생성 변수(테스트용)
	float RateAccelerator;
};
