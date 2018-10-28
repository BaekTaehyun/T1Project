// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "T1Project.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "T1Player.generated.h"

class USpringArmComponent;
class UCameraComponent;


UCLASS()
class T1PROJECT_API AT1Player : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AT1Player();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ī�޶��带 ����
	enum class ECameraControlMode
	{
		FOLLOW,
		DIABLO,
	};
	
	ECameraControlMode CurrentCameraControlMode = ECameraControlMode::FOLLOW;
	FVector DirectionToMove = FVector::ZeroVector;

	// ī�޶� ������
	float ArmLengthTo = 0.0f;
	FRotator ArmRotationTo = FRotator::ZeroRotator;
	float ArmLengthSpeed = 0.0f;
	float ArmRotationSpeed = 0.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// �ݸ� ������ �ľ� �ϱ� ���� �������̵�	
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;


	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;
	
private:
	// ������Ʈ �ɼǿ� ��ǲ�� ��Ī�Ǵ� �Լ�
	void UpDown(float newAxisValue);
	void LeftRight(float newAxisValue);
	void Turn(float newAxisValue);
	void LookUp(float newAxisValue);

	void CameraViewChange();
	void SetCameraControlMode(ECameraControlMode inControlMode);
};
