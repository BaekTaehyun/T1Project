// Fill out your copyright notice in the Description page of Project Settings.

#include "GsNpcCharacter.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AGsNpcCharacter::AGsNpcCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

AGsNpcCharacter::~AGsNpcCharacter()
{
}

// Called when the game starts or when spawned
void AGsNpcCharacter::BeginPlay()
{
	Super::BeginPlay();

	DynamicMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
	GetMesh()->SetMaterial(0, DynamicMaterial);
}

// Called every frame
void AGsNpcCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AGsNpcCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AGsNpcCharacter::SetColor(FLinearColor Color)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue("Param", Color);
	}
}

