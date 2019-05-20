// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsStateBase.h"
#include "GameObject/ActorExtend/GsNpcPawn.h"
#include "GameObject/ObjectClass/GsGameObjectNonPlayer.h"


template <class tState, typename tStateType>
class GAMESERVICE_API FGsStateSingleNpc : public FGsStateTargetBase<UGsGameObjectNonPlayer, tState, tStateType>
{
protected:
	//하위 Class들에서 define처리를 해야하지만 너무 많으므로... ㅠ
	typedef FGsStateSingleNpc Super;

	virtual bool OnProcessEvent(UGsGameObjectNonPlayer* Owner, tStateType StateID) override
	{
		return true;
	}

	//애님 블루프린트에 가장 최우선으로 상태를 전송해줘야한다.
	virtual void OnEnter(UGsGameObjectNonPlayer* Owner) override
	{
		if (AGsNpcPawn* actor = Owner->GetNpc())
		{
			auto anim = actor->GetAnim();
			anim->ChangeState(GetStateID());
		}
	}

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

class GAMESERVICE_API FGsStateNpcSpawn : public FGsStateSingleNpc<FGsStateNpcSpawn, EGsStateBase>
{
public:
    virtual int GetStateID() override;
    virtual FString Name() override;
    //virtual int GetAniRandomCount() override;
    
	virtual bool OnProcessEvent(UGsGameObjectNonPlayer* Owner, EGsStateBase StateID) override;
    virtual void OnEnter(UGsGameObjectNonPlayer* Owner) override;
};

class GAMESERVICE_API FGsStateNpcIdle : public FGsStateSingleNpc<FGsStateNpcIdle, EGsStateBase>
{
public:
	virtual int GetStateID() override;
	virtual FString Name() override;

	virtual bool OnProcessEvent(UGsGameObjectNonPlayer* Owner, EGsStateBase StateID) override;
	virtual void OnEnter(UGsGameObjectNonPlayer* Owner) override;
};

class GAMESERVICE_API FGsStateNpcWalk : public FGsStateSingleNpc<FGsStateNpcWalk, EGsStateBase>
{
public:
    virtual int GetStateID() override;
    virtual FString Name() override;

	virtual bool OnProcessEvent(UGsGameObjectNonPlayer* Owner, EGsStateBase StateID) override;
    virtual void OnEnter(UGsGameObjectNonPlayer* Owner) override;
};

class GAMESERVICE_API FGsStateNpcBeaten : public FGsStateSingleNpc<FGsStateNpcBeaten, EGsStateBase>
{
public:
	virtual int GetStateID() override;
	virtual FString Name() override;

	virtual bool OnProcessEvent(UGsGameObjectNonPlayer* Owner, EGsStateBase StateID) override;
	virtual void OnEnter(UGsGameObjectNonPlayer* Owner) override;
};
