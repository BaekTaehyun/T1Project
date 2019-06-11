// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObject/Input/GsInputBindingBase.h"
#include "GsInputBindingVehicle.generated.h"

class UGsGameObjectWheelVehicle;
class UWheeledVehicleMovementComponent;

/**
 * 탈것 관련 키입력 관련 바인딩 클래스 (테스트 목적이 강함)
 */
UCLASS()
class GAMESERVICE_API UGsInputBindingVehicle : public UGsInputBindingBase
{
	GENERATED_BODY()
	
public:
	UGsInputBindingVehicle();
	virtual ~UGsInputBindingVehicle();

	virtual void Initialize() override;
	virtual void Initialize(UGsGameObjectWheelVehicle* target);
	virtual void SetBinding(UInputComponent* input) override;

protected:
	void OnMoveForward();
	void OnMoveBackward();
	void OnMoveRight(float Val);
	void OnAction();

protected:
	UGsGameObjectWheelVehicle* Target;
	UWheeledVehicleMovementComponent* MovementComponent;
};
