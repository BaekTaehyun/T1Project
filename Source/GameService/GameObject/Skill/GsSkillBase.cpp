// Fill out your copyright notice in the Description page of Project Settings.

#include "GsSkillBase.h"
#include "Data/GsSkillDataBase.h"
#include "Container/GsSkillDataContainerBase.h"

void FGsSkillBase::Initialize(UGsGameObjectBase* owner)
{
	Owner = owner;
}

void FGsSkillBase::DeInitialize()
{
	if (CurrentSkillData)
	{
		delete CurrentSkillData;
	}
}

void FGsSkillBase::LoadData(const TCHAR * Path)
{
	SkillFactory = LoadObject<UGsSkillDataContainerBase>(NULL, Path, NULL, LOAD_None, NULL);
	if (!SkillFactory)
	{
		UE_LOG(LogTemp, Error, TEXT("스킬 데이터 로드에 실패하였습니다."));
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

	CurrentSkillData = new FGsRunSKillInfo(GetSkillData(ID));
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