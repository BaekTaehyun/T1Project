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

	// ���� ���� ����
	virtual void NextStep(class GsCameraModeManager* In_mng) override;
};