// Fill out your copyright notice in the Description page of Project Settings.

#include "T1PlayerStatComponent.h"
#include "FT1PlayerData.h"
#include "T1GameInstance.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UT1PlayerStatComponent::UT1PlayerStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	//InitializeComponent �Լ��� ȣ���ϰ� ������ ������ ���� �÷��װ� �����Ǿ�� ȣ��˴ϴ�.
	bWantsInitializeComponent = true;
	// ...

	Level = 1;
}


// Called when the game starts
void UT1PlayerStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


//InitializeComponent �Լ��� ȣ���ϰ� ������ ������ ���� �÷��װ� �����Ǿ�� ȣ��˴ϴ�.
//bWantsInitializeComponent = true;
void UT1PlayerStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetNewLevel(Level);

}

void UT1PlayerStatComponent::SetNewLevel(int32 NewLevel)
{
	auto T1GameInstance = Cast<UT1GameInstance>(UGameplayStatics::GetGameInstance((UObject*)GetWorld()));

	T1CHECK(nullptr != T1GameInstance);
	CurrentStatData = T1GameInstance->GetT1PlayerData(NewLevel);
	if (nullptr != CurrentStatData)
	{
		Level = NewLevel;
		//CurrentHP = CurrentStatData->MaxHP;
		SetHp(CurrentStatData->MaxHP);

	}
	else
	{
		T1LOG(Error, TEXT("Level (%d} data does't exist"), NewLevel);
	}
}


// Called every frame
void UT1PlayerStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UT1PlayerStatComponent::SetDamage(float NewDamage)
{
	T1CHECK(nullptr != CurrentStatData);
	SetHp(FMath::Clamp<float>(CurrentHP - NewDamage, 0.0f, CurrentStatData->MaxHP));
	if (CurrentHP <= 0.0f)
	{
		OnHPIsZero.Broadcast();
	}
}

// �𸮾� ���������� ���ð����� ������ �����Ҷ� KINDA_SMALL_NUMBER��� ��ũ�θ� �����Ѵ�.
void UT1PlayerStatComponent::SetHp(float NewHp)
{
	CurrentHP = NewHp;
	OnHPIsChanged.Broadcast();
	if (CurrentHP < KINDA_SMALL_NUMBER)
	{
		CurrentHP = 0.0f;
		OnHPIsZero.Broadcast();
	}
}

float UT1PlayerStatComponent::GetAttack()
{
	T1CHECK(nullptr != CurrentStatData, 0.0f);
	return CurrentStatData->Attack;
}

float UT1PlayerStatComponent::GetHpRatio()
{
	T1CHECK(nullptr != CurrentStatData, 0.0f);

	return (CurrentStatData->MaxHP < KINDA_SMALL_NUMBER) ? 0.0f : (CurrentHP / CurrentStatData->MaxHP);
}

