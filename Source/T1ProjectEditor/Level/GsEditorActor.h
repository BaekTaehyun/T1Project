// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GsEditorActor.generated.h"

class UWidgetComponent;

UCLASS()
class T1PROJECTEDITOR_API AGsEditorActor : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GsEditorBaseTerrain")
		UWidgetComponent* _Widget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GsEditorBaseTerrain")
		bool _IsDestoryOnPlay = false;
	
public:	
	// Sets default values for this actor's properties
	AGsEditorActor();
	virtual void BeginPlay() override;
	virtual bool ShouldTickIfViewportsOnly() const override;
	virtual void Tick(float in_delta) override;
};
