#include "GsCameraModeBase.h"
#include "../../T1Project/T1Player.h"
#include "GsCameraModeManager.h"
#include "GsCamModeData.h"
#include "../UTIL/GsText.h"

// ���� ��ȯ(���ڷ� ĳ����)
void GsCameraModeBase::Enter(ACharacter* In_char, GsCameraModeManager* In_mng)
{
	AT1Player* player = Cast<AT1Player>(In_char);
	if (player == nullptr)
	{
		GSLOG(Error, TEXT("Cast<AGsCharacter> player == nullptr"));
		return;
	}

	// ���ε� 
	player->FunctionZoomIn = [&]() {ZoomIn(); };
	player->FunctionZoomOut = [&]() {ZoomOut(); };


	EGsControlMode nowMode = GetType();
	FGsCamModeData* camModeData = In_mng->GetCamModeData(nowMode);

	if (camModeData == nullptr)
	{
		GSLOG(Error, TEXT("now mode: %s data is null"), 
			*EnumToString(EGsControlMode, nowMode));
		return;
	}
	// �� ����
	_armLengthTo = camModeData->ArmLengthTo;


	if (player->Controller && 
		player->Controller->IsLocalPlayerController())
	{
		APlayerController* const PC = CastChecked<APlayerController>(player->Controller);
		if (PC->IsLookInputIgnored() == false)
		{
			FRotator ViewRotation = PC->GetControlRotation();

			// ��ǲ���� ���簪�� ���ϹǷ�
			// ��ǥ������ ���簪�� ������ ����
			float newInput = camModeData->ControllerPitch - ViewRotation.Pitch;
			PC->RotationInput.Pitch = newInput;
		}
	}


	GSLOG(Warning, TEXT("GsCameraModeBase ACharacter Enter nowMode: %s"), 
		*EnumToString(EGsControlMode, nowMode));
}
// ���� ����(���ڷ� ĳ����)
void GsCameraModeBase::Exit(ACharacter* In_char)
{
	AT1Player* player = Cast<AT1Player>(In_char);
	if (player == nullptr)
	{
		GSLOG(Error, TEXT("Cast<AGsCharacter> player == nullptr"));
		return;
	}

	// ���ε� ����
	player->FunctionZoomIn = nullptr;
	player->FunctionZoomOut = nullptr;

	GSLOG(Warning, TEXT("GsCameraModeBase ACharacter Exit"));
}
// ��������(���ڷ� ĳ����)
void GsCameraModeBase::Update(ACharacter* In_char, float In_deltaTime)
{
	AT1Player* player = Cast<AT1Player>(In_char);
	if (player == nullptr)
	{
		GSLOG(Error, TEXT("Cast<AGsCharacter> player == nullptr"));
		return;
	}

	player->SpringArm->TargetArmLength = FMath::FInterpTo(
		player->SpringArm->TargetArmLength, 
		_armLengthTo, In_deltaTime, ArmLengthSpeed);

#ifdef PRINT_LOG
	GSLOG(Warning, TEXT("GsCameraModeBase ACharacter Update"));
#endif
}
// ����
void GsCameraModeBase::ZoomIn()
{
	_armLengthTo += ZoomInOutPower;
	_armLengthTo = FMath::Clamp(_armLengthTo, ArmLengthMin, ArmLengthMax);
}
// �ܾƿ�
void GsCameraModeBase::ZoomOut()
{
	_armLengthTo -= ZoomInOutPower;
	_armLengthTo = FMath::Clamp(_armLengthTo, ArmLengthMin, ArmLengthMax);
}