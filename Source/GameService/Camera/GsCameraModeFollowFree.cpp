#include "GsCameraModeFollowFree.h"
#include "GsCameraModeManager.h"


GsCameraModeFollowFree::GsCameraModeFollowFree():GsCameraModeFollow(EGsControlMode::Free)
{
	GSLOG(Warning, TEXT("GsCameraModeFollowFree constructor"));
}
// �Ҹ���
GsCameraModeFollowFree::~GsCameraModeFollowFree()
{
	GSLOG(Warning, TEXT("GsCameraModeFollowFree destructor"));
}

// ���� ���� ����
void GsCameraModeFollowFree::NextStep(GsCameraModeManager* In_mng)
{

	if (In_mng == nullptr)
	{
		GSLOG(Error, TEXT("GsCameraModeManager* In_mng == nullptr"));
		return;
	}

	// ������ free��
	In_mng->ChangeState(EGsControlMode::Quater);
}