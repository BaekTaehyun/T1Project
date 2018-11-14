// Fill out your copyright notice in the Description page of Project Settings.

#include "T1Player.h"
#include "T1Project.h"
#include "T1AnimInstance.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "Delegate.h"
#include "ConstructorHelpers.h"


// Sets default values
AT1Player::AT1Player()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 케릭터에게 카메라 세팅합니다.
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	// 데이터 컴퍼넌트 세팅
	CharInfoData = CreateDefaultSubobject<UDataActorComponent>(TEXT("CHARINFO"));

	// 컴포넌트 종속관계설정
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);
	
	// 좌표세팅(상대좌표로 초기값을 지정합니다.)
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	// 스켈레탈 메쉬세팅(외형) 
	// ConstructorHelpers 해당계열의 함수를 static으로 선언한것에 유의(생성자임으로 계열 클래스가 인스턴싱될때마다 호출하는것을 방지)
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_HERO(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_RaggedElite.SK_CharM_RaggedElite"));
	//ConstructorHelpers::FObjectFinder (오브젝트 검색)
	if (SK_HERO.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_HERO.Object);
	}

	// 애니메이션을 플레이할 모드를 설정한다.
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	// 애니메이션 블루프린트는 애님 그래프 로직에 따라 동작하는 케릭터 애니메이션 스템을 구동시키는데 이런한 시스템을 C++에선 애님인스턴드 클래스로 관리된다
	static ConstructorHelpers::FClassFinder<UAnimInstance> WARR_ANIM(TEXT("/Game/InfinityBladeWarriors/Character/WarriorAnimBP.WarriorAnimBP_C"));
	//ConstructorHelpers::FClassFinder  *주위 블루프린트 끝에 _C를 붙여서 검색함 (클래스 검색) : WarriorAnimBP_C
	if (WARR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARR_ANIM.Class);
	}

	// 카메라를 어떻게 제어할지 설정함
	SetCameraControlMode(ECameraControlMode::DIABLO);

	// 카메라 변경간 보간 세팅
	ArmLengthSpeed = 5.0f;
	ArmRotationSpeed = 10.0f;

	//점프처리
	GetCharacterMovement()->JumpZVelocity = 800.0f;

	//몽타주 공격 플레그
	IsAttacking = false;

	//콤보처리 추가
	MaxComboCount = 4;
	AttackEndComboState();
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

	InputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AT1Player::Jump);
	InputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AT1Player::Attack);
}


// 콜링 순서를 파악 하기 위한 오버라이딩	
void AT1Player::PostInitializeComponents()
{	
	Super::PostInitializeComponents();
	T1LOG_S(Warning);

	T1Anim = Cast<UT1AnimInstance>(GetMesh()->GetAnimInstance());
	T1CHECK(nullptr != T1Anim);

	/*
	  OnMontageEnded 델리게이트는 블루프린트와 호환되는 성질외에도 여러개의 함수를 받을수 있어서
	  행동이 끝나면 등록된 모든함수들에게 모두 알려주는 기능도 제공한다. 이러한 델리게이트를 멀티케스트
	  델리 게이트(Multicast Delegate)라고 한다.

	  애님 인스턴스 헤더에 선언된 OnMontageEnded가 사용하는 델리게이트를 정희한 코드는 #include "Delegate.h" 에 정의
	  언리얼 엔진에서 델리게이트의 선언은 언리얼이 제공하는 매크로를 통해서 정의되며, 이렇게 정의된 델리게이트 형식을
	  시그니처라고 한다.
	  매크로이기에 인텔리센스가 동작하지 않음
	*/
	T1Anim->OnMontageEnded.AddDynamic(this, &AT1Player::OnAttackMontageEnded);

	/*
		UT1AnimInstance에 정의된 델리게이트에 링크(노티파이를 받을 함수등록)
		해당람다식은 C++ 람다
	*/

	T1Anim->OnNextAttackCheck.AddLambda([this]()->void
	{
		T1LOG(Warning, TEXT("OnNextAttackCheck"));
		CanNextCombo = false;

		if (IsComboInputOn)
		{
			AttackStartComboState();
			T1Anim->JumpToAttackMontageSection(CurrentComboIndex);
		}
	});
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

void AT1Player::Attack()
{
	T1LOG_S(Warning);

	if (IsAttacking)
	{
		T1CHECK(FMath::IsWithinInclusive<int32>(CurrentComboIndex, 1, MaxComboCount));
		if (CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else
	{
		T1CHECK(CurrentComboIndex == 0);
		AttackStartComboState();
		T1Anim->PlayAttackMontage();
		T1Anim->JumpToAttackMontageSection(CurrentComboIndex);
		IsAttacking = true;

	}	
}

void AT1Player::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	T1CHECK(FMath::IsWithinInclusive<int32>(CurrentComboIndex, 0, MaxComboCount - 1));
	CurrentComboIndex = FMath::Clamp<int32>(CurrentComboIndex + 1, 1, MaxComboCount);
}

void AT1Player::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentComboIndex = 0;
}

void AT1Player::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterruped)
{
	T1CHECK(IsAttacking);
	IsAttacking = false;
	AttackEndComboState();
}


