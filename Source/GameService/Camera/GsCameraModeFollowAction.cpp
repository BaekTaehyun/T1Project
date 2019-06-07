#include "GsCameraModeFollowAction.h"
#include "GsCameraModeManager.h"
#include "GsControlMode.h"

GsCameraModeFollowAction::GsCameraModeFollowAction() : GsCameraModeFollowBase(EGsControlMode::Action)
{
	GSLOG(Warning, TEXT("GsCameraModeFollowAction construct"));
}

// �Ҹ���
GsCameraModeFollowAction::~GsCameraModeFollowAction()
{
	GSLOG(Warning, TEXT("GsCameraModeFollowAction destructor"));
}

// ���� ���� ����
void GsCameraModeFollowAction::NextStep(GsCameraModeManager* In_mng)
{

	if (In_mng == nullptr)
	{
		GSLOG(Error, TEXT("GsCameraModeManager* In_mng == nullptr"));
		return;
	}

	// ������ free��
	In_mng->ChangeState(EGsControlMode::Free);
}