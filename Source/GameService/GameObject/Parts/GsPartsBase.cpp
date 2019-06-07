// Fill out your copyright notice in the Description page of Project Settings.

#include "GsPartsBase.h"
#include "Engine/AssetManager.h"
#include "Container/GsPartsDataContainerBase.h"
#include "Data/GsPartsDataBase.h"

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
	for (auto el : Parts)
	{
		el.Get()->Mesh = NULL;
	}
	Parts.Reset();
}

void FGsPartsBase::LoadData(const TCHAR * Path)
{
	PartsFctory = LoadObject<UGsPartsDataContainerBase>(NULL, Path, NULL, LOAD_None, NULL);
	if (!PartsFctory)
	{
		UE_LOG(LogTemp, Error, TEXT("파츠 데이터 로드에 실패하였습니다."));
	}
}

const FGsPartsDataBase* FGsPartsBase::GetParts(EGsPartsType Type)
{
	return PartsFctory->GetPartsData().FindByPredicate([=](const FGsPartsDataBase& el)
	{
		return el.Type == Type;
	});
}

bool FGsPartsBase::IsEquip(EGsPartsType Type)
{
	return Parts.ContainsByPredicate([=](auto data)
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
	if (Parts.ContainsByPredicate([=](auto data)
		{
			return Type == data.Get()->Type;
		}))
	{
		return;
	}

	//Set처리 인터페이스 필요
	auto partsData = Parts.Emplace_GetRef(MakeShareable(new FGsCPartsData(Type, data->Path)));
}

void FGsPartsBase::RemoveParts(EGsPartsType Type)
{
	if (auto findParts = Parts.FindByPredicate([=](auto data)
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
	for (auto el : Parts) { Streams.Emplace(el.Get()->Path); }

	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		Streams, [&]() {
			//이부분 개선 필요..
			for (auto el : Parts)
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

