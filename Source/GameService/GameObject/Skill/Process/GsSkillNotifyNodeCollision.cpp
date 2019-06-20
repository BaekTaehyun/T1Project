// Fill out your copyright notice in the Description page of Project Settings.


#include "GsSkillNotifyNodeCollision.h"
#include "GameFramework/Actor.h"
#include "GameObject/Define/GsGameObjectDefine.h"
#include "GameObject/ObjectClass/GsGameObjectBase.h"
#include "GameObject/Component/GsSkillCollisionComponent.h"
#include "GameObject/Skill/Container/GsSkillDataContainerBase.h"
#include "GameObject/GsGameObjectDataCenter.h"
#include "GameObject/GsSpawnComponent.h"

FGsSkillNotifyNodeCollision::FGsSkillNotifyNodeCollision(const FGsSkillNotifyDataBase& Data) :
	Super(Data),
	CollistionID(Data.RefID)
{
}

FGsSkillNotifyNodeCollision::~FGsSkillNotifyNodeCollision()
{
}

void FGsSkillNotifyNodeCollision::Process(UGsGameObjectBase* Owner)
{
	
}

void FGsSkillNotifyNodeCollision::Action(UGsGameObjectBase* Owner)
{
	//USkillCollisionComponent 컴퍼넌트 찾기
	auto actor = Owner->GetActor();
	if (auto component = Cast<UGsSkillCollisionComponent>(actor->GetComponentByClass(UGsSkillCollisionComponent::StaticClass())))
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
