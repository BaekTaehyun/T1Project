// Fill out your copyright notice in the Description page of Project Settings.

#include "T1GameInstance.h"
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FT1PlayerData.h"
#include "ConstructorHelpers.h"

UT1GameInstance::UT1GameInstance() 
{

	FString PlayerDataPath = TEXT("/Game/Data/PlayerData.PlayerData");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_PLAYER_DT(*PlayerDataPath);

	T1CHECK(DT_PLAYER_DT.Succeeded());
	T1PlayerTable = DT_PLAYER_DT.Object;
	T1CHECK(T1PlayerTable->RowMap.Num() > 0);

}
void UT1GameInstance::Init()
{
	Super::Init();
	T1LOG(Warning, TEXT("DropExp of Level 20 T1Player : %d"), GetT1PlayerData(20)->DropExp);
}

FT1PlayerData* UT1GameInstance::GetT1PlayerData(int Level)
{
	return T1PlayerTable->FindRow<FT1PlayerData>(*FString::FromInt(Level), TEXT(""));
}




