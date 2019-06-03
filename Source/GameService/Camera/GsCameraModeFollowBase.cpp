#include "GsCameraModeFollowBase.h"
#include "GameService.h"
#include "GsCameraModeManager.h"
#include "EngineMinimal.h"

#include "../GameObject/ObjectClass/GsGameObjectLocal.h"
#include "../GameObject/ActorExtend/GsLocalCharacter.h"
#include "../GameObject/Input/GsInputBindingLocalPlayer.h"

//#define TEST_LOG

GsCameraModeFollowBase::GsCameraModeFollowBase()
{
	GSLOG(Warning, TEXT("GsCameraModeFollowBase constructor"));
}
GsCameraModeFollowBase::~GsCameraModeFollowBase()
{
	GSLOG(Warning, TEXT("GsCameraModeFollowBase destructor"));
}

// ��������(���ڷ� ĳ����)
void GsCameraModeFollowBase::Update(UGsGameObjectLocal* In_char, float In_deltaTime)
{
	GsCameraModeFreeBase::Update(In_char, In_deltaTime);


#ifdef TEST_LOG
	GSLOG(Warning,
		TEXT("_currentXInput: %f, _currentYInput: %f, "),
		_currentXInput,
		_currentYInput, 
		bTouchOn);
#endif

	// �Է��� �ִٸ�
	// ��ġ�� �ȵǾ��־����
	if ((_currentXInput != 0.0f || _currentYInput != 0) && (bTouchOn == false))
	{
		float inputDegrees = CalcDegreesKeyBoardInput(_currentXInput, _currentYInput);


#ifdef TEST_LOG
		GSLOG(Warning, 
			TEXT("now Degrees: %f"), 
			inputDegrees);
#endif

		float calcDegrees = 0.0f;
		// ���̽�ƽ ���� ������ üũ(���� �ȿ���)
		if (inputDegrees >= RightMin &&
			inputDegrees <= RightMax)
		{
			calcDegrees = inputDegrees;
		}

		// ���̽�ƽ ���� ���� üũ(���� �ȿ���)
		if (inputDegrees >= LeftMin &&
			inputDegrees <= LeftMax)
		{
			// 180�� �̻��� 360���� ���� ������ ����
			calcDegrees = inputDegrees - 360;
		}


#ifdef TEST_LOG
		GSLOG(Warning, TEXT("calcDegrees: %f"), calcDegrees);
#endif

		// ���� 0���� ������ ó���� �ʿ� ����
		if (calcDegrees == 0.0f)
		{
			// �ڵ� ȸ�� �ֱٰ� �ʱ�ȭ
			_currentAutoRotInput = 0;
		}
		else
		{
			// �ִ� 100�̹Ƿ� 100 ���� ������ �ִ� 1�� ����
			// �ű�� ����� ��
			_targetAutoRotInput = calcDegrees * 0.01f * AddRotVal;


			// �ڵ� ȸ�� �ֱٰ� ����ó��
			_currentAutoRotInput =
				FMath::FInterpTo(
					_currentAutoRotInput,
					_targetAutoRotInput, In_deltaTime, AutoRotIntpSpeed);

			GSLOG(Warning,
				TEXT("_targetAutoRotInput: %f, _currentAutoRotInput: %f"),
				_targetAutoRotInput,
				_currentAutoRotInput);
#ifdef NEW_CAM_CHAR
			if (In_char == nullptr)
			{
				GSLOG(Error, TEXT("In_char == nullptr"));
				return;
			}
			AGsLocalCharacter* localChar = In_char->GetLocalCharacter();
			if (localChar == nullptr)
			{
				GSLOG(Error, TEXT("localChar == nullptr"));
				return;
			}

			localChar->AddControllerYawInput(_currentAutoRotInput);
#else
			In_char->AddControllerYawInput(_currentAutoRotInput);
#endif
		}
	}
	// ������
	else
	{
		// �ڵ� ȸ�� �ֱٰ� �ʱ�ȭ
		_currentAutoRotInput = 0;
	}


}

