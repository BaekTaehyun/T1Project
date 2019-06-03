#pragma once
#include "GsCameraModeFollowBase.h"

//---------------------
// follow action 모드
//----------------------


class GsCameraModeFollowAction :public GsCameraModeFollowBase
{
public:
	GsCameraModeFollowAction();
	virtual ~GsCameraModeFollowAction();

	// 다음 스텝 진행
	virtual void NextStep(class GsCameraModeManager* In_mng) override;
};