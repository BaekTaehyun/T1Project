// Fill out your copyright notice in the Description page of Project Settings.

#include "GsSkillBase.h"
#include "Engine/AssetManager.h"
#include "Data/GsSkillDataBase.h"
#include "Container/GsSkillDataContainerBase.h"

FGsSkillBase::FGsSkillBase()
{
}

FGsSkillBase::~FGsSkillBase()
{
}

void FGsSkillBase::Initialize(UGsGameObjectBase* owner)
{
	Owner = owner;
}

void FGsSkillBase::Finalize()
{
	if (CurrentSkillData)
	{
		delete CurrentSkillData;
	}

	for (auto pair : MapAnimation)
	{
		pair.Value = NULL;
	}
	MapAnimation.Reset();
}

void FGsSkillBase::LoadData(const TCHAR * Path)
{
	SkillFactory = LoadObject<UGsSkillDataContainerBase>(NULL, Path, NULL, LOAD_None, NULL);
	if (!SkillFactory)
	{
		UE_LOG(LogTemp, Error, TEXT("스킬 데이터 로드에 실패하였습니다."));
	}
	else
	{
		for (auto el : SkillFactory->GetSkillData())
		{
			//몽타주 데이터 관리 처리
			if (false == MapAnimation.Contains(el.AniPath.ToString()))
			{
				//Montage Load 처리 관련 개선
				UAssetManager::GetStreamableManager().RequestAsyncLoad(el.AniPath, [&]()
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
}

void FGsSkillBase::Update(float delta)
{
}

const FGsSkillDataBase* FGsSkillBase::GetSkillData(int ID)
{
	return SkillFactory->GetSkillData().FindByPredicate([=](const FGsSkillDataBase& el)
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

	auto skillData = GetSkillData(ID);
	auto resAni = MapAnimation.Find(skillData->AniPath.ToString());
	if (skillData && resAni)
	{
		CurrentSkillData = new FGsRunSKillInfo(skillData, resAni->Get());
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