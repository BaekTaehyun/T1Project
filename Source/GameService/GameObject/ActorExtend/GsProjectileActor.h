// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GsProjectileActor.generated.h"

UCLASS()
class GAMESERVICE_API AGsProjectileActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGsProjectileActor();
	virtual void PostInitializeComponents() override;
    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    //UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    //UProjectileMovementComponent* ProjectileMovementComponent;
};
