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
// ���� ��ȯ(���ڷ� ĳ����)
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

	// ���ε� ó��
	// ���� ĸ�ĸ� ���۷���(&)�� �ϰ� ������ ������ �ٸ��ּҸ� �Ѱ��༭
	// ���� ȣ��� �̻� ��Ȳ �߻���....
	// �����ʹ� ����(=)�� ĸ���ؾ���....
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
// ���� ����(���ڷ� ĳ����)
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

	// ���ε� ����
	player->FunctionUpDown = nullptr;
	player->FunctionLeftRight = nullptr;
	player->FunctionLookUp = nullptr;
	player->FunctionTurn = nullptr;

	GSLOG(Warning, TEXT("GsCameraModeFollow ACharacter exit"));
}
// ��������(���ڷ� ĳ����)
void GsCameraModeFollow::Update(ACharacter* In_char, float In_deltaTime)
{
	GsCameraModeBase::Update(In_char, In_deltaTime);
}

// ��,�Ʒ� �̵� ó��
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
// ��,�� �̵� ó��
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
// ��, �Ʒ� ī�޶� ȸ��
void GsCameraModeFollow::LookUp(float NewAxisValue, ACharacter* In_char)
{
	if (In_char == nullptr ||
		In_char->Controller == nullptr)
	{
		return;
	}

	In_char->AddControllerPitchInput(NewAxisValue);
}
// ��, �� ī�޶� ȸ��
void GsCameraModeFollow::Turn(float NewAxisValue, ACharacter* In_char)
{
	if (In_char == nullptr ||
		In_char->Controller == nullptr)
	{
		return;
	}

	In_char->AddControllerYawInput(NewAxisValue);
}
