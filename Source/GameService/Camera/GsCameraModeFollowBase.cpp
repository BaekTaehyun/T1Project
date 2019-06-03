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

// 업데이터(인자로 캐릭터)
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

	// 입력이 있다면
	// 터치는 안되어있어야함
	if ((_currentXInput != 0.0f || _currentYInput != 0) && (bTouchOn == false))
	{
		float inputDegrees = CalcDegreesKeyBoardInput(_currentXInput, _currentYInput);


#ifdef TEST_LOG
		GSLOG(Warning, 
			TEXT("now Degrees: %f"), 
			inputDegrees);
#endif

		float calcDegrees = 0.0f;
		// 조이스틱 기준 오른쪽 체크(제한 안에서)
		if (inputDegrees >= RightMin &&
			inputDegrees <= RightMax)
		{
			calcDegrees = inputDegrees;
		}

		// 조이스틱 기준 왼쪽 체크(제한 안에서)
		if (inputDegrees >= LeftMin &&
			inputDegrees <= LeftMax)
		{
			// 180도 이상은 360도를 빼서 음수로 만듬
			calcDegrees = inputDegrees - 360;
		}


#ifdef TEST_LOG
		GSLOG(Warning, TEXT("calcDegrees: %f"), calcDegrees);
#endif

		// 값이 0으로 나오면 처리할 필요 없음
		if (calcDegrees == 0.0f)
		{
			// 자동 회전 최근값 초기화
			_currentAutoRotInput = 0;
		}
		else
		{
			// 최대 100이므로 100 으로 나누어 최대 1로 만듬
			// 거기다 모드의 값
			_targetAutoRotInput = calcDegrees * 0.01f * AddRotVal;


			// 자동 회전 최근값 보간처리
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
	// 없으면
	else
	{
		// 자동 회전 최근값 초기화
		_currentAutoRotInput = 0;
	}


}

// 위,아래 이동 처리
void GsCameraModeFollowBase::UpDown(float NewAxisValue, UGsGameObjectLocal* In_char)
{	
	GsCameraModeFreeBase::UpDown(NewAxisValue, In_char);

	// y입력 저장
	_currentYInput = NewAxisValue;
}
// 좌,우 이동 처리
void GsCameraModeFollowBase::LeftRight(float NewAxisValue, UGsGameObjectLocal* In_char)
{
	GsCameraModeFreeBase::LeftRight(NewAxisValue, In_char);
	// x입력 저장
	_currentXInput = NewAxisValue;
}
// 앞 이동 처리
void GsCameraModeFollowBase::MoveForward(UGsGameObjectLocal* In_char)
{
	GsCameraModeFreeBase::MoveForward(In_char);
	
	// y입력 저장
	_currentYInput = 1.0f;
}
// 뒤 이동 처리
void GsCameraModeFollowBase::MoveBackward(UGsGameObjectLocal* In_char)
{
	GsCameraModeFreeBase::MoveBackward(In_char);

	// y입력 저장
	_currentYInput = -1.0f;
}
// 좌 이동 처리
void GsCameraModeFollowBase::MoveLeft(UGsGameObjectLocal* In_char)
{
	GsCameraModeFreeBase::MoveLeft(In_char);

	// x입력 저장
	_currentXInput = -1.0f;
}
// 우 이동 처리
void GsCameraModeFollowBase::MoveRight(UGsGameObjectLocal* In_char)
{
	GsCameraModeFreeBase::MoveRight(In_char);

	// x입력 저장
	_currentXInput = 1.0f;
}

// 이동 정지
void GsCameraModeFollowBase::MoveStop(UGsGameObjectLocal* In_char)
{
	GsCameraModeFreeBase::MoveStop(In_char);

	//  x, y 동시에 들어와서 처리 현재는 안됨....
	// 수정해야할듯.....
	_currentXInput = 0.0f;
	_currentYInput = 0.0f;
}

// 상태 전환(인자로 캐릭터)
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

	// 바인딩 처리
	inputBinding->FunctionTouchPress = [=]() {TouchPress(); };
	inputBinding->FunctionTouchRelease = [=]() {TouchRelease(); };

	// 터치 땟다고 침...(나중에 터치중에 변경이 가능하거나, 전역적인 데이터 있으면 그걸로 씀...)
	bTouchOn = false;

	GSLOG(Error, TEXT("Enter"));
}

// 상태 종료(인자로 캐릭터)
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

	// 바인딩 해제
	inputBinding->FunctionTouchPress = nullptr;
	inputBinding->FunctionTouchRelease = nullptr;
}

// 터치 시작
void GsCameraModeFollowBase::TouchPress()
{
	bTouchOn = true;
	GSLOG(Error, TEXT("touch on"));
}
// 터치 끝
void GsCameraModeFollowBase::TouchRelease()
{
	bTouchOn = false;
	GSLOG(Error, TEXT("touch off"));
}