// ��,�Ʒ� �̵� ó��
void GsCameraModeFollowBase::UpDown(float NewAxisValue, UGsGameObjectLocal* In_char)
{	
	GsCameraModeFreeBase::UpDown(NewAxisValue, In_char);

	// y�Է� ����
	_currentYInput = NewAxisValue;
}
// ��,�� �̵� ó��
void GsCameraModeFollowBase::LeftRight(float NewAxisValue, UGsGameObjectLocal* In_char)
{
	GsCameraModeFreeBase::LeftRight(NewAxisValue, In_char);
	// x�Է� ����
	_currentXInput = NewAxisValue;
}
// �� �̵� ó��
void GsCameraModeFollowBase::MoveForward(UGsGameObjectLocal* In_char)
{
	GsCameraModeFreeBase::MoveForward(In_char);
	
	// y�Է� ����
	_currentYInput = 1.0f;
}
// �� �̵� ó��
void GsCameraModeFollowBase::MoveBackward(UGsGameObjectLocal* In_char)
{
	GsCameraModeFreeBase::MoveBackward(In_char);

	// y�Է� ����
	_currentYInput = -1.0f;
}
// �� �̵� ó��
void GsCameraModeFollowBase::MoveLeft(UGsGameObjectLocal* In_char)
{
	GsCameraModeFreeBase::MoveLeft(In_char);

	// x�Է� ����
	_currentXInput = -1.0f;
}
// �� �̵� ó��
void GsCameraModeFollowBase::MoveRight(UGsGameObjectLocal* In_char)
{
	GsCameraModeFreeBase::MoveRight(In_char);

	// x�Է� ����
	_currentXInput = 1.0f;
}

// �̵� ����
void GsCameraModeFollowBase::MoveStop(UGsGameObjectLocal* In_char)
{
	GsCameraModeFreeBase::MoveStop(In_char);

	//  x, y ���ÿ� ���ͼ� ó�� ����� �ȵ�....
	// �����ؾ��ҵ�.....
	_currentXInput = 0.0f;
	_currentYInput = 0.0f;
}

// ���� ��ȯ(���ڷ� ĳ����)
void GsCameraModeFollowBase::Enter(UGsGameObjectLocal* In_char, GsCameraModeManager* In_mng)
{
	if (In_char == nullptr)
	{
		return;
	}
	GsCameraModeFreeBase::Enter(In_char, In_mng);

	AGsLocalCharacter* localChar = In_char->GetLocalCharacter();
	if (localChar == nullptr)
	{
		GSLOG(Error, TEXT("localChar == nullptr"));
		return;
	}

	UGsInputBindingLocalPlayer* inputBinding = localChar->GetInputBinder();

	if (inputBinding == nullptr)
	{
		GSLOG(Error, TEXT("inputBinding == nullptr"));
		return;
	}

	// ���ε� ó��
	inputBinding->FunctionTouchPress = [=]() {TouchPress(); };
	inputBinding->FunctionTouchRelease = [=]() {TouchRelease(); };

	// ��ġ ���ٰ� ħ...(���߿� ��ġ�߿� ������ �����ϰų�, �������� ������ ������ �װɷ� ��...)
	bTouchOn = false;

	GSLOG(Error, TEXT("Enter"));
}

// ���� ����(���ڷ� ĳ����)
void GsCameraModeFollowBase::Exit(UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}

	GsCameraModeFreeBase::Exit(In_char);

	AGsLocalCharacter* localChar = In_char->GetLocalCharacter();
	if (localChar == nullptr)
	{
		GSLOG(Error, TEXT("localChar == nullptr"));
		return;
	}

	UGsInputBindingLocalPlayer* inputBinding = localChar->GetInputBinder();

	if (inputBinding == nullptr)
	{
		GSLOG(Error, TEXT("inputBinding == nullptr"));
		return;
	}

	// ���ε� ����
	inputBinding->FunctionTouchPress = nullptr;
	inputBinding->FunctionTouchRelease = nullptr;
}

// ��ġ ����
void GsCameraModeFollowBase::TouchPress()
{
	bTouchOn = true;
	GSLOG(Error, TEXT("touch on"));
}
// ��ġ ��
void GsCameraModeFollowBase::TouchRelease()
{
	bTouchOn = false;
	GSLOG(Error, TEXT("touch off"));
}