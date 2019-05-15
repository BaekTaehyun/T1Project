// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Public/SkeletalMeshMerge.h"
#include "Data/GsPartsDataBase.h"

/**
 * 
 */
class GAMESERVICE_API FGsPartsBase
{
public:
	FGsPartsBase();
	virtual ~FGsPartsBase();

	virtual void Initialize(class UGsGameObjectBase* Owner);
	virtual void DeInitialize();
	virtual void LoadData(const TCHAR * Path);

	virtual void Attach(EGsPartsType Type, ...);
	virtual void Detach(EGsPartsType Type, ...);
	virtual void AttachAll();

	const FGsPartsDataBase* GetParts(EGsPartsType Type);		//추후 리소스 Load로 수정
	bool IsEquip(EGsPartsType Type);

protected:
	class UGsGameObjectBase* Owner;
	
	class UGsPartsDataContainerBase* PartsFctory;
	TArray<const FGsPartsDataBase*> Parts;
};