// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GSBluePrintEnumLobby.h"
#include "GSBlueprintFunctionLibraryLobby.generated.h"


/**
 * �κ� �������� ��� ����Ʈ���� C++�� ����� �ʿ��Ҷ� ���ʿ��� �����ϵ��� �Ѵ�.
   UI ���η��� ó������ �ƴ� �̺�Ʈ(ó����)
 */
UCLASS()
class GAMESERVICE_API UGSBlueprintFunctionLibraryLobby : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category="GameServiceLobby")
	static void SendEventGSLobby(EGS_LOBBY_Enum inMessage);
};
