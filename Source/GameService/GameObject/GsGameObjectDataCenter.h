// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "../Class/GsSingleton.h"
#include "../Class/GsManager.h"

enum class EGameObjectDataType
{
	Path,
	Skill,
	Parts,
	SkillNotifyCollision,
};

/**
 * �ӽ� ������ ���� Ŭ����
 * ���� ������ ���� ������ ���ԵǸ� ����
 */
class GAMESERVICE_API FGsGameObjectDataCenter : 
	public TGsSingleton<FGsGameObjectDataCenter>,
	public IGsManager
{
private:
	TMap<EGameObjectDataType, UDataAsset*> MapData;

public:
	FGsGameObjectDataCenter();
	virtual ~FGsGameObjectDataCenter();

public:
	//IGsManager
	virtual void Initialize() override;
	virtual void Finalize() override;

private:
	UDataAsset* LoadData(EGameObjectDataType Type);

public:
	template <typename T>
	T* Get(EGameObjectDataType Type)
	{
		if (auto find = MapData.Find(Type))
		{
			return Cast<T>(*find);
		}

		return Cast<T>(LoadData(Type));
	}
};

typedef TGsSingleton<FGsGameObjectDataCenter> FGsGameObjectDataCenterSingle;
#define GGameObjectData() FGsGameObjectDataCenterSingle::Instance()
