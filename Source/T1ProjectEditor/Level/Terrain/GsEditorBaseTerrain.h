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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		FColor _PillarColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		FColor _PlaneColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		TArray<FVector> _PointArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		TArray<UGsEditorTerrainPillarComp*> _PillarArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		TArray<UGsEditorTerrainPlaneComp*> _PlaneArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		UMaterial* _PillarMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		UMaterial* _PlaneMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		FIntPoint _InsertRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		int32 _RemoveIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
	USceneComponent* _Root;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
	USplineComponent* _Spline;

public:
	// Sets default values for this actor's properties
	AGsEditorBaseTerrain();

protected:
	//OnConstruction에서 컴포넌트를 생성하면 정상적으로 컴포넌트가 붙지 않는 문제가 있다. Construction Script에서 컴포넌트 생성할 것

	// Called when the game starts or when spawned
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
	void InitPoints();
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
	void DestoryAllComponents();
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
	void ConstructFence();

	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
		void RegisterPillar(UGsEditorTerrainPillarComp* in_pillar, int32 in_index);
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
		void RegisterPlane(UGsEditorTerrainPlaneComp* in_plane);

	FVector GetCenterBetweenPoints(int32 in_start, int32 in_end);
	int32 GetUpperIndex(int32 in_start, int32 in_end);

	//create
	bool TryCreate(int32 in_start, int32 in_end, int32 in_index = -1);
	bool TryCreatePillar(int32 in_index = -1, FVector in_location = FVector::ZeroVector);
	bool TryCreatePlane();

	//remove	
	bool TryRemovePlane();	

public:
	// Called every frame	
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
	virtual void Draw();
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
	void AddPillar(int32 in_start, int32 in_end);
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
	void RemovePillar(int32 in_index);

};
