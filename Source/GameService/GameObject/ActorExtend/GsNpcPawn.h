// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameObject/Component/Animation/GsAnimInstanceState.h"
#include "GsNpcPawn.generated.h"

class UCapsuleComponent;

/**
* APawn을 상속받은 NoneCharacter
*/
UCLASS()
class GAMESERVICE_API AGsNpcPawn : public APawn
{
	GENERATED_BODY()
	
	// Sets default values for this pawn's properties
	AGsNpcPawn();
    // Called every frame
    virtual void Tick(float DeltaTime) override;
    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void PostInitializeComponents() override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	FORCEINLINE UGsAnimInstanceState* GetAnim() const { return Animation; }
    FORCEINLINE UPawnMovementComponent* GetPawnMovement() const { return MovementComponent; }

private:
    UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* Mesh;
    UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UCapsuleComponent* CapsuleComponent;
    UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UPawnMovementComponent* MovementComponent;
	//Anim
	UGsAnimInstanceState* Animation;
};
