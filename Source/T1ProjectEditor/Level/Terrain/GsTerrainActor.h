// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "GsTerrainActor.generated.h"

/**
 * 
 */
class UGsTerrainPoint;

UCLASS()
class T1PROJECTEDITOR_API AGsTerrainActor : public ATargetPoint
{
	GENERATED_BODY()

public:
	UPROPERTY()
		TArray<UGsTerrainPoint*> _TerrainPoint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category ="Gs Terrain Actor")
		FColor _DebugMeshColor;

public:
	AGsTerrainActor();	
	virtual void Tick(float in_deltaTime) override;
	virtual void TickActor(float in_deltaTime, enum ELevelTick in_tickType, FActorTickFunction& in_thisTickFunction) override;
	virtual bool ShouldTickIfViewportsOnly() const override;
	void DrawLines();	
	void DrawRects();

private:
	void DrawLine(UActorComponent* in_startComp, UActorComponent* in_endComp);
	void DrawRect(UActorComponent* in_startComp, UActorComponent* in_endComp);
	
};
