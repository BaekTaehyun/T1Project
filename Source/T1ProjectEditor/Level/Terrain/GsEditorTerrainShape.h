// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Level/GsEditorActor.h"
#include "GsEditorTerrainShape.generated.h"

const float DEFAULT_TERRAIN_DISTANCE = 100.0f;

class UGsEditorTerrainPillarComp;
class UGsEditorTerrainPlaneComp;
class UMaterial;
class USceneComponent;
class USplineComponent;
///----------------------------------------------------------------
// Scene 내 이벤트에 필요한 영역과 기능을 관리해주는 에디터용 액터
//----------------------------------------------------------------
UCLASS(NotBlueprintable)
class T1PROJECTEDITOR_API AGsEditorTerrainShape : public AGsEditorActor
{
	GENERATED_BODY()

public:
	//Setting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		FString _Tag = "Terrain info";

	//Widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		float _Height = 100.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		float _WidgetHeight = 25.0f;

	//View 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		FColor _PillarColor = FColor::Yellow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		FColor _PlaneOuterColor = FColor::Blue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		FColor _PlaneInsideColor = FColor::Red;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		UMaterial* _Material;

	//Info	
	UPROPERTY(BlueprintReadOnly, Category = "GsEditorBaseTerrain")
		TArray<FVector> _PointArray;
	UPROPERTY()
		TArray<UGsEditorTerrainPillarComp*> _PillarArray;
	UPROPERTY()
		TArray<UGsEditorTerrainPlaneComp*> _PlaneArray;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		USceneComponent* _Root;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GsEditorBaseTerrain")
		USplineComponent* _Spline;

public:
	AGsEditorTerrainShape();	
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void InitShape() {};
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
		void DestroyAllComponents();
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
		void RegisterPillar(UGsEditorTerrainPillarComp* in_pillar);
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
		void RegisterPlane(UGsEditorTerrainPlaneComp* in_plane);
	FVector GetCenterBetweenPoints(int32 in_start, int32 in_end);
	virtual void DrawShape() {};

protected:
	void InitPointArray();

	//Draw
	void DrawPlanes(bool in_close = true);
	void DrawPlillar();

	//Wiget
	void SetWidgetPosition();
	void SetWidgetText();
};
