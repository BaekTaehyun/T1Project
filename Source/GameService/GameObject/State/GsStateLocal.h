// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsStateBase.h"
#include "GameObject/ActorExtend/GsLocalCharacter.h"
#include "GameObject/ObjectClass/GsGameObjectLocal.h"

template <typename T>
class GAMESERVICE_API FGsStateSingleLocal : public FGsStateTargetBase<UGsGameObjectLocal, T>
{
protected:
	typedef FGsStateSingleLocal Super;

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
class GAMESERVICE_API FGsStateSpawn : public FGsStateSingleLocal<FGsStateSpawn>
{
public:
	virtual int GetStateID() override;
	virtual FString Name() override;
	virtual int GetAniRandomCount() override;

	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
};

class GAMESERVICE_API FGsStateIdle : public FGsStateSingleLocal<FGsStateIdle>
{
public:
	virtual int GetStateID() override;
	virtual FString Name() override;

	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
};

//캐릭터는 전방 이동과 후방이동으로 분류
class GAMESERVICE_API FGsStateForwardWalk : public FGsStateSingleLocal<FGsStateForwardWalk>
{
public:
	virtual int GetStateID() override;
	virtual FString Name() override;

	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
	virtual void OnUpdate(UGsGameObjectLocal* Owner, float Delta) override;
};

class GAMESERVICE_API FGsStateBackwardWalk : public FGsStateSingleLocal<FGsStateBackwardWalk>
{
public:
	virtual int GetStateID() override;
	virtual FString Name() override;

	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
	virtual void OnUpdate(UGsGameObjectLocal* Owner, float Delta) override;
};

class GAMESERVICE_API FGsStateSideWalk : public FGsStateSingleLocal<FGsStateSideWalk>
{
public:
	virtual int GetStateID() override;
	virtual FString Name() override;

	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
	virtual void OnUpdate(UGsGameObjectLocal* Owner, float Delta) override;
};

class GAMESERVICE_API FGsStateRun : public FGsStateSingleLocal<FGsStateRun>
{
public:
	virtual int GetStateID() override;
	virtual FString Name() override;

	virtual bool IsChange(int StateID) override;
	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
	virtual void OnUpdate(UGsGameObjectLocal* Owner, float Delta) override;
};


//Uppper
class GAMESERVICE_API FGsStateUpperIdle : public FGsStateSingleLocal<FGsStateUpperIdle>
{
public:
	virtual int GetStateID() override;
	virtual FString Name() override;

	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
};

class GAMESERVICE_API FGsStateAttack : public FGsStateSingleLocal<FGsStateAttack>
{
public:
	virtual int GetStateID() override;
	virtual FString Name() override;

	virtual void OnEnter(UGsGameObjectLocal* Owner) override;
	virtual void OnUpdate(UGsGameObjectLocal* Owner, float Delta) override;
};
