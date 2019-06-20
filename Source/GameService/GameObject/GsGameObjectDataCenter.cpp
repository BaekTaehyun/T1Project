// Fill out your copyright notice in the Description page of Project Settings.


#include "GsGameObjectDataCenter.h"
#include "GameObject/GsGameObjectPathContainer.h"
#include "GameObject/Skill/Container/GsSkillDataContainerBase.h"
#include "GameObject/Parts/Container/GsPartsDataContainerBase.h"

template<>
FGsGameObjectDataCenter* FGsGameObjectDataCenterSingle::_instance = nullptr;

FGsGameObjectDataCenter::FGsGameObjectDataCenter()
{
}

FGsGameObjectDataCenter::~FGsGameObjectDataCenter()
{
}

void FGsGameObjectDataCenter::Initialize()
{
	TGsSingleton::InitInstance(this);

	LoadData(EGameObjectDataType::Path);
}

void FGsGameObjectDataCenter::Finalize()
{
	TGsSingleton::RemoveInstance();
}

UDataAsset* FGsGameObjectDataCenter::LoadData(EGameObjectDataType Type)
{
	switch (Type)
	{
	case EGameObjectDataType::Path:
		{
			//���� ���ҽ� �н������� ������ �ִ� UGsGameObjectPathContainer Ŭ������ �ϵ��ڵ����� �н���θ� ����
			static FString resPathData(TEXT("GsGameObjectPathContainer'/Game/Resource/DataAsset/ResPath.ResPath'"));
			return MapData.Emplace(Type, LoadObject<UGsGameObjectPathContainer>(
				NULL, *resPathData, NULL, LOAD_None, NULL)
			);
		}
	case EGameObjectDataType::Skill:
		{
			auto path = Get<UGsGameObjectPathContainer>(EGameObjectDataType::Path);
			return MapData.Emplace(Type, LoadObject<UGsSkillDataContainerBase>(
				NULL, *path->Get(EGameObjectResType::SkillDataContainer), NULL, LOAD_None, NULL)
			);
		}
	case EGameObjectDataType::Parts:
		{
			auto path = Get<UGsGameObjectPathContainer>(EGameObjectDataType::Path);
			return MapData.Emplace(Type, LoadObject<UGsPartsDataContainerBase>(
				NULL, *path->Get(EGameObjectResType::PartsDataContainer), NULL, LOAD_None, NULL)
			);
		}
	case EGameObjectDataType::SkillNotifyCollision:
		{
			auto path = Get<UGsGameObjectPathContainer>(EGameObjectDataType::Path);
			return MapData.Emplace(Type, LoadObject<UGsSkillNotifyCollisionDataContainer>(
				NULL, *path->Get(EGameObjectResType::SkillNotifyCollisionDataContainer), NULL, LOAD_None, NULL)
			);
		}		
	}

	return nullptr;
}