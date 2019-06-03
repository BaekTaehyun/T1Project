#include "GsCameraModeFreeBase.h"
#include "GameService.h"
#include "GsCameraModeManager.h"
#include "EngineMinimal.h"

#include "../GameObject/ObjectClass/GsGameObjectLocal.h"
#include "../GameObject/ActorExtend/GsLocalCharacter.h"
#include "../GameObject/Input/GsInputBindingLocalPlayer.h"
#include "../GameObject/Movement/GsMovementBase.h"

GsCameraModeFreeBase::GsCameraModeFreeBase()
{
	GSLOG(Warning, TEXT("GsCameraModeFreeBase constructor"));
}
GsCameraModeFreeBase::~GsCameraModeFreeBase()
{
	GSLOG(Warning, TEXT("GsCameraModeFreeBase destructor"));
}

void GsCameraModeFreeBase::Enter()
{
	GSLOG(Warning, TEXT("GsCameraModeFreeBase Enter"));
}

void GsCameraModeFreeBase::Exit()
{
	GSLOG(Warning, TEXT("GsCameraModeFreeBase Exit"));

}

void GsCameraModeFreeBase::Update()
{
	GSLOG(Warning, TEXT("GsCameraModeFreeBase Update"));
}
// 상태 전환(인자로 캐릭터)
void GsCameraModeFreeBase::Enter(UGsGameObjectLocal* In_char, GsCameraModeManager* In_mng)
{
	if (In_char == nullptr)
	{
		return;
	}

	GsCameraModeBase::Enter(In_char, In_mng);
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

	// 바인딩 처리
	// 람다 캡쳐를 레퍼런스(&)로 하고 포인터 받으면 다른주소를 넘겨줘서
	// 실제 호출시 이상 상황 발생함....
	// 포인터는 복사(=)로 캡쳐해야함....
	inputBinding->FunctionMoveForward = [=]() {MoveForward(In_char); };
	inputBinding->FunctionMoveBackward = [=]() {MoveBackward(In_char); };
	inputBinding->FunctionMoveLeft = [=]() {MoveLeft(In_char); };
	inputBinding->FunctionMoveRight = [=]() {MoveRight(In_char); };

	inputBinding->FunctionLookUp = [=](float val) {LookUp(val, In_char); };
	inputBinding->FunctionTurn = [=](float val) {Turn(val, In_char); };

	localChar->GetSpringArm()->bUsePawnControlRotation = true;
	localChar->GetSpringArm()->bInheritPitch = true;
	localChar->GetSpringArm()->bInheritRoll = true;
	localChar->GetSpringArm()->bInheritYaw = true;
	localChar->GetSpringArm()->bDoCollisionTest = true;
	localChar->bUseControllerRotationYaw = false;

	localChar->GetCharacterMovement()->bOrientRotationToMovement = true;
	localChar->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	localChar->GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);


#else
	AGsCharacter* player = Cast<AGsCharacter>(In_char);
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

#endif

	GSLOG(Warning, TEXT("GsCameraModeFreeBase  Enter"));
}
// 상태 종료(인자로 캐릭터)
void GsCameraModeFreeBase::Exit(UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
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

	// 바인딩 해제
	inputBinding->FunctionMoveForward = nullptr;
	inputBinding->FunctionMoveBackward = nullptr;
	inputBinding->FunctionMoveLeft = nullptr;
	inputBinding->FunctionMoveRight = nullptr;
	inputBinding->FunctionLookUp = nullptr;
	inputBinding->FunctionTurn = nullptr;
#else
	AGsCharacter* player = Cast<AGsCharacter>(In_char);
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

	GSLOG(Warning, TEXT("GsCameraModeFreeBase  exit"));
}
// 업데이터(인자로 캐릭터)
void GsCameraModeFreeBase::Update(UGsGameObjectLocal* In_char, float In_deltaTime, GsCameraModeManager* In_mng)
{
	GsCameraModeBase::Update(In_char, In_deltaTime, In_mng);

}

