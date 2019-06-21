// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "GameService.h"
#include "GameObject/Define/GsGameObjectDefine.h"
#include "GsCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API UGsCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec, Category = ExecFunctions)
	void TeleportPlayer(FString in_tag);

#pragma todo("by : ��� ����Ʈ���� ���ڰ��� �����Ҽ� �ִ��� Ȯ��")
	UFUNCTION(Exec, BlueprintCallable, Category = "Cheat ManagerEx")
	void SpawnGameObject(EGsGameObjectType Type, TSubclassOf<AActor> ActorClass, int SpawnCount = 1, const FVector& StartPos = FVector::ZeroVector);
	
	UFUNCTION(Exec, Category = ExecFunctions)
		void AddPotion(FString in_tag);
};
