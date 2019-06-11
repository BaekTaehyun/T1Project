// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "GsWheelVehicle.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UGsInputBindingBase;

/**
 * 기계형 탈것 처리(바퀴가 있는..)
 * 생물형 탈것은 R&D 필요해 보임
 * AWheeledVehicle 클래스를 상속받아야 할것인가는 추후 고민해볼것
 * 조작 제어를 위해 UGsInputBindingBase를 설정하게 구성
 */
UCLASS()
class GAMESERVICE_API AGsWheelVehicle : public AWheeledVehicle
{
	GENERATED_BODY()
	
	/** Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;
	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;
	//InputBinder
	UGsInputBindingBase* InputBinder;

public:
	FORCEINLINE UGsInputBindingBase* GetInputBinder() const			{ return InputBinder; }
	FORCEINLINE void SetInputBinder(UGsInputBindingBase* Binder)	{ InputBinder = Binder; }

public:
	AGsWheelVehicle();
	virtual ~AGsWheelVehicle();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
