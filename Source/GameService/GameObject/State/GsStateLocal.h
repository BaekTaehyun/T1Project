// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsStateBase.h"
#include "GameObject/Component/GsAnimInstanceState.h"
#include "GameObject/Movement/GsMovementBase.h"

/**
* Local캐릭터 Base State 부모 클래스
* 진입시 AnimBlueprint 상태 전환 변수를 동기화 시켜주는 공통 로직을 포함
* @see : void Enter(UGsGameObjectBase* Owner)
*/
template <class tState>
class GAMESERVICE_API FGsStateBaseSingleLocal : public IGsStateBase, public TGsStateSingleton<tState>
{
public:
	virtual bool ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID) override;
	//애님 블루프린트에 상태 변경을 전송
	virtual void Enter(UGsGameObjectBase* Owner) override;
	
	virtual void ReEnter(UGsGameObjectBase* Owner) override				{}
	virtual void Update(UGsGameObjectBase* Owner, float Delta) override {}
	virtual void Exit(UGsGameObjectBase* Owner) override				{}

protected:

//타이핑 실수 방지 내부 사용 메크로 정의
#define ObjectBaseStateChange(State)	ChangeState<State>(Cast<UGsGameObjectLocal>(Owner)->GetBaseFSM())
};


/**
* Local 스폰 상태 클래스
*/
class GAMESERVICE_API FGsStateLocalSpawn : public FGsStateBaseSingleLocal<FGsStateLocalSpawn>
{
	typedef FGsStateBaseSingleLocal<FGsStateLocalSpawn> Super;

public:
	virtual uint8 GetStateID() override;
	virtual FString Name() override;
	virtual int GetAniRandomCount() override;

	virtual bool ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID) override;
	virtual void Enter(UGsGameObjectBase* Owner) override;
};

/**
* Local 유휴 상태 클래스
*/
class GAMESERVICE_API FGsStateLocalIdle : public FGsStateBaseSingleLocal<FGsStateLocalIdle>
{
	typedef FGsStateBaseSingleLocal<FGsStateLocalIdle> Super;

public:
	virtual uint8 GetStateID() override;
	virtual FString Name() override;

	virtual bool ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID) override;
};


/**
* Local 무브 관련 베이스 상태 클래스
* 캐릭터는 전방, 측면, 후방이동으로 분류
*/
template<class tState>
class GAMESERVICE_API FGsStateLocalMoveBase : public FGsStateBaseSingleLocal<tState>
{
	typedef FGsStateBaseSingleLocal<tState> Super;

public:
	virtual void Enter(UGsGameObjectBase* Owner) override;
	virtual void Update(UGsGameObjectBase* Owner, float Delta) override;
	virtual void Exit(UGsGameObjectBase* Owner) override;
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

	virtual bool ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID) override;
	virtual void Update(UGsGameObjectBase* Owner, float Delta) override;
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

	virtual bool ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID) override;
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

	virtual bool ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID) override;
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

	virtual bool ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID) override;
};

class GAMESERVICE_API FGsStateLocalRide : public FGsStateBaseSingleLocal<FGsStateLocalRide>
{
	typedef FGsStateBaseSingleLocal<FGsStateLocalRide> Super;

public:
	virtual uint8 GetStateID() override;
	virtual FString Name() override;

	virtual bool ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID) override;
	virtual void Enter(UGsGameObjectBase* Owner) override;
	virtual void Exit(UGsGameObjectBase* Owner) override;
};



