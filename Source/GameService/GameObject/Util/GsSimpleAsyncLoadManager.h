// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"

/**
 * 미적용 테스트용
 */
template<typename tDataType>
class GAMESERVICE_API FGsSimpleDataUpdater
{
public:
	typedef TArray<const tDataType*> Datas;
	DECLARE_DELEGATE_OneParam(FUpdateData, Datas);
	FUpdateData CallbackUpdateData;

	virtual ~FGsSimpleDataUpdater<tDataType>() {}

	virtual void Initialize()
	{
	}

	virtual void Finalize()
	{
		InDatas.Empty();
		Stop();
	}

	virtual bool Update(float Delta)
	{
		Execute();
		return true;
	}

	virtual void Execute()
	{
		if (InDatas.Num() > 0)
		{
			if (CallbackUpdateData.IsBound())
			{
				CallbackUpdateData.Execute(InDatas);
			}
			InDatas.Empty();
			Stop();
		}
	}

	void Start()
	{
		if (!TickDelegate.IsValid())
		{
			TickDelegate = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FGsSimpleDataUpdater::Update));
		}
	}

	void Stop()
	{
		if (TickDelegate.IsValid())
		{
			FTicker::GetCoreTicker().RemoveTicker(TickDelegate);
		}
	}

	void AddData(const tDataType* Data)
	{
		InDatas.AddUnique(Data);
		Start();
	}

protected:
	FDelegateHandle TickDelegate;
	Datas InDatas;
};

template<typename tDataType, typename tLoadType>
class GAMESERVICE_API FGsSimpleAsyncLoadManager : public FGsSimpleDataUpdater<tDataType>
{
	typedef FGsSimpleDataUpdater<tDataType> Super;

public:
	virtual void Initialize() override
	{
		Super::Initialize();
	}

	virtual void Finalize() override
	{
		Super::Finalize();

		tempDatas.Empty();
		InStreams.Empty();
	}

	virtual void Execute() override
	{
		if (InStreams.Num() > 0)
		{
			tempDatas = InDatas;
			UAssetManager::GetStreamableManager().RequestAsyncLoad(
				InStreams, [=]() {
					if (CallbackUpdateData.IsBound())
					{
						CallbackUpdateData.Execute(tempDatas);
						tempDatas.Empty();
					}
				});

			InDatas.Empty();
			InStreams.Empty();
			Stop();
		}
		else
		{
			
			Super::Execute();
		}
	}

	virtual void LoadData(const tDataType* Data, const FSoftObjectPath Object)
	{
		AddData(Data);
		InStreams.AddUnique(Object);
	}

protected:
	TArray<FSoftObjectPath> InStreams;
	Datas tempDatas;
};
