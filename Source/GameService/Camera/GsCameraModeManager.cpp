#include "GsCameraModeManager.h"
#include "../UTIL/GsText.h"
#include "Classes/Engine/DataTable.h"
#include "GsCamModeData.h"

// ��� Ŭ����
#include "GsCameraModeFree.h"
#include "GsCameraModeFollowQuater.h"
#include "GsCameraModeFollowWide.h"
#include "GsCameraModeFollowAction.h"
#include "GsCameraModeFix.h"

//------------------------------------------------
// �Ҵ� �ϴ� ��
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
// ī�޶� ��� ������
//-------------------------------------------------

// �Ҹ���
GsCameraModeManager* GsCameraModeSingle::Instance = nullptr;
GsCameraModeManager::~GsCameraModeManager()
{
	GSLOG(Warning, TEXT("GsCameraModeManager destructor"));
}

// �ʱ�ȭ
void GsCameraModeManager::Initialize()
{
	TGsSingleton::InitInstance(this);
	// ���̺� �б�

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
		// ��� ����
		MakeInstance(iter);


		// ���̺� �ε�
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

// ĳ���� ����
void GsCameraModeManager::SetCharacter(UGsGameObjectLocal* In_char)
{
	// ĳ���� �Ҵ�
	LocalPlayer = In_char;

	// �⺻���� ������ ����
	ChangeState(EGsControlMode::Free);
}

// ���� ����(�� ������Ʈ���� ĳ���͸� ���ڷ� �ѱ�� ����)
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

// ����
void GsCameraModeManager::Update(float In_deltaTime)
{
	if (_currentState.IsValid())
	{
		_currentState.Get()->Update(LocalPlayer, In_deltaTime);
	}
}
// ���� �������� ����
void GsCameraModeManager::NextStep()
{
	if (_currentState.IsValid())
	{
		_currentState.Get()->NextStep(this);
	}
}

// ��忡 ���� ī�޶� ������ ��������
FGsCamModeData* GsCameraModeManager::GetCamModeData(EGsControlMode In_mode)
{
	return _mapModeData.FindRef(In_mode);
}