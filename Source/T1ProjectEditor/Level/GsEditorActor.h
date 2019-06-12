// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Engine/EngineBaseTypes.h"
#include "GsEditorActor.generated.h"

class UWidgetComponent;

UCLASS()
class T1PROJECTEDITOR_API AGsEditorActor : public AActor
{
	GENERATED_BODY()

public:
	//Setting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AGsEditorActor")
		FString _Description = "Description";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AGsEditorActor")
		float _WidgetHeight = 25.0f;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AGsEditorActor")
		bool _IsDestoryOnPlay = false;

	//Component
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AGsEditorActor")
		UWidgetComponent* _Widget;	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AGsEditorActor")
		USceneComponent* _Root;
		
public:	
	// Sets default values for this actor's properties
	AGsEditorActor();
	virtual void BeginPlay() override;
	virtual bool ShouldTickIfViewportsOnly() const override;
	virtual void Tick(float in_delta) override;
	virtual void TickActor(float in_delta, ELevelTick in_type, FActorTickFunction& in_tickFuc) override;

protected:
	//Wiget
	UFUNCTION(BlueprintCallable, Category = "Game service actor")
	virtual void InvalideWidgetPosition();
	UFUNCTION(BlueprintCallable, Category = "Game service actor")
	void InvalideWidgetText();
};
