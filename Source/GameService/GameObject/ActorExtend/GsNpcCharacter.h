// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GsNpcCharacter.generated.h"

/**
* ACharacter를 상속받은 NoneCharacter
*/
UCLASS()
class GAMESERVICE_API AGsNpcCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGsNpcCharacter();
	virtual ~AGsNpcCharacter();

    // Called every frame
    virtual void Tick(float DeltaTime) override;
    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
