#include "GsCameraModeFollowFree.h"
#include "GsCameraModeManager.h"


GsCameraModeFollowFree::GsCameraModeFollowFree():GsCameraModeFollow(EGsControlMode::Free)
{
	GSLOG(Warning, TEXT("GsCameraModeFollowFree constructor"));
}
// 소멸자
GsCameraModeFollowFree::~GsCameraModeFollowFree()
{
	GSLOG(Warning, TEXT("GsCameraModeFollowFree destructor"));
}

// 다음 스텝 진행
void GsCameraModeFollowFree::NextStep(GsCameraModeManager* In_mng)
{

	if (In_mng == nullptr)
	{
		GSLOG(Error, TEXT("GsCameraModeManager* In_mng == nullptr"));
		return;
	}

	// 다음은 free다
	In_mng->ChangeState(EGsControlMode::Quater);
}