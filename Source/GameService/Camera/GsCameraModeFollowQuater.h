#pragma once

#include "GsCameraModeFollowBase.h"

//----------------------------------
// follow quater
//----------------------------------

class GsCameraModeFollowQuater :public GsCameraModeFollowBase
{
public:
	GsCameraModeFollowQuater();
	virtual ~GsCameraModeFollowQuater();

	// 다음 스텝 진행
	virtual void NextStep(class GsCameraModeManager* In_mng) override;
};