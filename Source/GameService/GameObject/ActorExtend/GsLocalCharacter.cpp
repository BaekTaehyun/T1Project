// Fill out your copyright notice in the Description page of Project Settings.

#include "GsLocalCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameObject/Input/GsInputBindingBase.h"
#include "UI/Common/GsPoolingWidgetComponent.h"

AGsLocalCharacter::~AGsLocalCharacter()
{
}

// Sets default values
AGsLocalCharacter::AGsLocalCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//InputBinder = CreateDefaultSubobject<UGsInputBindingLocalPlayer>(TEXT("CusomInputBinder"));

	PlayerWidget = CreateDefaultSubobject<UGsPoolingWidgetComponent>(TEXT("PlayerWidget"));
	PlayerWidget->SetupAttachment(GetMesh());
	PlayerWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	PlayerWidget->SetWidgetSpace(EWidgetSpace::Screen);
	PlayerWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
}

void AGsLocalCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Animation = Cast<UGsAnimInstanceState>(GetMesh()->GetAnimInstance());
	RestoreCollisionType = GetCapsuleComponent()->GetCollisionEnabled();
}

// Called when the game starts or when spawned
void AGsLocalCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AGsLocalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AGsLocalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (InputBinder)
	{
		InputBinder->SetBinding(PlayerInputComponent);
	}
}

#pragma todo("by: CapsuleComponent 비활성 방식 R&D 필요")
void AGsLocalCharacter::EnableCollision()
{
	GetCapsuleComponent()->SetCollisionEnabled(RestoreCollisionType);
}

void AGsLocalCharacter::DisableCollision()
{
	RestoreCollisionType = GetCapsuleComponent()->GetCollisionEnabled();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}