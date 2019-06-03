#include "GsCameraModeFollowWide.h"
#include "GsCameraModeManager.h"

GsCameraModeFollowWide::GsCameraModeFollowWide():GsCameraModeFollowBase(EGsControlMode::Wide)
{
	GSLOG(Warning, TEXT("GsCameraModeFollowWide constructor"));
}
// �Ҹ���
GsCameraModeFollowWide::~GsCameraModeFollowWide()
{
	GSLOG(Warning, TEXT("GsCameraModeFollowWide destructor"));
}

// ���� ���� ����
void GsCameraModeFollowWide::NextStep(GsCameraModeManager* In_mng)
{

	if (In_mng == nullptr)
	{
		GSLOG(Error, TEXT("GsCameraModeManager* In_mng == nullptr"));
		return;
	}

	// ������ free��
	In_mng->ChangeState(EGsControlMode::Action);
}