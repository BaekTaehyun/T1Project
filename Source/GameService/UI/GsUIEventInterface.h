// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object.h"
#include "UObject/NoExportTypes.h"
#include "GsUIEventInterface.generated.h"

/**
 * UI �������Ʈ �̺�Ʈ ȣ���� ���� �������̽�
 * �������̽� �����ڷ�
 * https://wiki.unrealengine.com/Interfaces_in_C++
 * https://wiki.unrealengine.com/How_To_Make_C%2B%2B_Interfaces_Implementable_By_Blueprints%28Tutorial%29
 */

UINTERFACE(BlueprintType)
class GAMESERVICE_API UGsUIEventInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()

};

class IGsUIEventInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	/** �ٿ�ε� */
	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GsManaged")
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UILobbyMsg")
	void UIEvent_ChangeDownloadRate(float InRate);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UILobbyMsg")
	void UIEvent_AccountLoginComplete();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UILobbyMsg")
	void UIEvent_ServerSelectComplete(int32 InServerID);
};
