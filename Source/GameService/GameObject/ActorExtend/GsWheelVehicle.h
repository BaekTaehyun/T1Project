// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "GsWheelVehicle.generated.h"

class UCameraComponent;
class USpringArmComponent;

/**
 * ����� Ż�� ó��(������ �ִ�..)
 * ������ Ż�͵� AWheeledVehicle Ŭ������ ��ӹ޾ƾ� �Ұ��ΰ��� ���� ����غ���
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
	class UGsInputBindingBase* InputBinder;

public:
	FORCEINLINE UGsInputBindingBase* GetInputBinder() const			{ return InputBinder; }
	FORCEINLINE void SetInputBinder(UGsInputBindingBase* Binder)	{ InputBinder = Binder; }

public:
	AGsWheelVehicle();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
