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
public:
    virtual void Initialize(UGsGameObjectBase* Owner);
    virtual void DeInitialize();
    virtual void Update(float Delta);

public:
    inline UCharacterMovementComponent* GetCharMovement() const { return CharMovement; }

protected:
    virtual void OnStop() override;
    virtual void OnMove() override;

protected:
    UGsGameObjectLocal* Local;
    UCharacterMovementComponent* CharMovement;
};
