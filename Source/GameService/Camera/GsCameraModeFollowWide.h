#pragma once
#include "GsCameraModeFollowBase.h"

//-----------------------------
// follow wide ���
//-----------------------------

class GsCameraModeFollowWide :public GsCameraModeFollowBase
{
public: 
	GsCameraModeFollowWide();
	virtual ~GsCameraModeFollowWide();

	// ���� ���� ����
	virtual void NextStep(class GsCameraModeManager* In_mng) override;
};