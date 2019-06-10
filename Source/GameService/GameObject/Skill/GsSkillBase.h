// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/GsSkillDataBase.h"

class UGsGameObjectBase;
class UGsSkillDataContainerBase;

/**
 * 
 */
class GAMESERVICE_API FGsSkillBase
{
public:
	FGsSkillBase();
	virtual ~FGsSkillBase();

	virtual void Initialize(UGsGameObjectBase* Owner);
	virtual void Finalize();
    virtual void Update(float Delta);

	virtual void LoadData(const TCHAR * Path);
	virtual void UseSKill(int ID);

	//CurrentSkillData 구조체 기반으로 스킬 실행 활성화
	//스킬 노드 제어기가 따로 필요할수있다.
	virtual void OnSKillNode();
	virtual void RunSkillNode(float DeltaTime);
	virtual void EndSKillNode();

	const FGsSkillDataBase* GetSkillData(int ID);
	struct FGsRunSKillInfo* CurrentSkillData = nullptr;

protected:
	UGsGameObjectBase* Owner;
	UGsSkillDataContainerBase* SkillFactory;
};

//발동 스킬(액션) 구조체
struct FGsRunSKillInfo
{
	const FGsSkillDataBase* Data;
	float Timer;

	FGsRunSKillInfo(const FGsSkillDataBase* SkillData)
	{
		Data = SkillData;
		Timer = 0.f;
	}

	UAnimMontage* GetAni()
	{
		return Data->ResAni.Get();
	}

	bool IsEnd()
	{
		return Data->Duration < Timer;
	}

	float GetRate()
	{
		return Timer / Data->Duration;
	}
};
