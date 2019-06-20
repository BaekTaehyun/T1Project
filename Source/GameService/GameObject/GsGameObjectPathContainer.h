// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GsGameObjectPathContainer.generated.h"

UENUM(BlueprintType)
enum class EGameObjectResType : uint8
{
	SkillDataContainer,
	PartsDataContainer,
	SkillNotifyCollisionDataContainer,
};

USTRUCT(BlueprintType)
struct FGameObjectResPath
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere)
	EGameObjectResType Type;

	UPROPERTY(EditAnywhere, meta = (AllowedClasses = "UDataAsset"))
	FSoftObjectPath Path;
};

/**
 * 
 */
UCLASS()
class GAMESERVICE_API UGsGameObjectPathContainer : public UDataAsset
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere)
	TArray<FGameObjectResPath> ListDataPath;
	
public:
	FString Get(EGameObjectResType Type);
};
