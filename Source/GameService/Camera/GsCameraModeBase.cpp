#include "GsCameraModeBase.h"
#include "../../T1Project/T1Player.h"
#include "GsCameraModeManager.h"
#include "GsCamModeData.h"
#include "../UTIL/GsText.h"

// 상태 전환(인자로 캐릭터)
void GsCameraModeBase::Enter(ACharacter* In_char, GsCameraModeManager* In_mng)
{
	AT1Player* player = Cast<AT1Player>(In_char);
	if (player == nullptr)
	{
		GSLOG(Error, TEXT("Cast<AGsCharacter> player == nullptr"));
		return;
	}

	// 바인딩 
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
	// 값 세팅
	_armLengthTo = camModeData->ArmLengthTo;


	if (player->Controller && 
		player->Controller->IsLocalPlayerController())
	{
		APlayerController* const PC = CastChecked<APlayerController>(player->Controller);
		if (PC->IsLookInputIgnored() == false)
		{
			FRotator ViewRotation = PC->GetControlRotation();

			// 인풋값을 현재값에 더하므로
			// 목표값에서 현재값을 뺀값을 넣음
			float newInput = camModeData->ControllerPitch - ViewRotation.Pitch;
			PC->RotationInput.Pitch = newInput;
		}
	}


	GSLOG(Warning, TEXT("GsCameraModeBase ACharacter Enter nowMode: %s"), 
		*EnumToString(EGsControlMode, nowMode));
}
// 상태 종료(인자로 캐릭터)
void GsCameraModeBase::Exit(ACharacter* In_char)
{
	AT1Player* player = Cast<AT1Player>(In_char);
	if (player == nullptr)
	{
		GSLOG(Error, TEXT("Cast<AGsCharacter> player == nullptr"));
		return;
	}

	// 바인딩 해제
	player->FunctionZoomIn = nullptr;
	player->FunctionZoomOut = nullptr;

	GSLOG(Warning, TEXT("GsCameraModeBase ACharacter Exit"));
}
// 업데이터(인자로 캐릭터)
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
// 줌인
void GsCameraModeBase::ZoomIn()
{
	_armLengthTo += ZoomInOutPower;
	_armLengthTo = FMath::Clamp(_armLengthTo, ArmLengthMin, ArmLengthMax);
}
// 줌아웃
void GsCameraModeBase::ZoomOut()
{
	_armLengthTo -= ZoomInOutPower;
	_armLengthTo = FMath::Clamp(_armLengthTo, ArmLengthMin, ArmLengthMax);
}