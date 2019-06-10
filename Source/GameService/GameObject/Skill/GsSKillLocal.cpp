// Fill out your copyright notice in the Description page of Project Settings.

#include "GsSKillLocal.h"
#include "Container/GsSkillDataContainerBase.h"
#include "Process/GsSKillActionNodeProjectile.h"
#include "GameObject/ObjectClass/GsGameObjectLocal.h"
#include "GameObject/State/GsFSMManager.h"
#include "GameObject/State/GsStateLocal.h"

FGsSKillLocal::FGsSKillLocal()
{
}

FGsSKillLocal::~FGsSKillLocal()
{
}

void FGsSKillLocal::Initialize(UGsGameObjectBase* owner)
{
	Super::Initialize(owner);
}

TArray<FGsSkillActionNodeBase*>* FGsSKillLocal::GetSKillNodes(int ID)
{
	return MapSkillNodes.Find(ID);
}

void FGsSKillLocal::LoadData(const TCHAR * Path)
{
	Super::LoadData(Path);
	LoadSKillNode();
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
	for (auto el : SkillFactory->GetSkillData())
	{
		for (auto el2 : el.SkillAction)
		{
			FGsSkillActionNodeBase* NodeData = CreateSkillNode(el2);
			if (NodeData)
			{
				NodeData->Process(Owner);
				TArray<FGsSkillActionNodeBase*>* findList = MapSkillNodes.Find(el.ID);
				if (findList)
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
	case 1:
		return new FGsSKillActionNodeProjectile(Data);
	}

	return nullptr;
}

void FGsSKillLocal::OnSKillNode()
{
	Super::OnSKillNode();

	if (CurrentSkillData)
	{
		RunSkillNodes.Empty();
		RunSkillNodes.Append(*GetSKillNodes(CurrentSkillData->Data->ID));
	}
}

void FGsSKillLocal::RunSkillNode(float DeltaTime)
{
	Super::RunSkillNode(DeltaTime);

	//실행할 노드 찾기
	for (auto el : RunSkillNodes)
	{
		if (el->GetRate() < CurrentSkillData->GetRate())
		{
			UE_LOG(LogTemp, Warning, TEXT("SkillAction node rate %f Skill Rate %f"), el->GetRate(), CurrentSkillData->GetRate());

			el->Action(Owner);
			//사용 스킬로 이전
			UseSkillNodes.Add(el);
			//실행 스킬에서 제거
			RunSkillNodes.Remove(el);
			break; //계속 검사해야된다면 iterator로 처리
		}
	}

	//사용 스킬 감시/제거
	UGsGameObjectBase* param = Owner;
	UseSkillNodes.RemoveAll([param](FGsSkillActionNodeBase* el)
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