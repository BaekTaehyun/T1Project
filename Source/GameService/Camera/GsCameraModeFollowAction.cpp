#include "GsCameraModeFollowAction.h"
#include "GsCameraModeManager.h"
#include "GsControlMode.h"

GsCameraModeFollowAction::GsCameraModeFollowAction() : GsCameraModeFollowBase(EGsControlMode::Action)
{
	GSLOG(Warning, TEXT("GsCameraModeFollowAction construct"));
}

// 소멸자
GsCameraModeFollowAction::~GsCameraModeFollowAction()
{
	GSLOG(Warning, TEXT("GsCameraModeFollowAction destructor"));
}

// 다음 스텝 진행
void GsCameraModeFollowAction::NextStep(GsCameraModeManager* In_mng)
{

	if (In_mng == nullptr)
	{
		GSLOG(Error, TEXT("GsCameraModeManager* In_mng == nullptr"));
		return;
	}

	// 다음은 free다
	In_mng->ChangeState(EGsControlMode::Free);
}