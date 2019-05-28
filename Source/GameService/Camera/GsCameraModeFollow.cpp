#include "GsCameraModeFollow.h"
#include "GameService.h"
#include "../../T1Project/T1Player.h"
#include "GsCameraModeManager.h"

GsCameraModeFollow::GsCameraModeFollow() 
{
	GSLOG(Warning, TEXT("GsCameraModeFollow constructor"));
}
GsCameraModeFollow::~GsCameraModeFollow()
{
	GSLOG(Warning, TEXT("GsCameraModeFollow destructor"));
}

void GsCameraModeFollow::Enter()
{
	GSLOG(Warning, TEXT("GsCameraModeFollow Enter"));
}

void GsCameraModeFollow::Exit()
{
	GSLOG(Warning, TEXT("GsCameraModeFollow Exit"));

}

void GsCameraModeFollow::Update()
{	
	GSLOG(Warning, TEXT("GsCameraModeFollow Update"));
}
// 상태 전환(인자로 캐릭터)
void GsCameraModeFollow::Enter(ACharacter* In_char, GsCameraModeManager* In_mng)
{
	if (In_char == nullptr)
	{
		return;
	}

	GsCameraModeBase::Enter(In_char, In_mng);
	AT1Player* player = Cast<AT1Player>(In_char);
	if (player == nullptr)
	{
		GSLOG(Error, TEXT("Cast<AGsCharacter> player == nullptr"));
		return;
	}

	if (In_mng == nullptr)
	{
		GSLOG(Error, TEXT("GsCameraModeManager* In_mng"));
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

	player->SpringArm->bUsePawnControlRotation = true;
	player->SpringArm->bInheritPitch = true;
	player->SpringArm->bInheritRoll = true;
	player->SpringArm->bInheritYaw = true;
	player->SpringArm->bDoCollisionTest = true;
	player->bUseControllerRotationYaw = false;

	player->GetCharacterMovement()->bOrientRotationToMovement = true;
	player->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	player->GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	
	GSLOG(Warning, TEXT("GsCameraModeFollow ACharacter Enter"));
}
// 상태 종료(인자로 캐릭터)
void GsCameraModeFollow::Exit(ACharacter* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}

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

	GSLOG(Warning, TEXT("GsCameraModeFollow ACharacter exit"));
}
// 업데이터(인자로 캐릭터)
void GsCameraModeFollow::Update(ACharacter* In_char, float In_deltaTime)
{
	GsCameraModeBase::Update(In_char, In_deltaTime);
}

// 위,아래 이동 처리
void GsCameraModeFollow::UpDown(float NewAxisValue, ACharacter* In_char)
{	
	if (In_char == nullptr || 
		In_char->Controller == nullptr)
	{
		return;
	}

	In_char->AddMovementInput(
		FRotationMatrix(
			In_char->GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);
}
// 좌,우 이동 처리
void GsCameraModeFollow::LeftRight(float NewAxisValue, ACharacter* In_char)
{
	if (In_char == nullptr ||
		In_char->Controller == nullptr)
	{
		return;
	}

	In_char->AddMovementInput(
		FRotationMatrix(
			In_char->GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);
}
// 위, 아래 카메라 회전
void GsCameraModeFollow::LookUp(float NewAxisValue, ACharacter* In_char)
{
	if (In_char == nullptr ||
		In_char->Controller == nullptr)
	{
		return;
	}

	In_char->AddControllerPitchInput(NewAxisValue);
}
// 좌, 우 카메라 회전
void GsCameraModeFollow::Turn(float NewAxisValue, ACharacter* In_char)
{
	if (In_char == nullptr ||
		In_char->Controller == nullptr)
	{
		return;
	}

	In_char->AddControllerYawInput(NewAxisValue);
}
