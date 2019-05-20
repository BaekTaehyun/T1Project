// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsStateBase.h"
#include "GameObject/ActorExtend/GsLocalCharacter.h"
#include "GameObject/ObjectClass/GsGameObjectLocal.h"

template <class tState, typename tStateType>
class GAMESERVICE_API FGsStateSingleLocal : public FGsStateTargetBase<UGsGameObjectLocal, tState, tStateType>
{
protected:
	//하위 Class들에서 define처리를 해야하지만 너무 많으므로... ㅠ
	typedef FGsStateSingleLocal Super;

	virtual bool OnProcessEvent(UGsGameObjectLocal* Owner, tStateType StateID) override
	{
		return true;
	}

	//애님 블루프린트에 가장 최우선으로 상태를 전송해줘야한다.
	virtual void OnEnter(UGsGameObjectLocal* Owner) override
	{
		if (auto anim = Owner->GetLocal()->GetAnim())
		{
			anim->ChangeState(GetStateID(), 0, GetAniRandomCount());
		}
	}

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

/**
 * 
 */
class GAMESERVICE_API FGsStateSpawn : public FGsStateSingleLocal<FGsStateSpawn, EGsStateBase>
{
public:
	virtual int GetStateID() override;
	virtual FString Name() override;
	virtual int GetAniRandomCount() override;

	virtual bool OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateBase StateID) override;
	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
};

class GAMESERVICE_API FGsStateIdle : public FGsStateSingleLocal<FGsStateIdle, EGsStateBase>
{
public:
	virtual int GetStateID() override;
	virtual FString Name() override;

	virtual bool OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateBase StateID) override;
	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
};

//캐릭터는 전방 이동과 후방이동으로 분류
class GAMESERVICE_API FGsStateForwardWalk : public FGsStateSingleLocal<FGsStateForwardWalk, EGsStateBase>
{
public:
	virtual int GetStateID() override;
	virtual FString Name() override;

	virtual bool OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateBase StateID) override;
	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
	virtual void OnUpdate(UGsGameObjectLocal* Owner, float Delta) override;
};

class GAMESERVICE_API FGsStateBackwardWalk : public FGsStateSingleLocal<FGsStateBackwardWalk, EGsStateBase>
{
public:
	virtual int GetStateID() override;
	virtual FString Name() override;

	virtual bool OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateBase StateID) override;
	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
	virtual void OnUpdate(UGsGameObjectLocal* Owner, float Delta) override;
};

class GAMESERVICE_API FGsStateSideWalk : public FGsStateSingleLocal<FGsStateSideWalk, EGsStateBase>
{
public:
	virtual int GetStateID() override;
	virtual FString Name() override;

	virtual bool OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateBase StateID) override;
	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
	virtual void OnUpdate(UGsGameObjectLocal* Owner, float Delta) override;
};

class GAMESERVICE_API FGsStateRun : public FGsStateSingleLocal<FGsStateRun, EGsStateBase>
{
public:
	virtual int GetStateID() override;
	virtual FString Name() override;

	//virtual bool IsChange(int StateID) override;
	virtual bool OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateBase StateID) override;
	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
	virtual void OnUpdate(UGsGameObjectLocal* Owner, float Delta) override;
};


//Uppper
class GAMESERVICE_API FGsStateUpperIdle : public FGsStateSingleLocal<FGsStateUpperIdle, EGsStateUpperBase>
{
public:
	virtual int GetStateID() override;
	virtual FString Name() override;

	virtual bool OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateUpperBase StateID) override;
	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
};

class GAMESERVICE_API FGsStateAttack : public FGsStateSingleLocal<FGsStateAttack, EGsStateUpperBase>
{
public:
	virtual int GetStateID() override;
	virtual FString Name() override;

	virtual bool OnProcessEvent(UGsGameObjectLocal* Owner, EGsStateUpperBase StateID) override;
	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
	virtual void OnUpdate(UGsGameObjectLocal* Owner, float Delta) override;
};
