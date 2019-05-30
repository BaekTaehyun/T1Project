#pragma once

#include "GsCameraModeFollow.h"

//----------------------------------
// follow free 모드
//----------------------------------

class GsCameraModeFollowFree :public GsCameraModeFollow
{
public:
	GsCameraModeFollowFree();
	virtual ~GsCameraModeFollowFree();

	// 다음 스텝 진행
	virtual void NextStep(class GsCameraModeManager* In_mng) override;
};