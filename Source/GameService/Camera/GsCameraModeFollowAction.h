#pragma once
#include "GsCameraModeFollowBase.h"

//---------------------
// follow action ���
//----------------------


class GsCameraModeFollowAction :public GsCameraModeFollowBase
{
public:
	GsCameraModeFollowAction();
	virtual ~GsCameraModeFollowAction();

	// ���� ���� ����
	virtual void NextStep(class GsCameraModeManager* In_mng) override;
};