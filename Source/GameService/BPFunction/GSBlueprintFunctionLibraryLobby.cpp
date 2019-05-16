// Fill out your copyright notice in the Description page of Project Settings.


#include "GsBlueprintFunctionLibraryLobby.h"
#include "GameService.h"
#include "Util/GsText.h"


void UGsBlueprintFunctionLibraryLobby::SendEventGSLobby(EGS_LOBBY_Enum inMessage)
{
	GSLOG(Warning, *EnumToString(EGS_LOBBY_Enum, inMessage));
}