// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/Terrain/GsEditorTerrainShape.h"
#include "GsEditorTerrainPolygon.generated.h"

const int32 DEFAULT_POLYGON_POINT_NUM = 3;
//----------------------------------------------------------------
// Scene ³» ´ÝÈù Æú¸®°ï ¿µ¿ªÀ» º¸¿©ÁÖ´Â ¾×ÅÍ
//----------------------------------------------------------------
UCLASS(Blueprintable)
class T1PROJECTEDITOR_API AGsEditorTerrainPolygon : public AGsEditorTerrainShape
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Game service eidtor terrain polygon")
	virtual void InitShape() override;
	UFUNCTION(BlueprintCallable, Category = "Game service eidtor terrain polygon")
	virtual void DrawShape() override;
	
};
