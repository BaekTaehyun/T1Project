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

	// ���� ���� ����
	virtual void NextStep(class GsCameraModeManager* In_mng) override;
};