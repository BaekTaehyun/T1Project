#pragma once

#include "GsCameraModeBase.h"

//--------------------------------------
// fix 모드 
//--------------------------------------
class GsCameraModeManager;
class UGsGameObjectLocal;

class GsCameraModeFix :public GsCameraModeBase
{

public:
	GsCameraModeFix();
	virtual ~GsCameraModeFix();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;

	// 상태 전환(인자로 캐릭터)
	virtual void Enter(UGsGameObjectLocal* In_char, GsCameraModeManager* In_mng) override;
	// 상태 종료(인자로 캐릭터)
	virtual void Exit(UGsGameObjectLocal* In_char) override;
	// 업데이터(인자로 캐릭터)
	virtual void Update(UGsGameObjectLocal* In_char, float In_deltaTime, GsCameraModeManager* In_mng) override;

	// 위,아래 이동 처리
	virtual void UpDown(float NewAxisValue, UGsGameObjectLocal* In_char) override;
	// 좌,우 이동 처리
	virtual void LeftRight(float NewAxisValue, UGsGameObjectLocal* In_char) override;

	// 앞 이동 처리
	virtual void MoveForward(UGsGameObjectLocal* In_char) override;
	// 뒤 이동 처리
	virtual void MoveBackward(UGsGameObjectLocal* In_char) override;
	// 좌 이동 처리
	virtual void MoveLeft(UGsGameObjectLocal* In_char) override;
	// 우 이동 처리
	virtual void MoveRight(UGsGameObjectLocal* In_char) override;

	// 위, 아래 카메라 회전
	virtual void LookUp(float NewAxisValue, UGsGameObjectLocal* In_char) override;
	// 좌, 우 카메라 회전
	virtual void Turn(float NewAxisValue, UGsGameObjectLocal* In_char) override;
	// 다음 스텝 진행
	virtual void NextStep(GsCameraModeManager* In_mng) override;

	// 이동 정지
	virtual void MoveStop(UGsGameObjectLocal* In_char) override;

	// 스프링 암 회전 보간 속도
	const float ArmRotationSpeed = 10.0f;
private:
	// fixed 모드 이동 백업 값
	FVector _directionToMove = FVector::ZeroVector;

	// 스핑암 회전 목표값(fixed로 변경될때 보간위한 값)
	FRotator _armRotationTo = FRotator::ZeroRotator;
};