#pragma once
#include "GsCameraModeFollow.h"

//-----------------------------
// follow wide ���
//-----------------------------

class GsCameraModeFollowWide :public GsCameraModeFollow
{
public: 
	GsCameraModeFollowWide();
	virtual ~GsCameraModeFollowWide();

	// ���� ���� ����
	virtual void NextStep(class GsCameraModeManager* In_mng) override;
};