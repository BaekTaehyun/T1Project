#include "GsCameraModeFix.h"
#include "GameService.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"

#include "GsCameraModeManager.h"

#include "../GameObject/ObjectClass/GsGameObjectLocal.h"
#include "../GameObject/ActorExtend/GsLocalCharacter.h"
#include "../GameObject/Input/GsInputBindingLocalPlayer.h"
#include "../GameObject/Movement/GsMovementBase.h"

#include "EngineMinimal.h"

GsCameraModeFix::GsCameraModeFix() : GsCameraModeBase(EGsControlMode::Fixed)
{
	GSLOG(Warning, TEXT("GsCameraModeFix constructor"));
}
GsCameraModeFix::~GsCameraModeFix()
{
	GSLOG(Warning, TEXT("GsCameraModeFix destructor"));
}

void GsCameraModeFix::Enter()
{
	GSLOG(Warning, TEXT("GsCameraModeFix Enter"));
}

void GsCameraModeFix::Exit()
{
	GSLOG(Warning, TEXT("GsCameraModeFix Exit"));

}

void GsCameraModeFix::Update()
{
	GSLOG(Warning, TEXT("GsCameraModeFix Update"));
}

// 상태 전환(인자로 캐릭터)
void GsCameraModeFix::Enter(UGsGameObjectLocal* In_char, GsCameraModeManager* In_mng)
{
	if (In_char == nullptr)
	{
		GSLOG(Error, TEXT("In_char == nullptr"));
		return;
	}
	GsCameraModeBase::Enter(In_char, In_mng);


	AGsLocalCharacter* localChar = In_char->GetLocalCharacter();
	if (localChar == nullptr)
	{
		GSLOG(Error, TEXT("localChar == nullptr"));
		return;
	}

	UGsInputBindingLocalPlayer* inputBinding = Cast<UGsInputBindingLocalPlayer>(localChar->GetInputBinder());

	if (inputBinding == nullptr)
	{
		GSLOG(Error, TEXT("inputBinding == nullptr"));
		return;
	}

	// 바인딩 처리
	// 람다 캡쳐를 레퍼런스(&)로 하고 포인터 받으면 다른주소를 넘겨줘서
	// 실제 호출시 이상 상황 발생함....
	// 포인터는 복사(=)로 캡쳐해야함....
	inputBinding->FunctionMoveForward = [=](float val) {MoveForward(val, In_char); };
	inputBinding->FunctionMoveBackward = [=](float val) {MoveBackward(val, In_char); };
	inputBinding->FunctionMoveLeft = [=](float val) {MoveLeft(val, In_char); };
	inputBinding->FunctionMoveRight = [=](float val) {MoveRight(val, In_char); };

	inputBinding->FunctionLookUp = [=](float val) {LookUp(val, In_char); };
	inputBinding->FunctionTurn = [=](float val) {Turn(val, In_char); };

	_armRotationTo = FRotator(-45.0f, 0.0f, 0.0f);

	localChar->GetSpringArm()->bUsePawnControlRotation = false;
	localChar->GetSpringArm()->bInheritPitch = false;
	localChar->GetSpringArm()->bInheritRoll = false;
	localChar->GetSpringArm()->bInheritYaw = false;
	localChar->GetSpringArm()->bDoCollisionTest = false;
	localChar->bUseControllerRotationYaw = false;

	localChar->GetCharacterMovement()->bOrientRotationToMovement = false;
	localChar->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	localChar->GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);


	GSLOG(Warning, TEXT("GsCameraModeFix ACharacter Enter"));
}
// 상태 종료(인자로 캐릭터)
void GsCameraModeFix::Exit(UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		GSLOG(Error, TEXT("In_char == nullptr"));
		return;
	}
	GsCameraModeBase::Exit(In_char);

	AGsLocalCharacter* localChar = In_char->GetLocalCharacter();
	if (localChar == nullptr)
	{
		GSLOG(Error, TEXT("localChar == nullptr"));
		return;
	}

	UGsInputBindingLocalPlayer* inputBinding = Cast<UGsInputBindingLocalPlayer>(localChar->GetInputBinder());

	if (inputBinding == nullptr)
	{
		GSLOG(Error, TEXT("inputBinding == nullptr"));
		return;
	}
	// 바인딩 해제
	inputBinding->FunctionMoveForward = nullptr;
	inputBinding->FunctionMoveBackward = nullptr;
	inputBinding->FunctionMoveLeft = nullptr;
	inputBinding->FunctionMoveRight = nullptr;
	inputBinding->FunctionLookUp = nullptr;
	inputBinding->FunctionTurn = nullptr;


	GSLOG(Warning, TEXT("GsCameraModeFix ACharacter exit"));
}
// 업데이터(인자로 캐릭터)
void GsCameraModeFix::Update(UGsGameObjectLocal* In_char, float In_deltaTime, GsCameraModeManager* In_mng)
{
	if (In_char == nullptr)
	{
		return;
	}

	GsCameraModeBase::Update(In_char, In_deltaTime, In_mng);

	AGsLocalCharacter* localChar = In_char->GetLocalCharacter();
	if (localChar == nullptr)
	{
		GSLOG(Error, TEXT("localChar == nullptr"));
		return;
	}

	localChar->GetSpringArm()->RelativeRotation = FMath::RInterpTo(
		localChar->GetSpringArm()->RelativeRotation, _armRotationTo,
		In_deltaTime, ArmRotationSpeed);

	if (_directionToMove.SizeSquared() > 0.0f)
	{
		localChar->GetController()->SetControlRotation(
			FRotationMatrix::MakeFromX(_directionToMove).Rotator());


		if (auto movement = In_char->GetMovement())
		{
			movement->Move(_directionToMove, 10.0f);
		}
	}

}

// 위,아래 이동 처리
void GsCameraModeFix::UpDown(float NewAxisValue, UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}

	_directionToMove.X = NewAxisValue;
}
// 좌,우 이동 처리
void GsCameraModeFix::LeftRight(float NewAxisValue, UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}

	_directionToMove.Y = NewAxisValue;
}

// 앞 이동 처리
void GsCameraModeFix::MoveForward(float NewAxisValue, UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}
	_directionToMove.X = 10.0f;
}
// 뒤 이동 처리
void GsCameraModeFix::MoveBackward(float NewAxisValue, UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}
	_directionToMove.X = -5.0f;
}
// 좌 이동 처리
void GsCameraModeFix::MoveLeft(float NewAxisValue, UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}
	_directionToMove.Y = -5.0f;
}
// 우 이동 처리
void GsCameraModeFix::MoveRight(float NewAxisValue, UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}
	_directionToMove.Y = 5.0f;
}


// 위, 아래 카메라 회전
void GsCameraModeFix::LookUp(float NewAxisValue, UGsGameObjectLocal* In_char)
{
}
// 좌, 우 카메라 회전
void GsCameraModeFix::Turn(float NewAxisValue, UGsGameObjectLocal* In_char)
{	
}

// 다음 스텝 진행
void GsCameraModeFix::NextStep(GsCameraModeManager* In_mng)
{
	if (In_mng == nullptr)
	{
		GSLOG(Error, TEXT("GsCameraModeManager* In_mng == nullptr"));
		return;
	}

	// 다음은 free다
	In_mng->ChangeState(EGsControlMode::Free);
}
// 이동 정지
void GsCameraModeFix::MoveStop(UGsGameObjectLocal* In_char)
{
	GsCameraModeBase::MoveStop(In_char);

	_directionToMove.X = 0.0f;
	_directionToMove.Y = 0.0f;

}