// 위,아래 이동 처리
void GsCameraModeFreeBase::UpDown(float NewAxisValue, UGsGameObjectLocal* In_char)
{
#ifdef NEW_CAM_CHAR

#else
	if (In_char == nullptr ||
		In_char->Controller == nullptr)
	{
		return;
	}

	// 값이 없어도 버림
	if (NewAxisValue == 0)
	{
		return;
	}

	In_char->AddMovementInput(
		FRotationMatrix(
			In_char->GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);
#endif


	//GSLOG(Warning, TEXT("UpDown axisVal: %f"), NewAxisValue);
}
// 좌,우 이동 처리
void GsCameraModeFreeBase::LeftRight(float NewAxisValue, UGsGameObjectLocal* In_char)
{
#ifdef NEW_CAM_CHAR
#else
	if (In_char == nullptr ||
		In_char->Controller == nullptr)
	{
		return;
	}

	// 값이 없어도 버림
	if (NewAxisValue == 0)
	{
		return;
	}


	In_char->AddMovementInput(
		FRotationMatrix(
			In_char->GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);

#endif
	//GSLOG(Warning, TEXT("LeftRight axisVal: %f"), NewAxisValue);
}

// 앞 이동 처리
void GsCameraModeFreeBase::MoveForward(UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}

	if (auto movement = In_char->GetMovement())
	{
		FVector dir = FRotationMatrix(In_char->GetLocalCharacter()->Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
		movement->Move(dir, EGsGameObjectMoveDirType::Forward, 10.0f);
	}
}
// 뒤 이동 처리
void GsCameraModeFreeBase::MoveBackward(UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}
	if (auto movement = In_char->GetMovement())
	{
		FVector dir = FRotationMatrix(In_char->GetLocalCharacter()->Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
		movement->Move(dir, EGsGameObjectMoveDirType::Backward, -5.f);
	}
}
// 좌 이동 처리
void GsCameraModeFreeBase::MoveLeft(UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}
	if (auto movement = In_char->GetMovement())
	{
		FVector dir = FRotationMatrix(In_char->GetLocalCharacter()->Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
		movement->Move(dir, EGsGameObjectMoveDirType::SideStep, -5.f);
	}
}
// 우 이동 처리
void GsCameraModeFreeBase::MoveRight(UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}
	if (auto movement = In_char->GetMovement())
	{
		FVector dir = FRotationMatrix(In_char->GetLocalCharacter()->Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
		movement->Move(dir, EGsGameObjectMoveDirType::SideStep, 5.f);
	}
}

// 위, 아래 카메라 회전
void GsCameraModeFreeBase::LookUp(float NewAxisValue, UGsGameObjectLocal* In_char)
{
#ifdef NEW_CAM_CHAR
	if (In_char == nullptr)
	{
		return;
	}
	In_char->GetLocalCharacter()->AddControllerPitchInput(NewAxisValue);

#else
	if (In_char == nullptr ||
		In_char->Controller == nullptr)
	{
		return;
	}



	// 값이 없어도 버림
	if (NewAxisValue == 0)
	{
		return;
	}

	In_char->AddControllerPitchInput(NewAxisValue);
#endif
}
// 좌, 우 카메라 회전
void GsCameraModeFreeBase::Turn(float NewAxisValue, UGsGameObjectLocal* In_char)
{
#ifdef NEW_CAM_CHAR
	if (In_char == nullptr)
	{
		return;
	}
	In_char->GetLocalCharacter()->AddControllerYawInput(NewAxisValue);
#else
	if (In_char == nullptr ||
		In_char->Controller == nullptr)
	{
		return;
	}

	// 값이 없어도 버림
	if (NewAxisValue == 0)
	{
		return;
	}

	In_char->AddControllerYawInput(NewAxisValue);
#endif
}
