// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameObject/Movement/GsMovementBase.h"
#include "GameObject/ObjectClass/GsGameObjectLocal.h"

/**
 * 
 */
class GAMESERVICE_API FGsMovementLocal : public FGsMovementBase
{
	typedef FGsMovementBase Super;

public:
    virtual void Initialize(UGsGameObjectBase* Owner) override;
	virtual void Finalize() override;
    virtual void Update(float Delta) override;

public:
    inline UCharacterMovementComponent* GetCharMovement() const { return CharMovement; }

protected:
    virtual void OnStop() override;
    virtual void OnMove() override;

protected:
    UGsGameObjectLocal* Local;
    UCharacterMovementComponent* CharMovement;
};
