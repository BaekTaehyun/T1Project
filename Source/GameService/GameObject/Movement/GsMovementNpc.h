// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObject/Movement/GsMovementBase.h"
#include "GameFramework/PawnMovementComponent.h"

class UGsGameObjectNonPlayer;

/**
 * 
 */
class GAMESERVICE_API FGsMovementNpc : public FGsMovementBase
{
	typedef FGsMovementBase Super;

public:
	FGsMovementNpc();
	virtual ~FGsMovementNpc();

    virtual void Initialize(UGsGameObjectBase* Owner) override;
	virtual void Finalize() override;
    virtual void Update(float Delta) override;

protected:
    void UpdateState(EPathFollowingStatus::Type Type);

private:
    void CallbackMove(UPathFollowingComponent* Path, FVector& Pos);
    void CallbackFinish(FAIRequestID ID, const FPathFollowingResult& Result);

protected:
    UGsGameObjectNonPlayer* Npc;
    UPawnMovementComponent* PawnMovement;
    
    //이동 상태 동기화 관리 변수
    //[Todo] 더좋은 방법을 찾아봐야 할듯
    EPathFollowingStatus::Type MoveStatus;
};
