// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsStateBase.h"
#include "GameObject/Component/GsAnimInstanceState.h"
#include "GameObject/ActorExtend/GsNpcPawn.h"
#include "GameObject/ObjectClass/GsGameObjectNonPlayer.h"

template <class tState, typename tStateType>
class GAMESERVICE_API FGsStateSingleNpc : public FGsStateTargetBase<UGsGameObjectNonPlayer, tState, tStateType>
{
protected:
	virtual bool OnProcessEvent(UGsGameObjectNonPlayer* Owner, tStateType StateID) override
	{
		return true;
	}

	//애님 블루프린트에 가장 최우선으로 상태를 전송해줘야한다.
	virtual void OnEnter(UGsGameObjectNonPlayer* Owner) override;

	virtual void OnReEnter(UGsGameObjectNonPlayer* Owner) override
	{
	}
	virtual void OnUpdate(UGsGameObjectNonPlayer* Owner, float Delta) override
	{
	}
	virtual void OnExit(UGsGameObjectNonPlayer* Owner) override
	{
	}
};

template< class tState, typename tStateType >
void FGsStateSingleNpc<tState, tStateType >::OnEnter(UGsGameObjectNonPlayer* Owner)
{
	if (auto anim = Owner->GetNpcPawn()->GetAnim())
	{
		anim->ChangeState(GetStateID(), 0, GetAniRandomCount());
	}
}

class GAMESERVICE_API FGsStateNpcSpawn : public FGsStateSingleNpc<FGsStateNpcSpawn, EGsStateBase>
{
	typedef FGsStateSingleNpc<FGsStateNpcSpawn, EGsStateBase> Super;

public:
    virtual int GetStateID() override;
    virtual FString Name() override;
    //virtual int GetAniRandomCount() override;
  
protected:
	virtual bool OnProcessEvent(UGsGameObjectNonPlayer* Owner, EGsStateBase StateID) override;
    virtual void OnEnter(UGsGameObjectNonPlayer* Owner) override;
};

class GAMESERVICE_API FGsStateNpcIdle : public FGsStateSingleNpc<FGsStateNpcIdle, EGsStateBase>
{
	typedef FGsStateSingleNpc<FGsStateNpcIdle, EGsStateBase> Super;

public:
	virtual int GetStateID() override;
	virtual FString Name() override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectNonPlayer* Owner, EGsStateBase StateID) override;
	virtual void OnEnter(UGsGameObjectNonPlayer* Owner) override;
};

// 무브 관련 베이스 스테이트
template<class tState>
class GAMESERVICE_API FGsStateNpcMoveBase : public FGsStateSingleNpc<tState, EGsStateBase>
{
	typedef FGsStateSingleNpc<tState, EGsStateBase> Super;

protected:
	virtual void OnUpdate(UGsGameObjectNonPlayer* Owner, float Delta) override
	{
		Super::OnUpdate(Owner, Delta);

		Owner->GetMovement()->Update(Delta);
	}
};

class GAMESERVICE_API FGsStateNpcWalk : public FGsStateNpcMoveBase<FGsStateNpcWalk>
{
	typedef FGsStateNpcMoveBase<FGsStateNpcWalk> Super;

public:
    virtual int GetStateID() override;
    virtual FString Name() override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectNonPlayer* Owner, EGsStateBase StateID) override;
    virtual void OnEnter(UGsGameObjectNonPlayer* Owner) override;
};

class GAMESERVICE_API FGsStateNpcBeaten : public FGsStateSingleNpc<FGsStateNpcBeaten, EGsStateBase>
{
	typedef FGsStateSingleNpc<FGsStateNpcBeaten, EGsStateBase> Super;

public:
	virtual int GetStateID() override;
	virtual FString Name() override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectNonPlayer* Owner, EGsStateBase StateID) override;
	virtual void OnEnter(UGsGameObjectNonPlayer* Owner) override;
};
