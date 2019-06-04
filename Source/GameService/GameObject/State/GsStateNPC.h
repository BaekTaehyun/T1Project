// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsStateBase.h"
#include "GameObject/Component/GsAnimInstanceState.h"
#include "GameObject/ActorExtend/GsNpcPawn.h"
#include "GameObject/ObjectClass/GsGameObjectNonPlayer.h"
#include "GameObject/Movement/GsMovementBase.h"


/**
* NPC 관련 상태들은 상하체 분리 구조가 아닌걸 가정한다.
* 
*/
template <class tState>
class GAMESERVICE_API FGsStateSingleNpc : public TGsStateSingleton<tState>
{
public:
	virtual bool ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID) override
	{
		return OnProcessEvent(Owner, static_cast<EGsStateBase>(StateID));
	}

	//애님 블루프린트에 가장 최우선으로 상태를 전송해줘야한다.
	virtual void Enter(UGsGameObjectBase* Owner) override
	{
		auto npc = Cast<UGsGameObjectNonPlayer>(Owner);
		if (auto anim = npc->GetNpcPawn()->GetAnim())
		{
			anim->ChangeState(GetStateID(), 0, GetAniRandomCount());
		}
	}
	virtual void ReEnter(UGsGameObjectBase* Owner) override				{}
	virtual void Update(UGsGameObjectBase* Owner, float Delta) override {}
	virtual void Exit(UGsGameObjectBase* Owner) override				{}

protected:
	virtual bool OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID)
	{
		return true;
	}
};

/**
* NPC 스폰 상태 클래스
*/
class GAMESERVICE_API FGsStateNpcSpawn : public FGsStateSingleNpc<FGsStateNpcSpawn>
{
	typedef FGsStateSingleNpc<FGsStateNpcSpawn> Super;

public:
    virtual uint8 GetStateID() override;
    virtual FString Name() override;

	virtual void Enter(UGsGameObjectBase* Owner) override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID) override;
};

/**
* NPC 유휴 상태 클래스
*/
class GAMESERVICE_API FGsStateNpcIdle : public FGsStateSingleNpc<FGsStateNpcIdle>
{
	typedef FGsStateSingleNpc<FGsStateNpcIdle> Super;

public:
	virtual uint8 GetStateID() override;
	virtual FString Name() override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID) override;
};

/**
* 무브 관련 베이스 스테이트
*/
template<class tState>
class GAMESERVICE_API FGsStateNpcMoveBase : public FGsStateSingleNpc<tState>
{
	typedef FGsStateSingleNpc<tState> Super;

public:
	virtual void Update(UGsGameObjectBase* Owner, float Delta) override
	{
		Super::Update(Owner, Delta);

		auto npc = Cast<UGsGameObjectNonPlayer>(Owner);
		npc->GetMovement()->Update(Delta);
	}
};

/**
* NPC 걷기 스테이트
*/
class GAMESERVICE_API FGsStateNpcWalk : public FGsStateNpcMoveBase<FGsStateNpcWalk>
{
	typedef FGsStateNpcMoveBase<FGsStateNpcWalk> Super;

public:
    virtual uint8 GetStateID() override;
    virtual FString Name() override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID) override;
};

/**
* NPC 피격 스테이트
*/
class GAMESERVICE_API FGsStateNpcBeaten : public FGsStateSingleNpc<FGsStateNpcBeaten>
{
	typedef FGsStateSingleNpc<FGsStateNpcBeaten> Super;

public:
	virtual uint8 GetStateID() override;
	virtual FString Name() override;

	virtual void Enter(UGsGameObjectBase* Owner) override;

protected:
	virtual bool OnProcessEvent(UGsGameObjectBase* Owner, EGsStateBase StateID) override;
};
