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
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Delegate.h"
#include "ConstructorHelpers.h"
#include "DrawDebugHelpers.h"
#include "T1AWeapon.h"
#include "T1PlayerStatComponent.h"
#include "T1PlayerWidget.h"
#include "T1AIController.h"
#include "UserWidget.h"
#include "MyUserWidget.h"

#include "../GameService/Camera/GsCameraModeManager.h"

// Sets default values
AT1Player::AT1Player()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// �ɸ��Ϳ��� ī�޶� �����մϴ�.
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	// ������ ���۳�Ʈ ����
	//CharInfoData = CreateDefaultSubobject<UDataActorComponent>(TEXT("CHARINFO"));

	// �÷��̾� ���� ������Ʈ
	PlayerStat = CreateDefaultSubobject<UT1PlayerStatComponent>(TEXT("PLAYERSTAT"));

	// HP ���� ���̱�
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));

	// ������Ʈ ���Ӱ��輳��
	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);
	HPBarWidget->SetupAttachment(GetMesh());
	
	// ��ǥ����(�����ǥ�� �ʱⰪ�� �����մϴ�.)
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	// ���̷�Ż �޽�����(����) 
	// ConstructorHelpers �ش�迭�� �Լ��� static���� �����ѰͿ� ����(������������ �迭 Ŭ������ �ν��Ͻ̵ɶ����� ȣ���ϴ°��� ����)
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_HERO(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_RaggedElite.SK_CharM_RaggedElite"));
	//ConstructorHelpers::FObjectFinder (������Ʈ �˻�)
	if (SK_HERO.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_HERO.Object);
	}

	// �ִϸ��̼��� �÷����� ��带 �����Ѵ�.
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	// �ִϸ��̼� �������Ʈ�� �ִ� �׷��� ������ ���� �����ϴ� �ɸ��� �ִϸ��̼� ������ ������Ű�µ� �̷��� �ý����� C++���� �ִ��ν��ϵ� Ŭ������ �����ȴ�
	static ConstructorHelpers::FClassFinder<UAnimInstance> WARR_ANIM(TEXT("/Game/InfinityBladeWarriors/Character/WarriorAnimBP.WarriorAnimBP_C"));
	//ConstructorHelpers::FClassFinder  *���� �������Ʈ ���� _C�� �ٿ��� �˻��� (Ŭ���� �˻�) : WarriorAnimBP_C
	if (WARR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARR_ANIM.Class);
	}

	// ī�޶� ��� �������� ������
	SetCameraControlMode(ECameraControlMode::DIABLO);

	// ī�޶� ���氣 ���� ����
	ArmLengthSpeed = 5.0f;
	ArmRotationSpeed = 10.0f;

	//����ó��
	GetCharacterMovement()->JumpZVelocity = 800.0f;

	//��Ÿ�� ���� �÷���
	IsAttacking = false;

	//�޺�ó�� �߰�
	MaxComboCount = 4;
	AttackEndComboState();

	//�ݸ��� �������� Ŀ���� ����
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("T1Character"));

	//���ݰŸ� ����� ������
	AttackRadius = 50.0f;
	AttackRange = 200.0f;

	// ���� ���Ͽ� ���̱�
	FName WeaponSocket(TEXT("hand_rSocket"));
	if (GetMesh()->DoesSocketExist(WeaponSocket))
	{
		Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(
			TEXT("/Game/InfinityBladeWeapons/Weapons/Blade/Swords/Blade_DragonSword/SK_Blade_DragonSword.SK_Blade_DragonSword"));
		if (SK_WEAPON.Succeeded())
		{
			Weapon->SetSkeletalMesh(SK_WEAPON.Object);
		}
		Weapon->SetupAttachment(GetMesh(), WeaponSocket);
	}

	// ���� ������ �ε�
	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(
		TEXT("/Game/UI/UI_HPBar.UI_HPBar_C"));
	if (UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
	}

	AIControllerClass = AT1AIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	
}

// Called when the game starts or when spawned
void AT1Player::BeginPlay()
{
	Super::BeginPlay();

	// UI Test
	CreateUI();

	//bak1210 ���Ͽ� �⺻Ŭ���� �Ҵ��ϴ� �ڵ�
	/*FName WeaponSocket(TEXT("hand_rSocket"));
	auto CurWeapon = GetWorld()->SpawnActor<AT1AWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);
	if (nullptr != CurWeapon)
	{
		CurWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocket);
	}*/

	// ĳ���� ����
	if (GsCameraModeSingle::Instance != nullptr)
	{
		GsCameraModeSingle::Instance->SetCharacter(this);
	}
}

