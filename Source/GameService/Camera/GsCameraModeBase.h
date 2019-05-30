#pragma once

#include "GsControlMode.h"
#include "../Class/GsState.h"


//----------------------------------------------------------------------
// ī�޶� ��� ���
//----------------------------------------------------------------------

class GsCameraModeManager;
class GsCameraModeBase : public TGsState<EGsControlMode>
{
public :
	GsCameraModeBase() : TGsState<EGsControlMode>(EGsControlMode::Free) {}
	GsCameraModeBase(EGsControlMode In_mode) :TGsState<EGsControlMode>(In_mode) {}
	virtual ~GsCameraModeBase() {}
	virtual void Enter() override {};
	virtual void Exit() override {};
	virtual void Update() override {};

	// ���� ��ȯ(���ڷ� ĳ����)
	virtual void Enter(ACharacter* In_char, GsCameraModeManager* In_mng);
	// ���� ����(���ڷ� ĳ����)
	virtual void Exit(ACharacter* In_char);
	// ��������(���ڷ� ĳ����)
	virtual void Update(ACharacter* In_char, float In_deltaTime);

	// ��,�Ʒ� �̵� ó��
	virtual void UpDown(float NewAxisValue, ACharacter* In_char) {}
	// ��,�� �̵� ó��
	virtual void LeftRight(float NewAxisValue, ACharacter* In_char) {}
	// ��, �Ʒ� ī�޶� ȸ��
	virtual void LookUp(float NewAxisValue, ACharacter* In_char) {}
	// ��, �� ī�޶� ȸ��
	virtual void Turn(float NewAxisValue, ACharacter* In_char) {}
	// ����
	virtual void ZoomIn();
	// �ܾƿ�
	virtual void ZoomOut();
	// ���� ���� ����(�Ŵ��� �ѱ�)
	virtual void NextStep(GsCameraModeManager* In_mng) {}

	// �ܾƿ� ����
	const float ZoomInOutPower = 20.0f;

	// ������ �� �Ÿ� �ִ�ġ
	const float ArmLengthMax = 1000.0f;
	// ������ �� �Ÿ� �ּ�ġ
	const float ArmLengthMin = 300.0f;

	// ������ �� �Ÿ� ���� �ӵ�
	const float ArmLengthSpeed = 3.0f;

private:

	// ������ �� �Ÿ� ��ǥ��
	float _armLengthTo = 0.0f;
};