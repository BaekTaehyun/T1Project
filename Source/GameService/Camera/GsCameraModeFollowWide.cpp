#include "GsCameraModeFollowWide.h"
#include "GsCameraModeManager.h"

GsCameraModeFollowWide::GsCameraModeFollowWide():GsCameraModeFollowBase(EGsControlMode::Wide)
{
	GSLOG(Warning, TEXT("GsCameraModeFollowWide constructor"));
}
// 소멸자
GsCameraModeFollowWide::~GsCameraModeFollowWide()
{
	GSLOG(Warning, TEXT("GsCameraModeFollowWide destructor"));
}

// 다음 스텝 진행
void GsCameraModeFollowWide::NextStep(GsCameraModeManager* In_mng)
{

	if (In_mng == nullptr)
	{
		GSLOG(Error, TEXT("GsCameraModeManager* In_mng == nullptr"));
		return;
	}

	// 다음은 free다
	In_mng->ChangeState(EGsControlMode::Action);
}