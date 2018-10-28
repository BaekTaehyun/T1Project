// Fill out your copyright notice in the Description page of Project Settings.

#include "T1Player.h"
#include "T1Project.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"

#include "ConstructorHelpers.h"


// Sets default values
AT1Player::AT1Player()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 케릭터에게 카메라 세팅합니다.
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	// 컴포넌트 종속관계설정
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);
	
	// 좌표세팅(상대좌표로 초기값을 지정합니다.)
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	// 스켈레탈 메쉬세팅(외형) 
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_HERO(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_RaggedElite.SK_CharM_RaggedElite"));
	//ConstructorHelpers::FObjectFinder (오브젝트 검색)
	if (SK_HERO.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_HERO.Object);
	}

	// 애니메이션을 플레이할 모드를 설정한다.
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	// 애니메이션 블루프린트는 애님 그래프 로지겡 따라 동작하는 케릭터 애니메이션 스템을 구동시키는데 이런한 시스템을 C++에선 애님인스턴드 클래스로 관리된다
	static ConstructorHelpers::FClassFinder<UAnimInstance> WARR_ANIM(TEXT("/Game/InfinityBladeWarriors/Character/WarriorAnimBP.WarriorAnimBP_C"));
	//ConstructorHelpers::FClassFinder  *주위 블루프린트 끝에 _C를 붙여서 검색함 (클래스 검색) 
	if (WARR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARR_ANIM.Class);
	}

	// 카메라를 어떻게 제어할지 설정함
	SetCameraControlMode(ECameraControlMode::DIABLO);

	ArmLengthSpeed = 5.0f;
	ArmRotationSpeed = 10.0f;
}

// Called when the game starts or when spawned
void AT1Player::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AT1Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	if (CurrentCameraControlMode == ECameraControlMode::DIABLO )
	{
		SpringArm->RelativeRotation = FMath::RInterpTo(SpringArm->RelativeRotation, ArmRotationTo, DeltaTime, ArmRotationSpeed);

		if (DirectionToMove.SizeSquared() > 0.0f)
		{
			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
			AddMovementInput(DirectionToMove);
		}
	}

}

// Called to bind functionality to input
void AT1Player::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	T1LOG_S(Warning);

	// 프로젝트 세팅에 명시된 인풋벨류를 실제로 케릭터 인풋과 바인딩하는 방법
	InputComponent->BindAxis(TEXT("UpDown"), this, &AT1Player::UpDown);
	InputComponent->BindAxis(TEXT("LeftRight"), this, &AT1Player::LeftRight);
	InputComponent->BindAxis(TEXT("Turn"), this, &AT1Player::Turn);
	InputComponent->BindAxis(TEXT("LookUp"), this, &AT1Player::LookUp);

	// 키와 액션바인딩
	InputComponent->BindAction(TEXT("CameraViewChange"), EInputEvent::IE_Pressed, this, &AT1Player::CameraViewChange);	
}


// 콜링 순서를 파악 하기 위한 오버라이딩	
void AT1Player::PostInitializeComponents()
{	
	Super::PostInitializeComponents();
	T1LOG_S(Warning);
}

// 콜링 순서를 파악 하기 위한 오버라이딩	
void AT1Player::PossessedBy(AController* NewController)
{
	T1LOG_S(Warning);
	Super::PossessedBy(NewController);
}

void AT1Player::UpDown(float newAxisValue)
{
	//바인딩된후 실제 이동처리
	if (CurrentCameraControlMode == ECameraControlMode::FOLLOW)
	{
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), newAxisValue);
	}
	else if (CurrentCameraControlMode == ECameraControlMode::DIABLO)
	{
		DirectionToMove.X = newAxisValue;
	}
	
}

void AT1Player::LeftRight(float newAxisValue)
{
	if (CurrentCameraControlMode == ECameraControlMode::FOLLOW)
	{
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), newAxisValue);
	}
	else if (CurrentCameraControlMode == ECameraControlMode::DIABLO)
	{
		DirectionToMove.Y = newAxisValue;
	}
	
}

void AT1Player::LookUp(float newAxisValue)
{
	if (CurrentCameraControlMode == ECameraControlMode::FOLLOW)
	{
		AddControllerPitchInput(newAxisValue);
	}
	
}

void AT1Player::Turn(float newAxisValue)
{
	if (CurrentCameraControlMode == ECameraControlMode::FOLLOW)
	{
		AddControllerYawInput(newAxisValue);
	}
}

void AT1Player::CameraViewChange()
{
	bool bFollow = CurrentCameraControlMode == ECameraControlMode::FOLLOW;
	GetController()->SetControlRotation(bFollow ? GetActorRotation() : SpringArm->RelativeRotation);
	SetCameraControlMode(bFollow ? ECameraControlMode::DIABLO : ECameraControlMode::FOLLOW);
}

void AT1Player::SetCameraControlMode(ECameraControlMode inMode)
{
	CurrentCameraControlMode = inMode;
	if (CurrentCameraControlMode == ECameraControlMode::FOLLOW)
	{
		/*SpringArm->TargetArmLength = 450.0f;
		SpringArm->SetRelativeRotation(FRotator::ZeroRotator);*/
		ArmLengthTo = 450.0f;
		// 스프링암(카메라)를 컨트롤러 방향에 매칭
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = SpringArm->bInheritRoll = SpringArm->bInheritYaw = true;
		SpringArm->bDoCollisionTest = true;
		// 케릭터(모델)와 컨트롤러의 Z축회전(방향을 끊어버림)
		bUseControllerRotationYaw = false;

		// 컨트롤러가 바라보는 방향으로 이동하도록 설정
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	}
	else if (CurrentCameraControlMode == ECameraControlMode::DIABLO)
	{
		ArmLengthTo = 900.0f;
		ArmRotationTo = FRotator(-45.0f, 0.0f, 0.0f);
		//SpringArm->TargetArmLength = 800.0f;
		//SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
		SpringArm->bUsePawnControlRotation = false;
		SpringArm->bInheritPitch = SpringArm->bInheritRoll = SpringArm->bInheritYaw = false;
		SpringArm->bDoCollisionTest = false;

		// 딱딱한 방향때문에 주석
		//bUseControllerRotationYaw = true;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);

	}
}

