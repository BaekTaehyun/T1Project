#pragma once
#include "GsCameraModeFollow.h"

//---------------------
// follow action ���
//----------------------


class GsCameraModeFollowAction :public GsCameraModeFollow
{
public:
	GsCameraModeFollowAction();
	virtual ~GsCameraModeFollowAction();

	// ���� ���� ����
	virtual void NextStep(class GsCameraModeManager* In_mng) override;
};