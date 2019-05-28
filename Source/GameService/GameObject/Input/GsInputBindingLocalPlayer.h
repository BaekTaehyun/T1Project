// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObject/Input/GsInputBindingBase.h"
#include "GameObject/Parts/Data/GsPartsDataBase.h"
#include "GsInputBindingLocalPlayer.generated.h"

/**
 * 
 */
UCLASS(DefaultToInstanced)
class GAMESERVICE_API UGsInputBindingLocalPlayer : public UGsInputBindingBase
{
	GENERATED_BODY()
	
	DECLARE_DELEGATE_OneParam(FOnAttack1, int32);
	DECLARE_DELEGATE_OneParam(FOnAttachParts, EGsPartsType);

public:
	virtual void Initialize() override;
	virtual void Initialize(class UGsGameObjectLocal* target);
	virtual void SetBinding(UInputComponent* input) override;

protected:
	//Local Character Bind
	void OnAttachParts(EGsPartsType Type);
	void OnAttack1(int32 slot);
	void OnAction();
	void OnMoveForward();
	void OnMoveBackward();
	void OnMoveLeft();
	void OnMoveRight();
	void OnMoveStop();
	void OnMoveRotate(float Value);
	void OnMoveRotateYaw(float Value);
	void OnMoveRotatePitch(float Value);
	//

protected:
	class UGsGameObjectLocal* Target;
};
