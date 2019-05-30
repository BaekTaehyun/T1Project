#pragma once

#include "GsCameraModeBase.h"
#include "GsControlMode.h"
//--------------------------------------
// follow ��� 
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

	// ���� ��ȯ(���ڷ� ĳ����)
	virtual void Enter(ACharacter* In_char, GsCameraModeManager* In_mng) override;
	// ���� ����(���ڷ� ĳ����)
	virtual void Exit(ACharacter* In_char) override;
	// ��������(���ڷ� ĳ����)
	virtual void Update(ACharacter* In_char, float In_deltaTime) override;

	// ��,�Ʒ� �̵� ó��
	virtual void UpDown(float NewAxisValue, ACharacter* In_char) override;
	// ��,�� �̵� ó��
	virtual void LeftRight(float NewAxisValue, ACharacter* In_char) override;
	// ��, �Ʒ� ī�޶� ȸ��
	virtual void LookUp(float NewAxisValue, ACharacter* In_char) override;
	// ��, �� ī�޶� ȸ��
	virtual void Turn(float NewAxisValue, ACharacter* In_char) override;
};