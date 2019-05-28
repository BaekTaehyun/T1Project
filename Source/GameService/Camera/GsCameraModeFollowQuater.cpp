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

// 다음 스텝 진행
void GsCameraModeFollowQuater::NextStep(GsCameraModeManager* In_mng)
{

	if (In_mng == nullptr)
	{
		GSLOG(Error, TEXT("GsCameraModeManager* In_mng == nullptr"));
		return;
	}

	// 다음은 free다
	In_mng->ChangeState(EGsControlMode::Wide);
}