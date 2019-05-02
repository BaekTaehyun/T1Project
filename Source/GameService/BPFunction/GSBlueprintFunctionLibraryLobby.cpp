// Fill out your copyright notice in the Description page of Project Settings.


#include "GSBlueprintFunctionLibraryLobby.h"
#include "GameService.h"
#include "Util/Text.h"


void UGSBlueprintFunctionLibraryLobby::SendEventGSLobby(EGS_LOBBY_Enum inMessage)
{
	GSLOG(Warning, *EnumToString(EGS_LOBBY_Enum, inMessage));

	
}