// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GSBluePrintEnumLobby.h"
#include "GSBlueprintFunctionLibraryLobby.generated.h"


/**
 * 로비 컨텐츠중 블루 프린트에서 C++로 통신이 필요할때 이쪽에서 구현하도록 한다.
   UI 내부로직 처리용이 아닌 이벤트(처리용)
 */
UCLASS()
class GAMESERVICE_API UGSBlueprintFunctionLibraryLobby : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category="GameServiceLobby")
	static void SendEventGSLobby(EGS_LOBBY_Enum inMessage);
};
