#pragma once

#include "GsCameraModeFollow.h"

//----------------------------------
// follow free ���
//----------------------------------

class GsCameraModeFollowFree :public GsCameraModeFollow
{
public:
	GsCameraModeFollowFree();
	virtual ~GsCameraModeFollowFree();

	// ���� ���� ����
	virtual void NextStep(class GsCameraModeManager* In_mng) override;
};