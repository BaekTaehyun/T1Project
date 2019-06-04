// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsStateBase.h"
#include "GameObject/Component/GsAnimInstanceState.h"
#include "GameObject/ActorExtend/GsLocalCharacter.h"
#include "GameObject/ObjectClass/GsGameObjectLocal.h"

template <class tState, typename tStateType>
class GAMESERVICE_API FGsStateSingleLocal : public FGsStateTargetBase<UGsGameObjectLocal, tState, tStateType>
{
protected:
	virtual bool OnProcessEvent(UGsGameObjectLocal* Owner, tStateType StateID) override
	{
		return true;
	}

	//애님 블루프린트에 가장 최우선으로 상태를 전송해줘야한다.
	virtual void OnEnter(UGsGameObjectLocal* Owner) override;

	virtual void OnReEnter(UGsGameObjectLocal* Owner) override
	{
	}
	virtual void OnUpdate(UGsGameObjectLocal* Owner, float Delta) override
	{
	}
	virtual void OnExit(UGsGameObjectLocal* Owner) override
	{
	}
};

template< class tState, typename tStateType >
void FGsStateSingleLocal<tState, tStateType >::OnEnter(UGsGameObjectLocal* Owner)
{
	if (auto anim = Owner->GetLocalCharacter()->GetAnim())
	{
		anim->ChangeState(GetStateID(), 0, GetAniRandomCount());
	}
}

/**
 * 
 */
class GAMESERVICE_API FGsStateSpawn : public FGsStateSingleLocal<FGsStateSpawn, EGsStateBase>
{
	typedef FGsStateSingleLocal<FGsStateSpawn, EGsStateBase> Super;

public:
	virtual int GetStateID() override;
	virtual FString Name() override;
	virtual int GetAniRandomCount() override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateBase StateID) override;
	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
};

class GAMESERVICE_API FGsStateIdle : public FGsStateSingleLocal<FGsStateIdle, EGsStateBase>
{
	typedef FGsStateSingleLocal<FGsStateIdle, EGsStateBase> Super;

public:
	virtual int GetStateID() override;
	virtual FString Name() override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateBase StateID) override;
	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
};

//캐릭터는 전방 이동과 후방이동으로 분류
//무브 관련 베이스 스테이트
template<class tState>
class GAMESERVICE_API FGsStateMoveBase : public FGsStateSingleLocal<tState, EGsStateBase>
{
	typedef FGsStateSingleLocal<tState, EGsStateBase> Super;

protected:
	virtual void OnUpdate(UGsGameObjectLocal* Owner, float Delta) override
	{
		Super::OnUpdate(Owner, Delta);

		Owner->GetMovement()->Update(Delta);
	}
};

class GAMESERVICE_API FGsStateForwardWalk : public FGsStateMoveBase<FGsStateForwardWalk>
{
	typedef FGsStateMoveBase<FGsStateForwardWalk> Super;

public:
	virtual int GetStateID() override;
	virtual FString Name() override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateBase StateID) override;
	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
	virtual void OnUpdate(UGsGameObjectLocal* Owner, float Delta) override;
};

class GAMESERVICE_API FGsStateBackwardWalk : public FGsStateMoveBase<FGsStateBackwardWalk>
{
	typedef FGsStateMoveBase<FGsStateBackwardWalk> Super;

public:
	virtual int GetStateID() override;
	virtual FString Name() override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateBase StateID) override;
	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
	virtual void OnUpdate(UGsGameObjectLocal* Owner, float Delta) override;
};

class GAMESERVICE_API FGsStateSideWalk : public FGsStateMoveBase<FGsStateSideWalk>
{
	typedef FGsStateMoveBase<FGsStateSideWalk> Super;

public:
	virtual int GetStateID() override;
	virtual FString Name() override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateBase StateID) override;
	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
	virtual void OnUpdate(UGsGameObjectLocal* Owner, float Delta) override;
};

class GAMESERVICE_API FGsStateRun : public FGsStateMoveBase<FGsStateRun>
{
	typedef FGsStateMoveBase Super;

public:
	virtual int GetStateID() override;
	virtual FString Name() override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateBase StateID) override;
	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
	virtual void OnUpdate(UGsGameObjectLocal* Owner, float Delta) override;
};

class GAMESERVICE_API FGsStateRide : public FGsStateSingleLocal<FGsStateRide, EGsStateBase>
{
	typedef FGsStateSingleLocal<FGsStateRide, EGsStateBase> Super;

public:
	virtual int GetStateID() override;
	virtual FString Name() override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateBase StateID) override;
	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
	virtual void OnUpdate(UGsGameObjectLocal* Owner, float Delta) override;
	virtual void OnExit(UGsGameObjectLocal* Owner) override;
};

//Uppper
class GAMESERVICE_API FGsStateUpperIdle : public FGsStateSingleLocal<FGsStateUpperIdle, EGsStateUpperBase>
{
	typedef FGsStateSingleLocal<FGsStateUpperIdle, EGsStateUpperBase> Super;

public:
	virtual int GetStateID() override;
	virtual FString Name() override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateUpperBase StateID) override;
	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
};

class GAMESERVICE_API FGsStateAttack : public FGsStateSingleLocal<FGsStateAttack, EGsStateUpperBase>
{
	typedef FGsStateSingleLocal<FGsStateAttack, EGsStateUpperBase> Super;

public:
	virtual int GetStateID() override;
	virtual FString Name() override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateUpperBase StateID) override;
	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
	virtual void OnUpdate(UGsGameObjectLocal* Owner, float Delta) override;
};
