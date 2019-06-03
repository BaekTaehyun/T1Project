#pragma once

#include "GsCameraModeFreeBase.h"
#include "GsControlMode.h"
//--------------------------------------
// follow 모드 
//--------------------------------------

// 자동 회전 처리
// 1. 인풋값을 360도로 치환한다
// 2. 키보드입력에서는 8가지 방향만 있다(0,1,-1 값을 x,y로 가짐)
// 2.1 조이스틱이면 360도 다됨
// 3. 제한 값안에서만 회전한다 ex) 10~100, 260~350
// 4. 180도 이상이면 -360도를 빼서 크기값 만듬
// 5. 최대 크기까지 보간으로 처리
// 6. 수동 회전시 처리 안함

class UGsGameObjectLocal;

class GsCameraModeFollowBase :public GsCameraModeFreeBase
{
private:
	// 최근 x input, yinput
	float _currentXInput, _currentYInput;

	// 최근 회전 입력값(보간중)
	float _currentAutoRotInput;
	// 목표 인풋값
	float _targetAutoRotInput;
	// 제한 각도
	// 오른쪽
	const float RightMin = 10.0f;
	const float RightMax = 100.0f;

	// 왼쪽
	const float LeftMin = 260.0f;
	const float LeftMax = 350.0f;

	// 회전 크기
	float AddRotVal = 0.3f;
	// 회전 보간 속도	
	float AutoRotIntpSpeed = 1.5f;

	// 현재 터치중인지??
	bool bTouchOn = false;
public:
	GsCameraModeFollowBase();
	GsCameraModeFollowBase(EGsControlMode In_mode) :GsCameraModeFreeBase(In_mode) {}
	virtual ~GsCameraModeFollowBase();

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

	// 이동 정지
	virtual void MoveStop(UGsGameObjectLocal* In_char) override;

	// 상태 전환(인자로 캐릭터)
	virtual void Enter(UGsGameObjectLocal* In_char, GsCameraModeManager* In_mng) override;

	// 상태 종료(인자로 캐릭터)
	virtual void Exit(UGsGameObjectLocal* In_char) override;
	// 터치 시작
	virtual void TouchPress();
	// 터치 끝
	virtual void TouchRelease();


	// 키보드 입력 각도 계산(360도)
	float CalcDegreesKeyBoardInput(float In_x, float In_y)
	{
		float resVal = 0.f;
		// x 0일 때
		if (In_x == 0.0f)
		{
			if (In_y == 1.0f)
			{
				resVal = 0.0f;
			}
			else if(In_y == -1.0f)
			{
				resVal = 180.0f;
			}
		}
		// x 1일때
		else if (In_x == 1.0f)
		{
			if (In_y == 1.0f)
			{
				resVal = 45.0f;
			}
			else if (In_y == 0.0f)
			{
				resVal = 90.0f;
			}
			else if (In_y == -1.0f)
			{
				resVal = 135.0f;
			}
		}
		// x -1일 때
		else if (In_x == -1.0f)
		{
			if (In_y == -1.0f)
			{
				resVal = 225.0f;
			}
			else if (In_y == 0.0f)
			{
				resVal = 270.0f;
			}
			else if (In_y == 1.0f)
			{
				resVal = 315.0f;
			}
		}

		return resVal;
	}
};