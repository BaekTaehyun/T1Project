#pragma once

#include "GsCameraModeFreeBase.h"

//----------------------------------
// follow free ���
//----------------------------------

class GsCameraModeFree :public GsCameraModeFreeBase
{
public:
	GsCameraModeFree();
	virtual ~GsCameraModeFree();

	// ���� ���� ����
	virtual void NextStep(class GsCameraModeManager* In_mng) override;
};