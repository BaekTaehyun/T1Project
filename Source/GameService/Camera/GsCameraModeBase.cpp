#include "GsCameraModeBase.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "GsCameraModeManager.h"
#include "GsCamModeData.h"
#include "../UTIL/GsText.h"

#include "../GameObject/ObjectClass/GsGameObjectLocal.h"
#include "../GameObject/ActorExtend/GsLocalCharacter.h"
#include "../GameObject/Input/GsInputBindingLocalPlayer.h"
#include "../GameObject/Movement/GsMovementBase.h"
#include "EngineMinimal.h"

// ���� ��ȯ(���ڷ� ĳ����)
void GsCameraModeBase::Enter(UGsGameObjectLocal* In_char, GsCameraModeManager* In_mng)
{

	if (In_char == nullptr)
	{
		GSLOG(Error, TEXT("In_char == nullptr"));
		return;
	}
	AGsLocalCharacter* localChar =	In_char->GetLocalCharacter();
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

	// ���ε�
	inputBinding->FunctionZoomIn = [=]() {ZoomIn(); };
	inputBinding->FunctionZoomOut = [=]() {ZoomOut(); };
	inputBinding->FunctionMoveStop = [=]() {MoveStop(In_char); };

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

	if (localChar->Controller &&
		localChar->Controller->IsLocalPlayerController())
	{
		APlayerController* const PC = 
			CastChecked<APlayerController>(localChar->Controller);
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
void GsCameraModeBase::Exit(UGsGameObjectLocal* In_char)
{

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

	UGsInputBindingLocalPlayer* inputBinding = Cast<UGsInputBindingLocalPlayer>(localChar->GetInputBinder());

	if (inputBinding == nullptr)
	{
		GSLOG(Error, TEXT("inputBinding == nullptr"));
		return;
	}

	inputBinding->FunctionZoomIn = nullptr;
	inputBinding->FunctionZoomOut = nullptr;
	inputBinding->FunctionMoveStop = nullptr;


	GSLOG(Warning, TEXT("GsCameraModeBase ACharacter Exit"));
}
// ��������(���ڷ� ĳ����)
void GsCameraModeBase::Update(UGsGameObjectLocal* In_char, float In_deltaTime, GsCameraModeManager* In_mng)
{

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

	localChar->GetSpringArm()->TargetArmLength = 
		FMath::FInterpTo(
			localChar->GetSpringArm()->TargetArmLength,
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

// �̵� ����
void GsCameraModeBase::MoveStop(UGsGameObjectLocal* In_char)
{
	GSLOG(Warning, TEXT("MoveStop"));

	if (In_char == nullptr)
	{
		return;
	}

	if (auto movement = In_char->GetMovement())
	{
		movement->Stop();
	}
}