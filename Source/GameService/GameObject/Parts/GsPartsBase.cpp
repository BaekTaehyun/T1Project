// Fill out your copyright notice in the Description page of Project Settings.

#include "GsPartsBase.h"
#include "Engine/AssetManager.h"
#include "Container/GsPartsDataContainerBase.h"
#include "Data/GsPartsDataBase.h"
#include "GameObject/GsGameObjectDataCenter.h"

FGsPartsBase::FGsPartsBase()
{
}

FGsPartsBase::~FGsPartsBase()
{
}

void FGsPartsBase::Initialize(UGsGameObjectBase* owner)
{
	Owner = owner;
}

void FGsPartsBase::Finalize()
{	
	for (auto el : ListParts)
	{
		el.Get()->Mesh = NULL;
	}
	ListParts.Reset();
}

const FGsPartsDataBase* FGsPartsBase::GetParts(EGsPartsType Type)
{
	auto dataContainer = GGameObjectData()->Get<UGsPartsDataContainerBase>(EGameObjectDataType::Parts);
	return dataContainer->GetPartsData().FindByPredicate([=](const FGsPartsDataBase& el)
	{
		return el.Type == Type;
	});
}

bool FGsPartsBase::IsEquip(EGsPartsType Type)
{
	return ListParts.ContainsByPredicate([=](auto data)
	{
		return data.Get()->Type == Type;
	});
}

void FGsPartsBase::AttachAll()
{
	Attach(EGsPartsType::HAIR, EGsPartsType::FACE,
		EGsPartsType::HEAD, EGsPartsType::BODY, EGsPartsType::SHOULDER,
		EGsPartsType::GLOVE, EGsPartsType::LEG, EGsPartsType::FOOT);
}

void FGsPartsBase::Attached()
{
}

void FGsPartsBase::Detached()
{
}

void FGsPartsBase::AddParts(EGsPartsType Type)
{
	//외부 데이터 읽기
	auto data = GetParts(Type);
	if (NULL == data) { return; }

	//장착 데이터 검사
	if (ListParts.ContainsByPredicate([=](auto data)
		{
			return Type == data.Get()->Type;
		}))
	{
		return;
	}

	//Set처리 인터페이스 필요
	auto partsData = ListParts.Emplace_GetRef(MakeShareable(new FGsCPartsData(Type, data->Path)));
}

void FGsPartsBase::RemoveParts(EGsPartsType Type)
{
	if (auto findParts = ListParts.FindByPredicate([=](auto data)
		{
			return Type == data.Get()->Type;
		}))
	{
		findParts->Get()->Mesh = NULL;
		//이렇게만 하면 리스트에서 제거되는지 확인 필요..
		findParts->Reset();
	}
}

void FGsPartsBase::Attach()
{
#pragma	message("[TODO] LBY : convet해주는 유틸 함수 있는지 확인")
	TArray<FSoftObjectPath> Streams;
	for (auto el : ListParts) { Streams.Emplace(el.Get()->Path); }

	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		Streams, [&]() {
			//로딩 완료가 콜되기전에 내부 Parts 관련 데이터가 삭제되면 크래쉬가 나는것 같다.
			//이부분은 확인이 필요
			for (auto el : ListParts)
			{
				if (auto mesh = el.Get()->Path.ResolveObject())
				{
					el.Get()->SetMesh(Cast<USkeletalMesh>(mesh));
				}
			}

			Attached();
		});
}

void FGsPartsBase::Detach()
{
	Detached();
}

