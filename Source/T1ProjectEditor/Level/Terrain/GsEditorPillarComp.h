// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GsEditorPillarComp.generated.h"

class AGsEditorBaseTerrain;

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class T1PROJECTEDITOR_API UGsEditorPillarComp : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorPillarComp")
		AGsEditorBaseTerrain* _Parent;
	/*UPROPERTY()
	UGsEditorPillarComp* _Start;
	UPROPERTY()
	UGsEditorPillarComp* _End;*/

public:
	// Sets default values for this component's properties
	UGsEditorPillarComp();
	void Draw();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransfo, ETeleportType Teleport) override;

};
