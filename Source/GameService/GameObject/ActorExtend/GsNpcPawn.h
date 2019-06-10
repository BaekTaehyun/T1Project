// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameObject/Component/GsAnimInstanceState.h"
#include "GsNpcPawn.generated.h"

class UCapsuleComponent;

/**
* APawn을 상속받은 NoneCharacter
* 이클래스는 USkeletalMeshComponent, UPawnMovementComponent의 동작을 하는 APawn클래스
* 외형및, 운동능력 장착 가능
* 이런 형태의 NPC객체들은 ACharacter를 상속 받은 클래스로 처리할지 고민이 필요해 보임
*/
UCLASS()
class GAMESERVICE_API AGsNpcPawn : public APawn
{
	GENERATED_BODY()
	
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* CapsuleComponent;
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPawnMovementComponent* MovementComponent;
	//Anim
	UGsAnimInstanceState* Animation;

public:
	FORCEINLINE UGsAnimInstanceState* GetAnim() const { return Animation; }
	FORCEINLINE UPawnMovementComponent* GetPawnMovement() const { return MovementComponent; }
	
public:
	// Sets default values for this pawn's properties
	AGsNpcPawn();
	virtual ~AGsNpcPawn();

    // Called every frame
    virtual void Tick(float DeltaTime) override;
    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void PostInitializeComponents() override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
