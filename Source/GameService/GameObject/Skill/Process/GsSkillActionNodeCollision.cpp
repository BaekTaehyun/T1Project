// Fill out your copyright notice in the Description page of Project Settings.


#include "GsSkillActionNodeCollision.h"
#include "GameFramework/Actor.h"
#include "GameObject/Define/GsGameObjectDefine.h"
#include "GameObject/ObjectClass/GsGameObjectBase.h"
#include "GameObject/Component/SkillCollisionComponent.h"
#include "GameObject/GsGameObjectDataCenter.h"
#include "GameObject/Skill/Container/GsSkillDataContainerBase.h"
#include "GameObject/GsSpawnComponent.h"

FGsSkillActionNodeCollision::FGsSkillActionNodeCollision(const FGsSkillActionDataBase& Data) :
	Super(Data),
	CollistionID(Data.RefID)
{
}

FGsSkillActionNodeCollision::~FGsSkillActionNodeCollision()
{
}

void FGsSkillActionNodeCollision::Process(UGsGameObjectBase* Owner)
{
	
}

void FGsSkillActionNodeCollision::Action(UGsGameObjectBase* Owner)
{
	//USkillCollisionComponent 컴퍼넌트 찾기
	auto actor = Owner->GetActor();
	if (auto component = Cast<USkillCollisionComponent>(actor->GetComponentByClass(USkillCollisionComponent::StaticClass())))
	{
		auto container = GGameObjectData()->Get<UGsSkillNotifyCollisionDataContainer>(EGameObjectDataType::SkillNotifyCollision);
		if (auto data = container->FindData(CollistionID))
		{
			component->Set(data);
			//임의의 대상 검출
			//테스트용
			auto spawnList = GSpawner()->FindObjectArray(EGsGameObjectType::NonPlayer);
			for (auto el : spawnList)
			{
				component->InSideCheck(el->GetLocation());
			}
		}
	}
}
