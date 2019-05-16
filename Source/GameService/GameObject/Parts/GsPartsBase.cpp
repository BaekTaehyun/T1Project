// Fill out your copyright notice in the Description page of Project Settings.

#include "GsPartsBase.h"
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

void FGsPartsBase::DeInitialize()
{
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
	return Parts.ContainsByPredicate([=](const FGsPartsDataBase* el)
	{
		return el->Type == Type;
	});
}

void FGsPartsBase::Attach(EGsPartsType Type, ...)
{
}

void FGsPartsBase::Detach(EGsPartsType Type, ...)
{
}

void FGsPartsBase::AttachAll()
{
}


