#pragma once
#include "GsCameraModeFollow.h"

//---------------------
// follow action 모드
//----------------------


class GsCameraModeFollowAction :public GsCameraModeFollow
{
public:
	GsCameraModeFollowAction();
	virtual ~GsCameraModeFollowAction();

	// 다음 스텝 진행
	virtual void NextStep(class GsCameraModeManager* In_mng) override;
};