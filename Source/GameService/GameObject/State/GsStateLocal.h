// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsStateBase.h"
#include "GameObject/Component/GsAnimInstanceState.h"
#include "GameObject/ActorExtend/GsLocalCharacter.h"
#include "GameObject/ObjectClass/GsGameObjectLocal.h"

template <class tState, typename tStateType>
class GAMESERVICE_API FGsStateSingleLocal : public TGsStateSingleton<tState>
{
public:
	virtual bool ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID) override
	{
		return OnProcessEvent(Owner, static_cast<tStateType>(StateID));
	}

	//애님 블루프린트에 가장 최우선으로 상태를 전송해줘야한다.
	virtual void Enter(UGsGameObjectBase* Owner) override
	{
		auto my = Cast<UGsGameObjectLocal>(Owner);
		if (auto anim = my->GetLocalCharacter()->GetAnim())
		{
			anim->ChangeState(GetStateID(), 0, GetAniRandomCount());
		}
	}
	virtual void ReEnter(UGsGameObjectBase* Owner) override				{}
	virtual void Update(UGsGameObjectBase* Owner, float Delta) override {}
	virtual void Exit(UGsGameObjectBase* Owner) override				{}

protected:
	virtual bool OnProcessEvent(UGsGameObjectBase* Owner, tStateType StateID)
	{
		return true;
	}

//타이핑 실수 방지 내부 사용 메크로 정의
#define ObjectBaseStateChange(State) ChangeState<State>(Cast<UGsGameObjectLocal>(Owner)->GetBaseFSM())
#define ObjectUpperStateChange(State) ChangeState<State>(Cast<UGsGameObjectLocal>(Owner)->GetUpperFSM())
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
* Local 스폰 상태 클래스
*/
class GAMESERVICE_API FGsStateLocalSpawn : public FGsStateSingleLocal<FGsStateLocalSpawn, EGsStateBase>
{
	typedef FGsStateSingleLocal Super;	typedef FGsStateSingleLocal<FGsStateLocalSpawn, EGsStateBase> Super;

public:
	virtual uint8 GetStateID() override;
	virtual FString Name() override;
	virtual int GetAniRandomCount() override;

	virtual void Enter(UGsGameObjectBase* Owner) override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID) override;
};

/**
* Local 유휴 상태 클래스
*/
class GAMESERVICE_API FGsStateLocalIdle : public FGsStateSingleLocal<FGsStateLocalIdle, EGsStateBase>
{
	
	typedef FGsStateSingleLocal<FGsStateLocalIdle, EGsStateBase> Super;

public:
	virtual uint8 GetStateID() override;
	virtual FString Name() override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID) override;
};


/**
* Local 무브 관련 베이스 상태 클래스
* 캐릭터는 전방, 측면, 후방이동으로 분류
*/
template<class tState>
class GAMESERVICE_API FGsStateLocalMoveBase : public FGsStateSingleLocal<tState, EGsStateBase>
{
	typedef FGsStateSingleLocal<tState, EGsStateBase> Super;

public:
	virtual void Update(UGsGameObjectBase* Owner, float Delta) override
	{
		Super::Update(Owner, Delta);

		auto my = Cast<UGsGameObjectLocal>(Owner);
		my->GetMovement()->Update(Delta);
	}

protected:
};

/**
* Local 전방 이동 상태 클래스
*/
class GAMESERVICE_API FGsStateLocalForwardWalk : public FGsStateLocalMoveBase<FGsStateLocalForwardWalk>
{
	
	typedef FGsStateLocalMoveBase<FGsStateLocalForwardWalk> Super;

public:
	virtual uint8 GetStateID() override;
	virtual FString Name() override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID) override;
};

/**
* Local 후방 이동 상태 클래스
*/
class GAMESERVICE_API FGsStateLocalBackwardWalk : public FGsStateLocalMoveBase<FGsStateLocalBackwardWalk>
{
	
	typedef FGsStateLocalMoveBase<FGsStateLocalBackwardWalk> Super;

public:
	virtual uint8 GetStateID() override;
	virtual FString Name() override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID) override;
};

/**
* Local 측면 이동 상태 클래스
*/
class GAMESERVICE_API FGsStateLocalSideWalk : public FGsStateLocalMoveBase<FGsStateLocalSideWalk>
{
	
	typedef FGsStateLocalMoveBase<FGsStateLocalSideWalk> Super;

public:
	virtual uint8 GetStateID() override;
	virtual FString Name() override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID) override;
};

/**
* Local 달리기 이동 상태 클래스
*/
class GAMESERVICE_API FGsStateLocalRun : public FGsStateLocalMoveBase<FGsStateLocalRun>
{
	typedef FGsStateLocalMoveBase<FGsStateLocalRun> Super;

public:
	virtual uint8 GetStateID() override;
	virtual FString Name() override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID) override;
};

class GAMESERVICE_API FGsStateLocalRide : public FGsStateSingleLocal<FGsStateLocalRide, EGsStateBase>
{
	
	typedef FGsStateSingleLocal<FGsStateLocalRide, EGsStateBase> Super;

public:
	virtual uint8 GetStateID() override;
	virtual FString Name() override;

	virtual void Enter(UGsGameObjectBase* Owner) override;
	virtual void Exit(UGsGameObjectBase* Owner) override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID) override;
};

/**
* Local 상체 유휴 상태 클래스
* 상체 상태 클래스
*/
class GAMESERVICE_API FGsStateLocalUpperIdle : public FGsStateSingleLocal<FGsStateLocalUpperIdle, EGsStateUpperBase>
{
	
	typedef FGsStateSingleLocal<FGsStateLocalUpperIdle, EGsStateUpperBase> Super;

public:
	virtual uint8 GetStateID() override;
	virtual FString Name() override;

	virtual void Enter(UGsGameObjectBase* Owner) override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectBase* Owner, EGsStateUpperBase StateID) override;
};

/**
* Local 공격 상태 클래스
* 상체 상태 클래스
*/
class GAMESERVICE_API FGsStateLocalAttack : public FGsStateSingleLocal<FGsStateLocalAttack, EGsStateUpperBase>
{
	
	typedef FGsStateSingleLocal<FGsStateLocalAttack, EGsStateUpperBase> Super;

public:
	virtual uint8 GetStateID() override;
	virtual FString Name() override;

	virtual void Enter(UGsGameObjectBase* Owner) override;
	virtual void Update(UGsGameObjectBase* Owner, float Delta) override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectBase* Owner, EGsStateUpperBase StateID) override;
};
