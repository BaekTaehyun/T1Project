#include "GsCameraModeFollowQuater.h"
#include "GsCameraModeManager.h"

GsCameraModeFollowQuater::GsCameraModeFollowQuater():GsCameraModeFollow(EGsControlMode::Quater)
{
	GSLOG(Warning, TEXT("GsCameraModeFollowerQuater construrctor"));
}

GsCameraModeFollowQuater::~GsCameraModeFollowQuater()
{
	GSLOG(Warning, TEXT("GsCameraModeFollowerQuater destructor"));
}

// ���� ���� ����
void GsCameraModeFollowQuater::NextStep(GsCameraModeManager* In_mng)
{

	if (In_mng == nullptr)
	{
		GSLOG(Error, TEXT("GsCameraModeManager* In_mng == nullptr"));
		return;
	}

	// ������ free��
	In_mng->ChangeState(EGsControlMode::Wide);
}