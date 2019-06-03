#pragma once

#include "GsCameraModeFreeBase.h"
#include "GsControlMode.h"
//--------------------------------------
// follow ��� 
//--------------------------------------

// �ڵ� ȸ�� ó��
// 1. ��ǲ���� 360���� ġȯ�Ѵ�
// 2. Ű�����Է¿����� 8���� ���⸸ �ִ�(0,1,-1 ���� x,y�� ����)
// 2.1 ���̽�ƽ�̸� 360�� �ٵ�
// 3. ���� ���ȿ����� ȸ���Ѵ� ex) 10~100, 260~350
// 4. 180�� �̻��̸� -360���� ���� ũ�Ⱚ ����
// 5. �ִ� ũ����� �������� ó��
// 6. ���� ȸ���� ó�� ����

class UGsGameObjectLocal;

class GsCameraModeFollowBase :public GsCameraModeFreeBase
{
private:
	// �ֱ� x input, yinput
	float _currentXInput, _currentYInput;

	// �ֱ� ȸ�� �Է°�(������)
	float _currentAutoRotInput;
	// ��ǥ ��ǲ��
	float _targetAutoRotInput;
	// ���� ����
	// ������
	const float RightMin = 10.0f;
	const float RightMax = 100.0f;

	// ����
	const float LeftMin = 260.0f;
	const float LeftMax = 350.0f;

	// ȸ�� ũ��
	float AddRotVal = 0.3f;
	// ȸ�� ���� �ӵ�	
	float AutoRotIntpSpeed = 1.5f;

	// ���� ��ġ������??
	bool bTouchOn = false;
public:
	GsCameraModeFollowBase();
	GsCameraModeFollowBase(EGsControlMode In_mode) :GsCameraModeFreeBase(In_mode) {}
	virtual ~GsCameraModeFollowBase();

	// ��������(���ڷ� ĳ����)
	virtual void Update(UGsGameObjectLocal* In_char, float In_deltaTime, GsCameraModeManager* In_mng) override;

	// ��,�Ʒ� �̵� ó��
	virtual void UpDown(float NewAxisValue, UGsGameObjectLocal* In_char) override;
	// ��,�� �̵� ó��
	virtual void LeftRight(float NewAxisValue, UGsGameObjectLocal* In_char) override;

	// �� �̵� ó��
	virtual void MoveForward(UGsGameObjectLocal* In_char) override;
	// �� �̵� ó��
	virtual void MoveBackward(UGsGameObjectLocal* In_char) override;
	// �� �̵� ó��
	virtual void MoveLeft(UGsGameObjectLocal* In_char) override;
	// �� �̵� ó��
	virtual void MoveRight(UGsGameObjectLocal* In_char) override;

	// �̵� ����
	virtual void MoveStop(UGsGameObjectLocal* In_char) override;

	// ���� ��ȯ(���ڷ� ĳ����)
	virtual void Enter(UGsGameObjectLocal* In_char, GsCameraModeManager* In_mng) override;

	// ���� ����(���ڷ� ĳ����)
	virtual void Exit(UGsGameObjectLocal* In_char) override;
	// ��ġ ����
	virtual void TouchPress();
	// ��ġ ��
	virtual void TouchRelease();


	// Ű���� �Է� ���� ���(360��)
	float CalcDegreesKeyBoardInput(float In_x, float In_y)
	{
		float resVal = 0.f;
		// x 0�� ��
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
		// x 1�϶�
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
		// x -1�� ��
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