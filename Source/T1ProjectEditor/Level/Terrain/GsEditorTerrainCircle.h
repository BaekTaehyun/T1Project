// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/Terrain/GsEditorTerrainShape.h"
#include "GsEditorTerrainCircle.generated.h"

const int32 DEFAULT_CIRCLE_POINT_NUM = 8;
const int32 INVALIDE_INDEX = -1;
//----------------------------------------------------------------
// Scene 내 닫힌 원 영역을 보여주는 액터
//----------------------------------------------------------------
UCLASS(Blueprintable)
class T1PROJECTEDITOR_API AGsEditorTerrainCircle : public AGsEditorTerrainShape
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain", meta = (EditCondition = "_bCircle"))
		float _CircleDistance = DEFAULT_TERRAIN_DISTANCE;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Game service eidtor terrain circle")
	virtual void InitShape() override;
	UFUNCTION(BlueprintCallable, Category = "Game service eidtor terrain circle")
	virtual void DrawShape() override;

private:
#if WITH_EDITOR
	bool TryGetSelectedIndexInSpline(int32& out_index);
#endif
};
