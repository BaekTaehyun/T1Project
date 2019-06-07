#pragma once

#include "GsCameraModeFreeBase.h"

//----------------------------------
// follow free 모드
//----------------------------------

class GsCameraModeFree :public GsCameraModeFreeBase
{
public:
	GsCameraModeFree();
	virtual ~GsCameraModeFree();

	// 다음 스텝 진행
	virtual void NextStep(class GsCameraModeManager* In_mng) override;
};