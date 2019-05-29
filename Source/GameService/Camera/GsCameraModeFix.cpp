#include "GsCameraModeFix.h"
#include "GameService.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
//#include "../../T1Project/T1Player.h"
#include "GsCameraModeManager.h"

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
void GsCameraModeFix::Enter(ACharacter* In_char, GsCameraModeManager* In_mng)
{
	if (In_char == nullptr)
	{
		return;
	}
#ifdef CAM_MODE
	GsCameraModeBase::Enter(In_char, In_mng);
	AT1Player* player = Cast<AT1Player>(In_char);
	if (player == nullptr)
	{
		GSLOG(Error, TEXT("Cast<AGsCharacter> player == nullptr"));
		return;
	}


	// 바인딩 처리
	// 람다 캡쳐를 레퍼런스(&)로 하고 포인터 받으면 다른주소를 넘겨줘서
	// 실제 호출시 이상 상황 발생함....
	// 포인터는 복사(=)로 캡쳐해야함....
	player->FunctionUpDown = [=](float val) {UpDown(val, In_char); };
	player->FunctionLeftRight = [=](float val) {LeftRight(val, In_char); };
	player->FunctionLookUp = [=](float val) {LookUp(val, In_char); };
	player->FunctionTurn = [=](float val) {Turn(val, In_char); };

	_armRotationTo = FRotator(-45.0f, 0.0f, 0.0f);
	// 값 세팅
	player->SpringArm->bUsePawnControlRotation = false;
	player->SpringArm->bInheritPitch = false;
	player->SpringArm->bInheritRoll = false;
	player->SpringArm->bInheritYaw = false;
	player->SpringArm->bDoCollisionTest = false;
	player->bUseControllerRotationYaw = false;

	player->GetCharacterMovement()->bOrientRotationToMovement = false;
	player->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	player->GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
#endif

	GSLOG(Warning, TEXT("GsCameraModeFix ACharacter Enter"));
}
// 상태 종료(인자로 캐릭터)
void GsCameraModeFix::Exit(ACharacter* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}
#ifdef CAM_MODE
	GsCameraModeBase::Exit(In_char);
	AT1Player* player = Cast<AT1Player>(In_char);
	if (player == nullptr)
	{
		GSLOG(Error, TEXT("Cast<AGsCharacter> player == nullptr"));
		return;
	}

	// 바인딩 해제
	player->FunctionUpDown = nullptr;
	player->FunctionLeftRight = nullptr;
	player->FunctionLookUp = nullptr;
	player->FunctionTurn = nullptr;
#endif
	GSLOG(Warning, TEXT("GsCameraModeFix ACharacter exit"));
}
// 업데이터(인자로 캐릭터)
void GsCameraModeFix::Update(ACharacter* In_char, float In_deltaTime)
{
	if (In_char == nullptr)
	{
		return;
	}

	GsCameraModeBase::Update(In_char, In_deltaTime);

#ifdef CAM_MODE
	AT1Player* player = Cast<AT1Player>(In_char);
	if (player == nullptr)
	{
		GSLOG(Error, TEXT("Cast<AGsCharacter> player == nullptr"));
		return;
	}

	player->SpringArm->RelativeRotation = FMath::RInterpTo(
		player->SpringArm->RelativeRotation, _armRotationTo,
		In_deltaTime, ArmRotationSpeed);

	if (_directionToMove.SizeSquared() > 0.0f)
	{
		player->GetController()->SetControlRotation(
			FRotationMatrix::MakeFromX(_directionToMove).Rotator());

		player->AddMovementInput(_directionToMove);
	}
#endif
}

// 위,아래 이동 처리
void GsCameraModeFix::UpDown(float NewAxisValue, ACharacter* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}

	_directionToMove.X = NewAxisValue;
}
// 좌,우 이동 처리
void GsCameraModeFix::LeftRight(float NewAxisValue, ACharacter* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}

	_directionToMove.Y = NewAxisValue;
}
// 위, 아래 카메라 회전
void GsCameraModeFix::LookUp(float NewAxisValue, ACharacter* In_char)
{
}
// 좌, 우 카메라 회전
void GsCameraModeFix::Turn(float NewAxisValue, ACharacter* In_char)
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
