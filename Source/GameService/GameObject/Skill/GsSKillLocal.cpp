// Fill out your copyright notice in the Description page of Project Settings.

#include "GsSKillLocal.h"
#include "Container/GsSkillDataContainerBase.h"
#include "Process/GsSKillActionNodeProjectile.h"
#include "Process/GsSkillActionNodeCollision.h"
#include "GameObject/ObjectClass/GsGameObjectLocal.h"
#include "GameObject/State/GsFSMManager.h"
#include "GameObject/State/GsStateLocal.h"
#include "GameObject/GsGameObjectDataCenter.h"

FGsSKillLocal::FGsSKillLocal()
{
}

FGsSKillLocal::~FGsSKillLocal()
{
}

void FGsSKillLocal::Initialize(UGsGameObjectBase* owner)
{
	Super::Initialize(owner);

	LoadSKillNode();
}

void FGsSKillLocal::Finalize()
{
	Super::Finalize();

	for (auto pair : MapSkillNodes)
	{
		for (auto el : pair.Value)
		{
			delete el;
		}
	}
	MapSkillNodes.Reset();
}

TArray<FGsSkillActionNodeBase*>* FGsSKillLocal::GetSKillNodes(int ID)
{
	return MapSkillNodes.Find(ID);
}

void FGsSKillLocal::UseSKill(int ID)
{
	Super::UseSKill(ID);

	//스킬 상태로 전환
	auto My = Cast<UGsGameObjectLocal>(Owner);
	auto fsm = My->GetUpperFSM();
	fsm->ProcessEvent(EGsStateBase::Attack);
}

void FGsSKillLocal::LoadSKillNode()
{
	MapSkillNodes.Empty();

	auto dataContainer = GGameObjectData()->Get<UGsSkillDataContainerBase>(EGameObjectDataType::Skill);
	for (auto el : dataContainer->GetSkillData())
	{
		for (auto el2 : el.ListSkillAction)
		{
			FGsSkillActionNodeBase* NodeData = CreateSkillNode(el2);
			if (NodeData)
			{
				NodeData->Process(Owner);
				if (auto findList = MapSkillNodes.Find(el.ID))
				{
					findList->Push(NodeData);
				}
				else
				{
					TArray<FGsSkillActionNodeBase*> nodeList;
					nodeList.Add(NodeData);
					MapSkillNodes.Add(el.ID, nodeList);
				}
			}
		}
	}
}

FGsSkillActionNodeBase* FGsSKillLocal::CreateSkillNode(const FGsSkillActionDataBase& Data) const
{
	//임시
	switch (Data.Type)
	{
	case SkillActionType::CreateProjectile:
		return new FGsSKillActionNodeProjectile(Data);
	case SkillActionType::Collision:
		return new FGsSkillActionNodeCollision(Data);
	}

	return nullptr;
}

void FGsSKillLocal::OnSKillNode()
{
	Super::OnSKillNode();

	if (CurrentSkillData)
	{
		ListRunSkillNodes.Empty();
		ListRunSkillNodes.Append(*GetSKillNodes(CurrentSkillData->Data->ID));
	}
}

void FGsSKillLocal::RunSkillNode(float DeltaTime)
{
	Super::RunSkillNode(DeltaTime);

	//실행할 노드 찾기
	for (auto el : ListRunSkillNodes)
	{
		if (el->GetRate() < CurrentSkillData->GetRate())
		{
			UE_LOG(LogTemp, Warning, TEXT("SkillAction node rate %f Skill Rate %f"), el->GetRate(), CurrentSkillData->GetRate());

			el->Action(Owner);
			//사용 스킬로 이전
			ListUseSkillNodes.Add(el);
			//실행 스킬에서 제거
			ListRunSkillNodes.Remove(el);
			break; //계속 검사해야된다면 iterator로 처리
		}
	}

	//사용 스킬 감시/제거
	UGsGameObjectBase* param = Owner;
	ListUseSkillNodes.RemoveAll([param](FGsSkillActionNodeBase* el)
	{
		return el->Update(param);
	});

	//스킬 종료
	if (CurrentSkillData->IsEnd())
	{
		//상체 유휴 상태로 전환
		auto* My = Cast<UGsGameObjectLocal>(Owner);
		auto* fsm = My->GetUpperFSM();
		fsm->ProcessEvent(EGsStateBase::Idle);
	}
}