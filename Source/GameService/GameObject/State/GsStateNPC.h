// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsStateBase.h"
#include "GameObject/ActorExtend/GsNpcPawn.h"
#include "GameObject/ObjectClass/GsGameObjectNonPlayer.h"


template <typename T>
class GAMESERVICE_API FGsStateSingleNpc : public FGsStateTargetBase<UGsGameObjectNonPlayer, T>
{
protected:
	typedef FGsStateSingleNpc Super;

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

class GAMESERVICE_API FGsStateNpcSpawn : public FGsStateSingleNpc<FGsStateNpcSpawn>
{
public:
    virtual int GetStateID() override;
    virtual FString Name() override;
    //virtual int GetAniRandomCount() override;
    
    virtual void OnEnter(UGsGameObjectNonPlayer* Owner) override;
};

class GAMESERVICE_API FGsStateNpcIdle : public FGsStateSingleNpc<FGsStateNpcIdle>
{
public:
	virtual int GetStateID() override;
	virtual FString Name() override;

	virtual void OnEnter(UGsGameObjectNonPlayer* Owner) override;
};

class GAMESERVICE_API FGsStateNpcWalk : public FGsStateSingleNpc<FGsStateNpcWalk>
{
public:
    virtual int GetStateID() override;
    virtual FString Name() override;

    virtual void OnEnter(UGsGameObjectNonPlayer* Owner) override;
};

class GAMESERVICE_API FGsStateNpcBeaten : public FGsStateSingleNpc<FGsStateNpcBeaten>
{
public:
	virtual int GetStateID() override;
	virtual FString Name() override;

	virtual void OnEnter(UGsGameObjectNonPlayer* Owner) override;
};
