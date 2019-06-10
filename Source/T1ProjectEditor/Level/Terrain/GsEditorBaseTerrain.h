// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GsEditorBaseTerrain.generated.h"

UENUM(BlueprintType)
enum class ETerrainShapeType : uint8
{
	Polygon,
	Circle,
	Line,
};

const int32 DEFAULT_CIRCLE_POINT_NUM = 8;
const int32 DEFAULT_LINE_POINT_NUM = 2;
const int32 DEFAULT_POLYGON_POINT_NUM = 3;
const float DEFAULT_TERRAIN_DISTANCE = 100.0f;
const int32 INVALIDE_INDEX = -1;

class UGsEditorTerrainPillarComp;
class UGsEditorTerrainPlaneComp;
class UMaterial;
class USceneComponent;
class USplineComponent;
class UWidgetComponent;
class UGsEditorTerrainWidget;

//----------------------------------------------------------------
// Scene 내 이벤트에 필요한 영역과 기능을 관리해주는 에디터용 액터
//----------------------------------------------------------------
UCLASS()
class T1PROJECTEDITOR_API AGsEditorBaseTerrain : public AActor
{
	GENERATED_BODY()

public:
	//Setting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		ETerrainShapeType _ShapeType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		FString _Tag = "Terrain info";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		float _Distance = DEFAULT_TERRAIN_DISTANCE;

	//Widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		float _Height = 100.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		float _WidgetHeight = 25.0f;

	//View 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		FColor _PillarColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		FColor _PlaneOuterColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		FColor _PlaneInsideColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		UMaterial* _Material;	

	//Info	
	UPROPERTY(BlueprintReadOnly, Category = "GsEditorBaseTerrain")
		TArray<FVector> _PointArray;
	UPROPERTY()
		TArray<UGsEditorTerrainPillarComp*> _PillarArray;
	UPROPERTY()
		TArray<UGsEditorTerrainPlaneComp*> _PlaneArray;	

	//Default components
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
		UGsEditorTerrainWidget* _WidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GsEditorBaseTerrain")	
		USceneComponent* _Root;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GsEditorBaseTerrain")
		USplineComponent* _Spline;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GsEditorBaseTerrain")
		UWidgetComponent* _Widget;	

public:
	// Sets default values for this actor's properties
	AGsEditorBaseTerrain();
	virtual void Tick(float in_delta) override;
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
		virtual void Draw();	
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
	bool IsLineType();

protected:
	//OnConstruction에서 컴포넌트를 생성하면 정상적으로 컴포넌트가 붙지 않는 문제가 있다. Construction Script에서 컴포넌트 생성할 것

	// Called when the game starts or when spawned
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
		void InitPoints();	
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
		void DestoryAllComponents();	
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
		void RegisterPillar(UGsEditorTerrainPillarComp* in_pillar, int32 in_index);
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
		void RegisterPlane(UGsEditorTerrainPlaneComp* in_plane);
	FVector GetCenterBetweenPoints(int32 in_start, int32 in_end);

#if WITH_EDITOR
	virtual bool ShouldTickIfViewportsOnly() const override;
	void OnSelectCallback(UObject* in_object);
	void OnUnSelectCallback();	
#endif

private:	
	//Initialize 
	void InitPolygon();
	void InitCircle();
	void InitLine();
	void InitPointArray();
#if WITH_EDITOR
	bool TryGetSelectedIndexInSpline(int32& out_index);
#endif

	//Draw
	void DrawPolygon();
	void DrawCircle();
	void DawLine();
	void DrawPlanes(bool in_close = true);
	void DrawPlillar();

	//Wiget
	void SetWidgetPosition();
	void SetWidgetText();


};
