// Fill out your copyright notice in the Description page of Project Settings.

#include "GsNpcPawn.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameObject/Component/GsAnimInstanceState.h"

// Sets default values
AGsNpcPawn::AGsNpcPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("NpcCollision"));
	CapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
	CapsuleComponent->CanCharacterStepUpOn = ECB_No;
	CapsuleComponent->SetShouldUpdatePhysicsVolume(true);
	CapsuleComponent->SetCanEverAffectNavigation(false);
	CapsuleComponent->bDynamicObstacle = true;

	RootComponent = CapsuleComponent;

    Mesh = CreateOptionalDefaultSubobject<USkeletalMeshComponent>(TEXT("NpcSkeletalMesh"));
    Mesh->AlwaysLoadOnClient = true;
    Mesh->AlwaysLoadOnServer = true;
    Mesh->bOwnerNoSee = false;
    Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
    Mesh->bCastDynamicShadow = true;
    Mesh->bAffectDynamicIndirectLighting = true;
    Mesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
    Mesh->SetupAttachment(CapsuleComponent);
    Mesh->SetCollisionProfileName(TEXT("NpcMesh"));
    Mesh->SetGenerateOverlapEvents(false);
    Mesh->SetCanEverAffectNavigation(false);

    MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("NpcMovement"));
    MovementComponent->UpdatedComponent = CapsuleComponent;
}

AGsNpcPawn::~AGsNpcPawn()
{
}

// Called when the game starts or when spawned
void AGsNpcPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGsNpcPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGsNpcPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AGsNpcPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Mesh)
	{
		Animation = Cast<UGsAnimInstanceState>(Mesh->GetAnimInstance());
	}
}


