// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GsUIParameter.generated.h"


/**
 *  이걸 상속받은 클래스를 전달. 블루프린트에서도 처리가능하게 하기 위함.
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