// Fill out your copyright notice in the Description page of Project Settings.

#include "T1AItemBox.h"
#include "T1AWeapon.h"
#include "T1Player.h"
#include "CoreMinimal.h"
#include "ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"

// Sets default values
AT1AItemBox::AT1AItemBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BOX"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));

	RootComponent = Trigger;
	Box->SetupAttachment(RootComponent);
	Effect->SetupAttachment(RootComponent);

	Trigger->SetBoxExtent(FVector(40.0f, 42.0f, 30.0f));
	static ConstructorHelpers::FObjectFinderOptional<UStaticMesh> SM_BOX(
		TEXT("/Game/InfinityBladeGrassLands/Environments/Breakables/StaticMesh/Box/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1"));
	if (SM_BOX.Succeeded())
	{
		Box->SetStaticMesh(SM_BOX.Get());
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_CHEST_OPEN(
		TEXT("/Game/InfinityBladeGrassLands/Effects/FX_Treasure/Chest/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh"));

	if (P_CHEST_OPEN.Succeeded())
	{
		Effect->SetTemplate(P_CHEST_OPEN.Object);
		Effect->bAutoActivate = false;
	}

	Box->SetRelativeLocation(FVector(0.0f, -3.5f, -30.0f));

	Trigger->SetCollisionProfileName(TEXT("ItemBox"));
	Box->SetCollisionProfileName(TEXT("NoCollision"));

	WeaponItemClass = AT1AWeapon::StaticClass();
}

// Called when the game starts or when spawned
void AT1AItemBox::BeginPlay()
{
	Super::BeginPlay();
	
}

void AT1AItemBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AT1AItemBox::OnCharacterOverlap);
}

void AT1AItemBox::OnCharacterOverlap(UPrimitiveComponent* OverlappedComponent, AActor*
	OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	T1LOG_S(Warning);

	auto Player = Cast<AT1Player>(OtherActor);
	T1CHECK(nullptr != Player);

	if (nullptr != Player && nullptr != WeaponItemClass)
	{
		if (Player->CanSetWeapon())
		{
			auto newWeapon = GetWorld()->SpawnActor<AT1AWeapon>(WeaponItemClass, FVector::ZeroVector, FRotator::ZeroRotator);
			Player->SetWeapon(newWeapon);

			Effect->Activate(true);

			//bak1210:
			//엑터의 컴포넌트에서 시각적인 기능을 끄고자할때 사용하는 함수로 SetVisbility와 SetHiddenInGame이 있다.
			//SetVisibility는 해당 컴포넌트의 시각적인 기능을 아예없애는 함수다. 
			//Visibility 옵션을 끈 컴포는터는 에디터 화면과 게임프레이 화면에서 모두사라진다.
			//하지만 HiddenInGame 옵션은 에디터 레벨 작업을 할 때는 보여주되, 게임플레이중에는 사라진다.

			Box->SetHiddenInGame(true, true);
			SetActorEnableCollision(false);

			Effect->OnSystemFinished.AddDynamic(this, &AT1AItemBox::OnEffectFinish);
		}
		else
		{
			T1LOG(Warning, TEXT("%s can't equip Weapon Currently"), *Player->GetName());
		}
	}
}

// Called every frame
void AT1AItemBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AT1AItemBox::OnEffectFinish(UParticleSystemComponent* PSystem)
{
	Destroy();
}

