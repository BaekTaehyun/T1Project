// Fill out your copyright notice in the Description page of Project Settings.

#include "DataActorComponent.h"
#include "T1Project.h"

#include "UObject/ConstructorHelpers.h"


// Sets default values for this component's properties
UDataActorComponent::UDataActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	Level = 1;

	// ...
	FString CharInfoDataPath = TEXT("/Game/Data/CCharInfoData.CCharInfoData");
	//FString CharInfoDataPath = TEXT("/Content/Data/CCharInfoData.json");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_CHARACTER(*CharInfoDataPath);
	T1CHECK(DT_CHARACTER.Succeeded());
	//CharacterTable = DT_CHARACTER;
	CharacterTable = DT_CHARACTER.Object;
	T1CHECK(CharacterTable->RowMap.Num() > 0);

	//SetNewLevel(Level);
}


// Called when the game starts
void UDataActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...	
}

void UDataActorComponent::InitializeComponent()
{
	Super::InitializeComponent();

	//FString CharInfoDataPath = TEXT("/Content/Data/CCharInfoData.CCharInfoData");
	////FString CharInfoDataPath = TEXT("/Content/Data/CCharInfoData.json");
	//static ConstructorHelpers::FObjectFinder<UDataTable> DT_CHARACTER(*CharInfoDataPath);
	//T1CHECK(DT_CHARACTER.Succeeded());
	//CharacterTable = DT_CHARACTER.Object;
	//T1CHECK(CharacterTable->RowMap.Num() > 0);

	SetNewLevel(Level);
	//T1LOG(Warning, TEXT("MaxHP of Level 1 Character : %d"), GetCharInfoData(1)->MaxHP);
}

void UDataActorComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UDataActorComponent, Level))
	{
		//various uproperty tricks, see link
		SetNewLevel(Level);
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

// Called every frame
void UDataActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

}

FCharInfoData* UDataActorComponent::GetCharInfoData(int32 Level)
{
	return CharacterTable->FindRow<FCharInfoData>(*FString::FromInt(Level), TEXT(""));
}

void UDataActorComponent::SetNewLevel(int32 NewLevel)
{
	CurrentCharInfoData = GetCharInfoData(NewLevel);
	if(CurrentCharInfoData != nullptr)
	{
		Level = NewLevel;
		CurrentHP = CurrentCharInfoData->MaxHP;
	}
	else
	{
		T1LOG(Error, TEXT("Level (%d) data doesn't exist"), NewLevel);
	}
}

const UDataTable* UDataActorComponent::GetDataTable() const
{
	return CharacterTable;
}

UDataTable* UDataActorComponent::GetDataTable()
{
	return CharacterTable;
}

void UDataActorComponent::SetInfoData(int32 EditLevel)
{
	EditLevel;
}