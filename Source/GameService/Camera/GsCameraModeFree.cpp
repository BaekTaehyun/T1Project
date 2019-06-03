#include "GsCameraModeFree.h"
#include "GsCameraModeManager.h"


GsCameraModeFree::GsCameraModeFree():GsCameraModeFreeBase(EGsControlMode::Free)
{
	GSLOG(Warning, TEXT("GsCameraModeFree constructor"));
}
// �Ҹ���
GsCameraModeFree::~GsCameraModeFree()
{
	GSLOG(Warning, TEXT("GsCameraModeFree destructor"));
}

// ���� ���� ����
void GsCameraModeFree::NextStep(GsCameraModeManager* In_mng)
{

	if (In_mng == nullptr)
	{
		GSLOG(Error, TEXT("GsCameraModeManager* In_mng == nullptr"));
		return;
	}

	// ������ free��
	In_mng->ChangeState(EGsControlMode::Quater);
}