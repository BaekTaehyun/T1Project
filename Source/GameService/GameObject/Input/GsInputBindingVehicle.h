// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObject/Input/GsInputBindingBase.h"
#include "GsInputBindingVehicle.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API UGsInputBindingVehicle : public UGsInputBindingBase
{
	GENERATED_BODY()
	
public:
	virtual void Initialize() override;
	virtual void Initialize(class UGsGameObjectWheelVehicle* target);
	virtual void SetBinding(UInputComponent* input) override;

protected:
	void OnMoveForward();
	void OnMoveBackward();
	void OnMoveRight(float Val);
	void OnAction();

protected:
	class UGsGameObjectWheelVehicle* Target;
	class UWheeledVehicleMovementComponent* MovementComponent;
	
};
