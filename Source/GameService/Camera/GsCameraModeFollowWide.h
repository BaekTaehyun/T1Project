#pragma once
#include "GsCameraModeFollowBase.h"

//-----------------------------
// follow wide 모드
//-----------------------------

class GsCameraModeFollowWide :public GsCameraModeFollowBase
{
public: 
	GsCameraModeFollowWide();
	virtual ~GsCameraModeFollowWide();

	// 다음 스텝 진행
	virtual void NextStep(class GsCameraModeManager* In_mng) override;
};