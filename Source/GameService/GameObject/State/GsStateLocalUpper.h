// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsStateBase.h"

/**
* Localĳ���� ��ü���� State �θ� Ŭ����
* ���Խ� AnimBlueprint ���� ��ȯ ������ ���� �˷����� �ʴ´�. 
* ���� ��� ������ �ִٸ� Upper ���� ������ �߰��Ͽ� ����ȭ ������� �ҵ�...
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

//Ÿ���� �Ǽ� ���� ���� ��� ��ũ�� ����
#define ObjectUpperStateChange(State)	ChangeState<State>(Cast<UGsGameObjectLocal>(Owner)->GetUpperFSM())
};

/**
* Local ��ü ���� ���� Ŭ����
* ��ü ���� Ŭ����
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
* Local ���� ���� Ŭ����
* ��ü ���� Ŭ����
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
