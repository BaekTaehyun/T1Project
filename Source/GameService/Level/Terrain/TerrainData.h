// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "TerrainData.generated.h"
/**
 * 
 */
UENUM(BlueprintType)
enum class ETerrainShapeType : uint8
{
	Polygon,
	Circle,
	Line
};

UENUM(BlueprintType)
enum class ETerrainEventType : uint8
{
	Spawn,
	Patrol,
};

USTRUCT(BlueprintType)
struct FTerrainBaseData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerrainTableData")
		int32 ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerrainTableData")
		FString PersistantLevelName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerrainTableData")
		FString SubLevelName;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerrainTableData")
		ETerrainShapeType ShapeType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerrainTableData")
		TArray<FVector> PointArray;	
};

USTRUCT(BlueprintType)
struct FTerrainSpawnData : public FTerrainBaseData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerrainSpawnTableData")
		int32 SpawnID;
};

USTRUCT(BlueprintType)
struct FTerrainPatrolData : public FTerrainBaseData
{
	GENERATED_BODY()
};

