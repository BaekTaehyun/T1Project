// Fill out your copyright notice in the Description page of Project Settings.

#include "T1AWeapon.h"
#include "ConstructorHelpers.h"
#include "CoreMinimal.h"
#include "Engine/SkeletalMesh.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AT1AWeapon::AT1AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
	RootComponent = dynamic_cast<USceneComponent*>(Weapon);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(
		TEXT("/Game/InfinityBladeWeapons/Weapons/Blunt/Blunt_Temperance/SK_Blunt_Temperance.SK_Blunt_Temperance"));

	if (SK_WEAPON.Succeeded())
	{
		Weapon->SetSkeletalMesh(SK_WEAPON.Object);
	}

	Weapon->SetCollisionProfileName(TEXT("NoCollision"));

}

// Called when the game starts or when spawned
void AT1AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AT1AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

