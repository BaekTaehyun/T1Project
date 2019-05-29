#pragma once

#include "GsCameraModeFollow.h"

//----------------------------------
// follow quater
//----------------------------------

class GsCameraModeFollowQuater :public GsCameraModeFollow
{
public:
	GsCameraModeFollowQuater();
	virtual ~GsCameraModeFollowQuater();

	// 다음 스텝 진행
	virtual void NextStep(class GsCameraModeManager* In_mng) override;
};