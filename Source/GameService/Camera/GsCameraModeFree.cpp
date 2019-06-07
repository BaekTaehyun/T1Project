#include "GsCameraModeFree.h"
#include "GsCameraModeManager.h"


GsCameraModeFree::GsCameraModeFree():GsCameraModeFreeBase(EGsControlMode::Free)
{
	GSLOG(Warning, TEXT("GsCameraModeFree constructor"));
}
// 소멸자
GsCameraModeFree::~GsCameraModeFree()
{
	GSLOG(Warning, TEXT("GsCameraModeFree destructor"));
}

// 다음 스텝 진행
void GsCameraModeFree::NextStep(GsCameraModeManager* In_mng)
{

	if (In_mng == nullptr)
	{
		GSLOG(Error, TEXT("GsCameraModeManager* In_mng == nullptr"));
		return;
	}

	// 다음은 free다
	In_mng->ChangeState(EGsControlMode::Quater);
}