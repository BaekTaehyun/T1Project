// Fill out your copyright notice in the Description page of Project Settings.

#include "GsSkillBase.h"
#include "Engine/AssetManager.h"
#include "Data/GsSkillDataBase.h"
#include "Container/GsSkillDataContainerBase.h"
#include "GameObject/GsGameObjectDataCenter.h"

FGsSkillBase::FGsSkillBase()
{
}

FGsSkillBase::~FGsSkillBase()
{
}

void FGsSkillBase::Initialize(UGsGameObjectBase* owner)
{
	Owner = owner;

	//몽타주 데이터 관리 처리
	//관련 리소스 관리 구조가 필요할것으로 보임
	auto dataContainer = GGameObjectData()->Get<UGsSkillDataContainerBase>(EGameObjectDataType::Skill);
	for (auto el : dataContainer->GetSkillData())
	{
		//몽타주 데이터 관리 처리
		//관련 리소스 관리 구조가 필요할것으로 보임
		if (false == MapAnimation.Contains(el.AniPath.ToString()))
		{
			UAssetManager::GetStreamableManager().RequestAsyncLoad(el.AniPath, [=]()
				{
					if (auto ani = el.AniPath.ResolveObject())
					{
						MapAnimation.Emplace(el.AniPath.ToString(), Cast<UAnimMontage>(ani));
					}
				});
		}
		//
	}
}

void FGsSkillBase::Finalize()
{
	if (CurrentSkillData)
	{
		CurrentSkillData->Animation = NULL;
		delete CurrentSkillData;
	}

	//unload처리 확인
	for (auto pair : MapAnimation)
	{
		//UAssetManager::GetStreamableManager().Unload(pair.Key);
		pair.Value = NULL;
	}
	MapAnimation.Reset();
}

void FGsSkillBase::Update(float delta)
{
}

const FGsSkillDataBase* FGsSkillBase::GetSkillData(int ID)
{
	auto dataContainer = GGameObjectData()->Get<UGsSkillDataContainerBase>(EGameObjectDataType::Skill);
	return dataContainer->GetSkillData().FindByPredicate([=](const FGsSkillDataBase& el)
	{
		return el.ID == ID;
	});
}

void FGsSkillBase::UseSKill(int ID)
{
	//기존 정보 제거
	if (CurrentSkillData)
	{
		delete CurrentSkillData;
	}

	if (auto skillData = GetSkillData(ID))
	{
		auto resAni = MapAnimation.Find(skillData->AniPath.ToString());
		CurrentSkillData = new FGsRunSKillInfo(skillData, *resAni);
	}
}

void FGsSkillBase::OnSKillNode()
{
	//타이머 설정
}

void FGsSkillBase::RunSkillNode(float DeltaTime)
{
	CurrentSkillData->Timer += DeltaTime;
}

void FGsSkillBase::EndSKillNode()
{
	CurrentSkillData = nullptr;
}