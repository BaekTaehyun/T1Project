#pragma once


#include "GsCameraModeBase.h"
#include "GsControlMode.h"
//--------------------------------------
// free base
// ȸ�� ����
//--------------------------------------

class GsCameraModeManager;
class UGsGameObjectLocal;

class GsCameraModeFreeBase :public GsCameraModeBase
{
public:
	GsCameraModeFreeBase();
	GsCameraModeFreeBase(EGsControlMode In_mode) :GsCameraModeBase(In_mode) {}
	virtual ~GsCameraModeFreeBase();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;

	// ���� ��ȯ(���ڷ� ĳ����)
	virtual void Enter(UGsGameObjectLocal* In_char, GsCameraModeManager* In_mng) override;
	// ���� ����(���ڷ� ĳ����)
	virtual void Exit(UGsGameObjectLocal* In_char) override;
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


	// ��, �Ʒ� ī�޶� ȸ��
	virtual void LookUp(float NewAxisValue, UGsGameObjectLocal* In_char) override;
	// ��, �� ī�޶� ȸ��
	virtual void Turn(float NewAxisValue, UGsGameObjectLocal* In_char) override;
};