// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsControlMode.h"
#include "Classes/Engine/DataTable.h"
#include "GsCamModeData.generated.h"
/**
 * 
 */
USTRUCT(Blueprintable)
struct GAMESERVICE_API FGsCamModeData : public FTableRowBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGsControlMode Mode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ArmLengthTo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ControllerPitch;
};
