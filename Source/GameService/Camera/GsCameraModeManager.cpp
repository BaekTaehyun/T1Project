#include "GsCameraModeManager.h"
#include "../UTIL/GsText.h"
#include "Classes/Engine/DataTable.h"
#include "GsCamModeData.h"

// 모드 클래스
#include "GsCameraModeFree.h"
#include "GsCameraModeFollowQuater.h"
#include "GsCameraModeFollowWide.h"
#include "GsCameraModeFollowAction.h"
#include "GsCameraModeFix.h"

//------------------------------------------------
// 할당 하는 넘
//------------------------------------------------
GsCameraModeBase* GsCameraModeAllocator::Alloc(EGsControlMode In_mode)
{
	if (In_mode == EGsControlMode::Free)
	{
		return new GsCameraModeFree();
	}
	else if (In_mode == EGsControlMode::Quater)
	{
		return new GsCameraModeFollowQuater();
	}
	else if (In_mode == EGsControlMode::Wide)
	{
		return new GsCameraModeFollowWide();
	}
	else if (In_mode == EGsControlMode::Action)
	{
		return new GsCameraModeFollowAction();
	}
	else if (In_mode == EGsControlMode::Fixed)
	{
		return new GsCameraModeFix();
	}

	return nullptr;
}
//-------------------------------------------------
// 카메라 모드 관리자
//-------------------------------------------------

// 소멸자
GsCameraModeManager* GsCameraModeSingle::Instance = nullptr;
GsCameraModeManager::~GsCameraModeManager()
{
	GSLOG(Warning, TEXT("GsCameraModeManager destructor"));
}

// 초기화
void GsCameraModeManager::Initialize()
{
	TGsSingleton::InitInstance(this);
	// 테이블 읽기

	static ConstructorHelpers::FObjectFinder<UDataTable> TBL(
		TEXT("/Game/Game/Camera/CamModeData.CamModeData"));

	CamModeData = TBL.Object;

	TArray<EGsControlMode> arrMode;

	arrMode.Add(EGsControlMode::Free);
	arrMode.Add(EGsControlMode::Quater);
	arrMode.Add(EGsControlMode::Wide);
	arrMode.Add(EGsControlMode::Action);
	arrMode.Add(EGsControlMode::Fixed);

	
	for (auto& iter : arrMode)
	{
		// 모드 생성
		MakeInstance(iter);


		// 테이블 로드
		FString rowNameStr = EnumToString(EGsControlMode, iter);

		FString cutStr =
			rowNameStr.Replace(TEXT("EGsControlMode::"), TEXT(""));

		GSLOG(Warning, TEXT("cutStr: %s"), *cutStr);

		FName rowName = FName(*cutStr);

		FGsCamModeData* RowLookUp =
			CamModeData->FindRow<FGsCamModeData>(rowName, "");

		if (RowLookUp == nullptr)
		{
			GSLOG(Error, TEXT("Mode: %s data is nullptr"), *cutStr);
		}
		else
		{
			GSLOG(Warning, TEXT("Mode: %s, ArmLengthTo: %f, ControllerPitch: %f"),
				*EnumToString(EGsControlMode, RowLookUp->Mode),
				RowLookUp->ArmLengthTo,
				RowLookUp->ControllerPitch);

			_mapModeData.Add(iter, RowLookUp);
		}
	}
}

// 캐릭터 세팅
void GsCameraModeManager::SetCharacter(UGsGameObjectLocal* In_char)
{
	// 캐릭터 할당
	LocalPlayer = In_char;

	// 기본으로 프리로 세팅
	ChangeState(EGsControlMode::Free);
}

// 상태 변경(각 스테이트한테 캐릭터를 인자로 넘기는 버전)
void GsCameraModeManager::ChangeState(EGsControlMode In_state)
{
	GSLOG(Warning, TEXT("ChangeState [%s] "),
		*EnumToString(EGsControlMode, In_state));

	auto instance = Find(In_state);

	if (_currentState.IsValid())
	{
		_currentState.Get()->Exit(LocalPlayer);
		_onLeaveState.Broadcast(_currentState.Get()->GetType());
		GSLOG(Warning, TEXT("GSTStateMng : exit state [%s] Exit"), 
			*EnumToString(EGsControlMode, _currentState.Get()->GetType()));
	}

	_currentState = instance;

	if (_currentState.IsValid())
	{
		_currentState.Get()->Enter(LocalPlayer, this);
		_onEnterState.Broadcast(_currentState.Get()->GetType());
		GSLOG(Warning, TEXT("`GSTStateMng : enter state [%s] Enter"), 			
			*EnumToString(EGsControlMode, _currentState.Get()->GetType()));
	}
}

// 갱신
void GsCameraModeManager::Update(float In_deltaTime)
{
	if (_currentState.IsValid())
	{
		_currentState.Get()->Update(LocalPlayer, In_deltaTime);
	}
}
// 다음 스텝으로 진행
void GsCameraModeManager::NextStep()
{
	if (_currentState.IsValid())
	{
		_currentState.Get()->NextStep(this);
	}
}

// 모드에 따른 카메라 데이터 가져오기
FGsCamModeData* GsCameraModeManager::GetCamModeData(EGsControlMode In_mode)
{
	return _mapModeData.FindRef(In_mode);
}