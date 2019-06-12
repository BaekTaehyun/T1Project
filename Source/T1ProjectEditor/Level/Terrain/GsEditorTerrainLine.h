// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/Terrain/GsEditorTerrainShape.h"
#include "GsEditorTerrainLine.generated.h"

const int32 DEFAULT_LINE_POINT_NUM = 3;
///----------------------------------------------------------------
// Scene 내 이벤트에 필요한 영역과 기능을 관리해주는 에디터용 액터
//----------------------------------------------------------------
UCLASS(Blueprintable)
class T1PROJECTEDITOR_API AGsEditorTerrainLine : public AGsEditorTerrainShape
{
	GENERATED_BODY()
	
public:
	AGsEditorTerrainLine();
	UFUNCTION(BlueprintCallable, Category = "Game service eidtor terrain line")
	virtual void InitShape() override;
	UFUNCTION(BlueprintCallable, Category = "Game service eidtor terrain line")
	virtual void DrawShape() override;
};