void AT1Player::CreateUI()
{
	UBlueprintGeneratedClass* _isLoaded = LoadObject<UBlueprintGeneratedClass>(nullptr, TEXT("/Game/Blueprint/Test_BluePrint.Test_BluePrint_C"));
	if (nullptr == _isLoaded)
	{	
		UE_LOG(LogTemp, Log, TEXT("_isLoaded is nullptr !!"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("_isLoaded Success !!"));
		if (nullptr != GetWorld())
		{
			GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
			UUserWidget* _createWidget = CreateWidget(GetWorld()->GetFirstPlayerController(), _isLoaded);
			if (nullptr == _createWidget)
			{
				UE_LOG(LogTemp, Log, TEXT("_createWidget is nullptr !!"));
			}
			else
			{
				_createWidget->AddToViewport();
			}
		}
	}
}

bool AT1Player::CanSetWeapon()
{
	return (nullptr == CurrentWeapon);
}

void AT1Player::SetWeapon(AT1AWeapon* NewWeapon)
{
	T1CHECK(nullptr != NewWeapon && nullptr == CurrentWeapon);
	FName WeaponSocket(TEXT("hand_rSocket"));
	if (nullptr != NewWeapon)
	{
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}

}

// Called every frame
void AT1Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#ifdef CAM_MODE
	if (GsCameraModeSingle::Instance != nullptr)
	{
		GsCameraModeSingle::Instance->Update(DeltaTime);
	}
#else
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
#endif
}

// Called to bind functionality to input
void AT1Player::SetupPlayerInputComponent(UInputComponent* inputComponent)
{
	Super::SetupPlayerInputComponent(inputComponent);

	T1LOG_S(Warning);

	// ������Ʈ ���ÿ� ��õ� ��ǲ������ ������ �ɸ��� ��ǲ�� ���ε��ϴ� ���
	InputComponent->BindAxis(TEXT("UpDown"), this, &AT1Player::UpDown);
	InputComponent->BindAxis(TEXT("LeftRight"), this, &AT1Player::LeftRight);
	InputComponent->BindAxis(TEXT("Turn"), this, &AT1Player::Turn);
	InputComponent->BindAxis(TEXT("LookUp"), this, &AT1Player::LookUp);

	// Ű�� �׼ǹ��ε�
	InputComponent->BindAction(TEXT("CameraViewChange"), EInputEvent::IE_Pressed, this, &AT1Player::CameraViewChange);	

	InputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AT1Player::Jump);
	InputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AT1Player::Attack);

	InputComponent->BindAction(TEXT("ZoomIn"),
		EInputEvent::IE_Pressed,
		this,
		&AT1Player::ZoomIn);

	InputComponent->BindAction(TEXT("ZoomOut"),
		EInputEvent::IE_Pressed,
		this,
		&AT1Player::ZoomOut);
}


