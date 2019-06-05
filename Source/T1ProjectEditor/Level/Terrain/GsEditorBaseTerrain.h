// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GsEditorBaseTerrain.generated.h"

class UGsEditorTerrainPillarComp;
class UGsEditorTerrainPlaneComp;
class UMaterial;
class USceneComponent;
class USplineComponent;

UCLASS()
class T1PROJECTEDITOR_API AGsEditorBaseTerrain : public AActor
{
	GENERATED_BODY()

public:
	//Setting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		float _Height = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		FColor _PillarColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		FColor _PlaneOuterColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		FColor _PlaneInsideColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		UMaterial* _Material;

	//Info
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
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
	// Sets default values for this actor's properties
	AGsEditorBaseTerrain();
	/*virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;*/

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

public:
	// Called every frame	
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
		virtual void Draw();
};
