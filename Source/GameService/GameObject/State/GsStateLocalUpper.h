// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsStateBase.h"

/**
* Local캐릭터 상체관련 State 부모 클래스
* 진입시 AnimBlueprint 상태 전환 정보를 따로 알려주지 않는다. 
* 추후 제어에 문제가 있다면 Upper 상태 변수를 추가하여 동기화 시켜줘야 할듯...
 */
template <class tState>
class GAMESERVICE_API FGsStateUpperSingleLocal : public IGsStateBase, public TGsStateSingleton<tState>
{
public:
	virtual bool ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID) override { return true; }
	virtual void Enter(UGsGameObjectBase* Owner) override						{}
	virtual void ReEnter(UGsGameObjectBase* Owner) override						{}
	virtual void Update(UGsGameObjectBase* Owner, float Delta) override			{}
	virtual void Exit(UGsGameObjectBase* Owner) override						{}

protected:

//타이핑 실수 방지 내부 사용 메크로 정의
#define ObjectUpperStateChange(State)	ChangeState<State>(Cast<UGsGameObjectLocal>(Owner)->GetUpperFSM())
};

/**
* Local 상체 유휴 상태 클래스
* 상체 상태 클래스
*/
class GAMESERVICE_API FGsStateLocalUpperIdle : public FGsStateUpperSingleLocal<FGsStateLocalUpperIdle>
{
	typedef FGsStateUpperSingleLocal<FGsStateLocalUpperIdle> Super;

public:
	virtual uint8 GetStateID() override;
	virtual FString Name() override;

	virtual bool ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID) override;
	virtual void Enter(UGsGameObjectBase* Owner) override;
};

/**
* Local 공격 상태 클래스
* 상체 상태 클래스
*/
class GAMESERVICE_API FGsStateLocalAttack : public FGsStateUpperSingleLocal<FGsStateLocalAttack>
{
	typedef FGsStateUpperSingleLocal<FGsStateLocalAttack> Super;

public:
	virtual uint8 GetStateID() override;
	virtual FString Name() override;

	virtual bool ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID) override;
	virtual void Enter(UGsGameObjectBase* Owner) override;
	virtual void Update(UGsGameObjectBase* Owner, float Delta) override;
};
