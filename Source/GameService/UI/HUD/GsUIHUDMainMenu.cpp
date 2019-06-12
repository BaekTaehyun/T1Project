// Fill out your copyright notice in the Description page of Project Settings.


#include "GsUIHUDMainMenu.h"
#include "Kismet/GameplayStatics.h"
#include "GSGameInstance.h"


void UGsUIHUDMainMenu::GotoLobby()
{
	UGameplayStatics::OpenLevel((UObject*)GetGameInstance(), FName(TEXT("Begin")), true, TEXT("?game=/Game/Blueprint/GameMode/BP_GameModeLobby.BP_GameModeLobby_C"));
}