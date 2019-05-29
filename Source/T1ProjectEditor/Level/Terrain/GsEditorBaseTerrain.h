// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GsEditorBaseTerrain.generated.h"

class UGsEditorPillarComp;
class UGsEditorTerrainPlaneComp;

UCLASS()
class T1PROJECTEDITOR_API AGsEditorBaseTerrain : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		TArray<UGsEditorPillarComp*> _PillarArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		TArray<UGsEditorTerrainPlaneComp*> _PlaneArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		UMaterial* _PillarMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		UMaterial* _PlaneMaterial;

public:
	// Sets default values for this actor's properties
	AGsEditorBaseTerrain();

protected:
	// Called when the game starts or when spawned
	virtual void OnConstruction(const FTransform& in_transform) override;
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = "GsEditorBaseTerrain")
		void RegisterPillar(UGsEditorPillarComp* in_pillar);
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
	virtual void Tick(float DeltaTime) override;
	virtual void Draw();
	void AddPillar(int32 in_start, int32 in_end);
	void RemovePillar(int32 in_index);

};
