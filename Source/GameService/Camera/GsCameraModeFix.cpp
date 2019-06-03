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

// ���� ��ȯ(���ڷ� ĳ����)
void GsCameraModeFix::Enter(UGsGameObjectLocal* In_char, GsCameraModeManager* In_mng)
{
	if (In_char == nullptr)
	{
		GSLOG(Error, TEXT("In_char == nullptr"));
		return;
	}
	GsCameraModeBase::Enter(In_char, In_mng);
#ifndef NEW_CAM_CHAR	
	AT1Player* player = Cast<AT1Player>(In_char);
	if (player == nullptr)
	{
		GSLOG(Error, TEXT("Cast<AGsCharacter> player == nullptr"));
		return;
	}


	// ���ε� ó��
	// ���� ĸ�ĸ� ���۷���(&)�� �ϰ� ������ ������ �ٸ��ּҸ� �Ѱ��༭
	// ���� ȣ��� �̻� ��Ȳ �߻���....
	// �����ʹ� ����(=)�� ĸ���ؾ���....
	player->FunctionUpDown = [=](float val) {UpDown(val, In_char); };
	player->FunctionLeftRight = [=](float val) {LeftRight(val, In_char); };
	player->FunctionLookUp = [=](float val) {LookUp(val, In_char); };
	player->FunctionTurn = [=](float val) {Turn(val, In_char); };

	_armRotationTo = FRotator(-45.0f, 0.0f, 0.0f);
	// �� ����
	player->SpringArm->bUsePawnControlRotation = false;
	player->SpringArm->bInheritPitch = false;
	player->SpringArm->bInheritRoll = false;
	player->SpringArm->bInheritYaw = false;
	player->SpringArm->bDoCollisionTest = false;
	player->bUseControllerRotationYaw = false;

	player->GetCharacterMovement()->bOrientRotationToMovement = false;
	player->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	player->GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
#else

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
	// ���� ĸ�ĸ� ���۷���(&)�� �ϰ� ������ ������ �ٸ��ּҸ� �Ѱ��༭
	// ���� ȣ��� �̻� ��Ȳ �߻���....
	// �����ʹ� ����(=)�� ĸ���ؾ���....
	inputBinding->FunctionMoveForward = [=]() {MoveForward(In_char); };
	inputBinding->FunctionMoveBackward = [=]() {MoveBackward(In_char); };
	inputBinding->FunctionMoveLeft = [=]() {MoveLeft(In_char); };
	inputBinding->FunctionMoveRight = [=]() {MoveRight(In_char); };

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
#endif

	GSLOG(Warning, TEXT("GsCameraModeFix ACharacter Enter"));
}
// ���� ����(���ڷ� ĳ����)
void GsCameraModeFix::Exit(UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		GSLOG(Error, TEXT("In_char == nullptr"));
		return;
	}
	GsCameraModeBase::Exit(In_char);

#ifdef NEW_CAM_CHAR
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
	inputBinding->FunctionMoveForward = nullptr;
	inputBinding->FunctionMoveBackward = nullptr;
	inputBinding->FunctionMoveLeft = nullptr;
	inputBinding->FunctionMoveRight = nullptr;
	inputBinding->FunctionLookUp = nullptr;
	inputBinding->FunctionTurn = nullptr;

#else
	GsCameraModeBase::Exit(In_char);
	AT1Player* player = Cast<AT1Player>(In_char);
	if (player == nullptr)
	{
		GSLOG(Error, TEXT("Cast<AGsCharacter> player == nullptr"));
		return;
	}

	// ���ε� ����
	player->FunctionUpDown = nullptr;
	player->FunctionLeftRight = nullptr;
	player->FunctionLookUp = nullptr;
	player->FunctionTurn = nullptr;
#endif
	GSLOG(Warning, TEXT("GsCameraModeFix ACharacter exit"));
}
// ��������(���ڷ� ĳ����)
void GsCameraModeFix::Update(UGsGameObjectLocal* In_char, float In_deltaTime, GsCameraModeManager* In_mng)
{
	if (In_char == nullptr)
	{
		return;
	}

	GsCameraModeBase::Update(In_char, In_deltaTime, In_mng);

#ifdef NEW_CAM_CHAR
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
			movement->Move(_directionToMove, EGsGameObjectMoveDirType::Forward, 10.0f);
		}
	}
#else
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

// ��,�Ʒ� �̵� ó��
void GsCameraModeFix::UpDown(float NewAxisValue, UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}

	_directionToMove.X = NewAxisValue;
}
// ��,�� �̵� ó��
void GsCameraModeFix::LeftRight(float NewAxisValue, UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}

	_directionToMove.Y = NewAxisValue;
}

// �� �̵� ó��
void GsCameraModeFix::MoveForward(UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}
	_directionToMove.X = 10.0f;
}
// �� �̵� ó��
void GsCameraModeFix::MoveBackward(UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}
	_directionToMove.X = -5.0f;
}
// �� �̵� ó��
void GsCameraModeFix::MoveLeft(UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}
	_directionToMove.Y = -5.0f;
}
// �� �̵� ó��
void GsCameraModeFix::MoveRight(UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}
	_directionToMove.Y = 5.0f;
}


// ��, �Ʒ� ī�޶� ȸ��
void GsCameraModeFix::LookUp(float NewAxisValue, UGsGameObjectLocal* In_char)
{
}
// ��, �� ī�޶� ȸ��
void GsCameraModeFix::Turn(float NewAxisValue, UGsGameObjectLocal* In_char)
{	
}

// ���� ���� ����
void GsCameraModeFix::NextStep(GsCameraModeManager* In_mng)
{
	if (In_mng == nullptr)
	{
		GSLOG(Error, TEXT("GsCameraModeManager* In_mng == nullptr"));
		return;
	}

	// ������ free��
	In_mng->ChangeState(EGsControlMode::Free);
}
