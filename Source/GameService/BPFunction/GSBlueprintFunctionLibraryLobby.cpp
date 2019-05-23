// Fill out your copyright notice in the Description page of Project Settings.


#include "GsBlueprintFunctionLibraryLobby.h"
#include "GameService.h"
#include "Util/GsText.h"
#include "../Message/GsMessageManager.h"


void UGsBlueprintFunctionLibraryLobby::SendEventGSLobby(EGS_LOBBY_Enum inMessage)
{
	GSLOG(Warning, *EnumToString(EGS_LOBBY_Enum, inMessage));

	// FIX: ±¸Á¶È­
	switch (inMessage)
	{
	case EGS_LOBBY_Enum::GS_LOBBY_END_INTRO:
	{
		GMessage()->GetStage().SendMessage(MessageLobby::Stage::INTRO_COMPLETE);
	}
	break;
	case EGS_LOBBY_Enum::GS_LOBBY_BACKTO_SERVER_SELECT:
	{
		GMessage()->GetStage().SendMessage(MessageLobby::Stage::BACKTO_SERVER_SELECT);
	}
	break;
	case EGS_LOBBY_Enum::GS_LOBBY_ENTER_INGAME:
	{
		GMessage()->GetStage().SendMessage(MessageLobby::Stage::ENTER_INGAME);
	}
	break;
	case EGS_LOBBY_Enum::GS_LOBBY_INGAME_LOAD_COMPLETE:
	{
		// FIX:
		GMessage()->GetStage().SendMessage(MessageLobby::Stage::INGAME_LOAD_COMPLETE);
	}
	break;
	}
}