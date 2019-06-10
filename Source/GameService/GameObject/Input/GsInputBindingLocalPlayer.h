// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObject/Input/GsInputBindingBase.h"
#include "GameObject/Parts/Data/GsPartsDataBase.h"
#include "GsInputBindingLocalPlayer.generated.h"

/**
 * 
 */
#define CAM_MODE

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

	// forward 에서 호출할 tfunction(모드에 따라 변경)
	TFunction<void(float In_newAxisValue)> FunctionMoveForward;
	// backward 에서 호출할 tfunction(모드에 따라 변경)
	TFunction<void(float In_newAxisValue)> FunctionMoveBackward;
	// Left 에서 호출할 tfunction(모드에 따라 변경)
	TFunction<void(float In_newAxisValue)> FunctionMoveLeft;
	// Right 에서 호출할 tfunction(모드에 따라 변경)
	TFunction<void(float In_newAxisValue)> FunctionMoveRight;
	// stop 에서 호출할 tfunction(모드에 따라 변경)
	TFunction<void()> FunctionMoveStop;
	// LookUp 에서 호출할 tfunction(모드에 따라 변경)
	TFunction<void(float In_newAxisValue)> FunctionLookUp;
	// Turn 에서 호출할 tfunction(모드에 따라 변경)
	TFunction<void(float In_newAxisValue)> FunctionTurn;

	// ZoomIn 에서 호출할 tfunction(모드에 따라 변경)
	TFunction<void()> FunctionZoomIn;
	// ZoomOut 에서 호출할 tfunction(모드에 따라 변경)
	TFunction<void()> FunctionZoomOut;
	// 터치 시작 에서 호출할 tfunction(모드에 따라 변경)
	TFunction<void()> FunctionTouchPress;
	// 터치 끝 에서 호출할 tfunction(모드에 따라 변경)
	TFunction<void()> FunctionTouchRelease;
protected:
	//Local Character Bind
	void OnAttachParts(EGsPartsType Type);
	void OnAttack1(int32 slot);
	void OnAction();
	void OnMoveStop();
	void OnMoveForward(float Value);
	void OnMoveBackward(float Value);
	void OnMoveLeft(float Value);
	void OnMoveRight(float Value);
	void OnMoveRotate(float Value);
	void OnMoveRotateYaw(float Value);
	void OnMoveRotatePitch(float Value);
	// 터치 시작(pc는 좌클릭)
	void OnTouchPress();
	// 터치 끝(pc는 좌클릭)
	void OnTouchRelease();

	// 줌인
	void OnZoomIn();
	// 줌아웃
	void OnZoomOut();
	// 카메로 모드 변경
	void OnViewChange();
	//

protected:
	class UGsGameObjectLocal* Target;
};
