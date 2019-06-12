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
// ���� ��ȯ(���ڷ� ĳ����)
void GsCameraModeFreeBase::Enter(UGsGameObjectLocal* In_char, GsCameraModeManager* In_mng)
{
	if (In_char == nullptr)
	{
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

	// ���ε� ó��
	// ���� ĸ�ĸ� ���۷���(&)�� �ϰ� ������ ������ �ٸ��ּҸ� �Ѱ��༭
	// ���� ȣ��� �̻� ��Ȳ �߻���....
	// �����ʹ� ����(=)�� ĸ���ؾ���....
#ifdef OLD_FUNCTION
	inputBinding->FunctionMoveForward = [=](float val) {MoveForward(val, In_char); };
	inputBinding->FunctionMoveBackward = [=](float val) {MoveBackward(val, In_char); };
	inputBinding->FunctionMoveLeft = [=](float val) {MoveLeft(val, In_char); };
	inputBinding->FunctionMoveRight = [=](float val) {MoveRight(val, In_char); };
#else
	inputBinding->FunctionMoveForwardBackward = [=](float val) {UpDown(val, In_char); };
	inputBinding->FunctionMoveLeftRight = [=](float val) {LeftRight(val, In_char); };
#endif

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



	GSLOG(Warning, TEXT("GsCameraModeFreeBase  Enter"));
}
// ���� ����(���ڷ� ĳ����)
void GsCameraModeFreeBase::Exit(UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
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

	// ���ε� ����
#ifdef OLD_FUNCTION
	inputBinding->FunctionMoveForward = nullptr;
	inputBinding->FunctionMoveBackward = nullptr;
	inputBinding->FunctionMoveLeft = nullptr;
	inputBinding->FunctionMoveRight = nullptr;
#else
	inputBinding->FunctionMoveForwardBackward = nullptr;
	inputBinding->FunctionMoveLeftRight = nullptr;
#endif
	inputBinding->FunctionLookUp = nullptr;
	inputBinding->FunctionTurn = nullptr;


	GSLOG(Warning, TEXT("GsCameraModeFreeBase  exit"));
}
// ��������(���ڷ� ĳ����)
void GsCameraModeFreeBase::Update(UGsGameObjectLocal* In_char, float In_deltaTime, GsCameraModeManager* In_mng)
{
	GsCameraModeBase::Update(In_char, In_deltaTime, In_mng);

}

// ��,�Ʒ� �̵� ó��
void GsCameraModeFreeBase::UpDown(float NewAxisValue, UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}

	if (auto movement = In_char->GetMovement())
	{
		FVector dir = FRotationMatrix(In_char->GetLocalCharacter()->Controller->GetControlRotation()).GetUnitAxis(EAxis::X);
		movement->Move(dir, NewAxisValue);
	}


	//GSLOG(Warning, TEXT("UpDown axisVal: %f"), NewAxisValue);
}
// ��,�� �̵� ó��
void GsCameraModeFreeBase::LeftRight(float NewAxisValue, UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}
	if (auto movement = In_char->GetMovement())
	{
		FVector dir = FRotationMatrix(In_char->GetLocalCharacter()->Controller->GetControlRotation()).GetUnitAxis(EAxis::Y);	
		movement->Move(dir, NewAxisValue);
	}
	//GSLOG(Warning, TEXT("LeftRight axisVal: %f"), NewAxisValue);
}

// �� �̵� ó��
void GsCameraModeFreeBase::MoveForward(float NewAxisValue, UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}

	if (auto movement = In_char->GetMovement())
	{
		FVector dir = FRotationMatrix(In_char->GetLocalCharacter()->Controller->GetControlRotation()).GetUnitAxis(EAxis::X);
		//In_char->GetLocalCharacter()->AddMovementInput(dir, NewAxisValue);
		movement->Move(dir, NewAxisValue);
	}
}
// �� �̵� ó��
void GsCameraModeFreeBase::MoveBackward(float NewAxisValue, UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}
	if (auto movement = In_char->GetMovement())
	{
		FVector dir = FRotationMatrix(In_char->GetLocalCharacter()->Controller->GetControlRotation()).GetUnitAxis(EAxis::X);
		movement->Move(dir, NewAxisValue);
	}
}
// �� �̵� ó��
void GsCameraModeFreeBase::MoveLeft(float NewAxisValue, UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}
	if (auto movement = In_char->GetMovement())
	{
		FVector dir = FRotationMatrix(In_char->GetLocalCharacter()->Controller->GetControlRotation()).GetUnitAxis(EAxis::Y);
		//In_char->GetLocalCharacter()->AddMovementInput(dir, NewAxisValue);
		movement->Move(dir, NewAxisValue);
	}
}
// �� �̵� ó��
void GsCameraModeFreeBase::MoveRight(float NewAxisValue, UGsGameObjectLocal* In_char)
{
	if (In_char == nullptr)
	{
		return;
	}
	if (auto movement = In_char->GetMovement())
	{
		FVector dir = FRotationMatrix(In_char->GetLocalCharacter()->Controller->GetControlRotation()).GetUnitAxis(EAxis::Y);
		//In_char->GetLocalCharacter()->AddMovementInput(dir, NewAxisValue);
		movement->Move(dir, NewAxisValue);
	}
}

// ��, �Ʒ� ī�޶� ȸ��
void GsCameraModeFreeBase::LookUp(float NewAxisValue, UGsGameObjectLocal* In_char)
{

	if (In_char == nullptr)
	{
		return;
	}
	In_char->GetLocalCharacter()->AddControllerPitchInput(NewAxisValue);

}
// ��, �� ī�޶� ȸ��
void GsCameraModeFreeBase::Turn(float NewAxisValue, UGsGameObjectLocal* In_char)
{

	if (In_char == nullptr)
	{
		return;
	}
	In_char->GetLocalCharacter()->AddControllerYawInput(NewAxisValue);

}