// �ݸ� ������ �ľ� �ϱ� ���� �������̵�	
void AT1Player::PostInitializeComponents()
{	
	Super::PostInitializeComponents();
	T1LOG_S(Warning);

	T1Anim = Cast<UT1AnimInstance>(GetMesh()->GetAnimInstance());
	T1CHECK(nullptr != T1Anim);

	/*
	  OnMontageEnded ��������Ʈ�� �������Ʈ�� ȣȯ�Ǵ� �����ܿ��� �������� �Լ��� ������ �־
	  �ൿ�� ������ ��ϵ� ����Լ��鿡�� ��� �˷��ִ� ��ɵ� �����Ѵ�. �̷��� ��������Ʈ�� ��Ƽ�ɽ�Ʈ
	  ���� ����Ʈ(Multicast Delegate)��� �Ѵ�.

	  �ִ� �ν��Ͻ� ����� ����� OnMontageEnded�� ����ϴ� ��������Ʈ�� ������ �ڵ�� #include "Delegate.h" �� ����
	  �𸮾� �������� ��������Ʈ�� ������ �𸮾��� �����ϴ� ��ũ�θ� ���ؼ� ���ǵǸ�, �̷��� ���ǵ� ��������Ʈ ������
	  �ñ״�ó��� �Ѵ�.
	  ��ũ���̱⿡ ���ڸ������� �������� ����
	*/
	T1Anim->OnMontageEnded.AddDynamic(this, &AT1Player::OnAttackMontageEnded);

	/*
		UT1AnimInstance�� ���ǵ� ��������Ʈ�� ��ũ(��Ƽ���̸� ���� �Լ����)
		�ش���ٽ��� C++ ����
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

	//�̸������� ��������Ʈ�� �浹 ó�� �̺�Ʈ ���
	T1Anim->OnAttackHitCheck.AddUObject(this, &AT1Player::AttackCheck);

	PlayerStat->OnHPIsZero.AddLambda([this]()-> void {
		T1LOG(Warning, TEXT("OnHpIsZero"));
		T1Anim->SetDeadAnim();
		SetActorEnableCollision(false);
	});

	auto PlayerWidget = Cast<UT1PlayerWidget>(HPBarWidget->GetUserWidgetObject());
	if (nullptr != PlayerWidget)
	{
		PlayerWidget->BindPlayerStat(PlayerStat);
	}
}

// �ݸ� ������ �ľ� �ϱ� ���� �������̵�	
void AT1Player::PossessedBy(AController* NewController)
{
	T1LOG_S(Warning);
	Super::PossessedBy(NewController);
}

void AT1Player::UpDown(float newAxisValue)
{
#ifdef CAM_MODE
	if (FunctionUpDown != nullptr)
	{
		FunctionUpDown(newAxisValue);
	}
#else
	//���ε����� ���� �̵�ó��
	if (CurrentCameraControlMode == ECameraControlMode::FOLLOW)
	{
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), newAxisValue);
	}
	else if (CurrentCameraControlMode == ECameraControlMode::DIABLO)
	{
		DirectionToMove.X = newAxisValue;
	}
#endif
}

void AT1Player::LeftRight(float newAxisValue)
{
#ifdef CAM_MODE
	if (FunctionLeftRight != nullptr)
	{
		FunctionLeftRight(newAxisValue);
	}
#else
	if (CurrentCameraControlMode == ECameraControlMode::FOLLOW)
	{
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), newAxisValue);
	}
	else if (CurrentCameraControlMode == ECameraControlMode::DIABLO)
	{
		DirectionToMove.Y = newAxisValue;
	}
#endif
}

void AT1Player::LookUp(float newAxisValue)
{
#ifdef CAM_MODE
	if (FunctionLookUp != nullptr)
	{
		FunctionLookUp(newAxisValue);
	}
#else
	if (CurrentCameraControlMode == ECameraControlMode::FOLLOW)
	{
		AddControllerPitchInput(newAxisValue);
	}
#endif
	
}

void AT1Player::Turn(float newAxisValue)
{
#ifdef CAM_MODE
	if (FunctionTurn != nullptr)
	{
		FunctionTurn(newAxisValue);
	}
#else
	if (CurrentCameraControlMode == ECameraControlMode::FOLLOW)
	{
		AddControllerYawInput(newAxisValue);
	}
#endif
}

void AT1Player::CameraViewChange()
{
#ifdef CAM_MODE
	if (GsCameraModeSingle::Instance != nullptr)
	{
		GsCameraModeSingle::Instance->NextStep();
	}
#else

	bool bFollow = CurrentCameraControlMode == ECameraControlMode::FOLLOW;
	GetController()->SetControlRotation(bFollow ? GetActorRotation() : SpringArm->RelativeRotation);
	SetCameraControlMode(bFollow ? ECameraControlMode::DIABLO : ECameraControlMode::FOLLOW);
#endif

}

void AT1Player::ZoomIn()
{
	if (FunctionZoomIn != nullptr)
	{
		FunctionZoomIn();
	}
}

void AT1Player::ZoomOut()
{
	if (FunctionZoomOut != nullptr)
	{
		FunctionZoomOut();
	}
}

void AT1Player::SetCameraControlMode(ECameraControlMode inMode)
{
	CurrentCameraControlMode = inMode;
	if (CurrentCameraControlMode == ECameraControlMode::FOLLOW)
	{
		/*SpringArm->TargetArmLength = 450.0f;
		SpringArm->SetRelativeRotation(FRotator::ZeroRotator);*/
		ArmLengthTo = 450.0f;
		// ��������(ī�޶�)�� ��Ʈ�ѷ� ���⿡ ��Ī
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = SpringArm->bInheritRoll = SpringArm->bInheritYaw = true;
		SpringArm->bDoCollisionTest = true;
		// �ɸ���(��)�� ��Ʈ�ѷ��� Z��ȸ��(������ �������)
		bUseControllerRotationYaw = false;

		// ��Ʈ�ѷ��� �ٶ󺸴� �������� �̵��ϵ��� ����
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

		// ������ ���⶧���� �ּ�
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

void AT1Player::AttackCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	FVector TraceVec = GetActorForwardVector() * AttackRange;;

	bool bResult = GetWorld()->SweepSingleByChannel(HitResult, 
		GetActorLocation(),
		GetActorLocation() + TraceVec,
		FQuat::Identity, 
		ECollisionChannel::ECC_EngineTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius),
		Params);

	// ����׿� ������ �޼ҵ� ����
#if ENABLE_DRAW_DEBUG
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Red : FColor::Green;
	float DrawLifeTime = 5.0f;

	DrawDebugCapsule(GetWorld(), Center, HalfHeight, AttackRadius, CapsuleRot, DrawColor, false, DrawLifeTime);
#endif

	if (bResult == false) { return; }
	if (HitResult.Actor.IsValid() == false) { return; }

	T1LOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());

	FDamageEvent damageEvent;
	HitResult.Actor->TakeDamage(PlayerStat->GetAttack(), damageEvent, GetController(), this);

}

float AT1Player::TakeDamage(float DamageAmout, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmout, DamageEvent, EventInstigator, DamageCauser);
	T1LOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

	PlayerStat->SetDamage(FinalDamage);
	return FinalDamage;
}


