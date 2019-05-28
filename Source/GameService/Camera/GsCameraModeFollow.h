#pragma once

#include "GsCameraModeBase.h"
#include "GsControlMode.h"
//--------------------------------------
// follow 모드 
//--------------------------------------
class GsCameraModeManager;
class GsCameraModeFollow :public GsCameraModeBase
{
public:
	GsCameraModeFollow();
	GsCameraModeFollow(EGsControlMode In_mode) :GsCameraModeBase(In_mode) {}
	virtual ~GsCameraModeFollow();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;

	// 상태 전환(인자로 캐릭터)
	virtual void Enter(ACharacter* In_char, GsCameraModeManager* In_mng) override;
	// 상태 종료(인자로 캐릭터)
	virtual void Exit(ACharacter* In_char) override;
	// 업데이터(인자로 캐릭터)
	virtual void Update(ACharacter* In_char, float In_deltaTime) override;

	// 위,아래 이동 처리
	virtual void UpDown(float NewAxisValue, ACharacter* In_char) override;
	// 좌,우 이동 처리
	virtual void LeftRight(float NewAxisValue, ACharacter* In_char) override;
	// 위, 아래 카메라 회전
	virtual void LookUp(float NewAxisValue, ACharacter* In_char) override;
	// 좌, 우 카메라 회전
	virtual void Turn(float NewAxisValue, ACharacter* In_char) override;
};