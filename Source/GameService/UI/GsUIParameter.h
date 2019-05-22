// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GsUIParameter.generated.h"


/**
 *  �̰� ��ӹ��� Ŭ������ ����. �������Ʈ������ ó�������ϰ� �ϱ� ����.
 */
UCLASS(BlueprintType)
class GAMESERVICE_API UGsUIParameter : public UObject
{
	GENERATED_BODY()

};

/**
 *
 */
UCLASS(BlueprintType)
class GAMESERVICE_API UGsUIParameterInt : public UGsUIParameter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Data;
};

/**
 *
 */
UCLASS(BlueprintType)
class GAMESERVICE_API UGsUIParameterFloat : public UGsUIParameter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Data;
};