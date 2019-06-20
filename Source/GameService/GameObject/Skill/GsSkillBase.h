// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObject/Skill/Data/GsSkillDataBase.h"

class UGsGameObjectBase;
class UGsSkillDataContainerBase;
struct FGsRunSKillInfo;

/**
 * Object 스킬 처리 담당 Base 클래스
 * UGsGameObjectPlayer 클래스 하위 타입으로만 임의 제한
 */
class GAMESERVICE_API FGsSkillBase
{
public:
	FGsRunSKillInfo* CurrentSkillData = nullptr;

protected:
	UGsGameObjectBase* Owner = nullptr;
	//Montage 리소스 관리용
	//이부분에 대한 구조는 개선이 필요함
	TMap<FString, UAnimMontage*> MapAnimation;

public:
	FGsSkillBase();
	virtual ~FGsSkillBase();

public:
	virtual void Initialize(UGsGameObjectBase* Owner);
	virtual void Finalize();
    virtual void Update(float Delta);

	virtual void UseSKill(int ID);

public:
	//CurrentSkillData 구조체 기반으로 스킬 실행 활성화
	//스킬 노드 제어기가 따로 필요할수있다.
	virtual void OnSKillNode();
	virtual void RunSkillNode(float DeltaTime);
	virtual void EndSKillNode();

public:
	const FGsSkillDataBase* GetSkillData(int ID);
};

//발동 스킬(액션) 구조체
struct FGsRunSKillInfo
{
	const FGsSkillDataBase* Data;
	UAnimMontage* Animation;
	float Timer;

	~FGsRunSKillInfo()
	{
		Data = NULL;
		//Animation = NULL;
	}

	FGsRunSKillInfo(const FGsSkillDataBase* SkillData, UAnimMontage* Ani)
	{
		Data = SkillData;
		Animation = Ani;
		Timer = 0.f;
	}
	
	UAnimMontage* GetAni()
	{
		return Animation;
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
