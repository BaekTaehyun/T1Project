 // Fill out your copyright notice in the Description page of Project Settings.
	 
#include "GsPlayer.h"
#include "GameFrameWork/CharacterMovementComponent.h"
	 
// Sets default values
AGsPlayer::AGsPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGsPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGsPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGsPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AGsPlayer::SetPlayerSpawnState(bool in_spawn)
{
	UPawnMovementComponent* pawnMovComp = GetMovementComponent();

	if (pawnMovComp)
	{
		UCharacterMovementComponent* charMovComp = Cast<UCharacterMovementComponent>(pawnMovComp);

		if (charMovComp)
		{
			if (in_spawn)
			{
				charMovComp->GravityScale = 1.0f;
			}
			else
			{
				charMovComp->GravityScale = 0.0f;
			}
		}
	}
}

