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
	virtual void Finalize();
	virtual void LoadData(const TCHAR * Path);

	//가변 템플릿 사용
	//관련 참고 링크 정보 : 
	//https://lusain.tistory.com/3
	//https://docs.microsoft.com/en-us/cpp/cpp/ellipses-and-variadic-templates?view=vs-2019
	template<typename T = EGsPartsType, typename... T2>
	void Attach(const T& Type, const T2&... Types);
	
	template<typename T = EGsPartsType, typename... T2>
	void Detach(const T& Type, const T2&... Types);

	void AttachAll();

	const FGsPartsDataBase* GetParts(EGsPartsType Type);		//추후 리소스 Load로 수정
	bool IsEquip(EGsPartsType Type);

protected:
	virtual void AddParts(EGsPartsType Type);
	virtual void RemoveParts(EGsPartsType Type);

	//template<typename T = EGsPartsType, typename... T2> 후처리용
	void Attach();
	//template<typename T = EGsPartsType, typename... T2> 후처리용
	void Detach();

	virtual void Attached();
	virtual void Detached();

protected:
	class UGsGameObjectBase* Owner;
	
	class UGsPartsDataContainerBase* PartsFctory;
	TArray<TSharedPtr<FGsCPartsData>> Parts;
};

template<typename T, typename... T2>
void FGsPartsBase::Attach(const T& Type, const T2&... Types)
{
	AddParts(Type);
	Attach(Types...);
}

template<typename T, typename... T2>
void FGsPartsBase::Detach(const T& Type, const T2&... Types)
{
	RemoveParts(Type);
	Detach(Types...);
}