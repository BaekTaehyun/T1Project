// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameObject/ObjectClass/GsGameObjectBase.h"
#include "GsGameObjectBlueprintLibrary.generated.h"

/**
 * GameObject 관련 BP 제공 유틸함수 모음
 */
UCLASS()
class GAMESERVICE_API UGsGameObjectBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	//로컬 캐릭터 스폰
	UFUNCTION(BlueprintCallable, meta = (Tooltip = ""), Category = "GameObject")
	static void SpawnPlayer(TSubclassOf<ACharacter> ActorClass, const FVector& StartPos, const FVector& Offset);
	//지형위에 스폰되는 함수 제공
	//ACharacter 클래스에는 UCharacterMovementComponent 가 제공해주고 있지만 APawn계열엔 없다.
	UFUNCTION(BlueprintCallable, meta = (Tooltip = ""), Category = "GameObject")
	static AActor* SpawnOnGround(TSubclassOf<AActor> ActorClass, const FVector& StartPos, const FVector& Offset);

	UFUNCTION(BlueprintCallable, meta = (Tooltip = ""), Category = "GameObject")
	static AActor* SpawnVehicle(TSubclassOf<AActor> ActorClass, const FVector& StartPos, const FVector& Offset);

	//GameObject 찾기
    UFUNCTION(BlueprintPure, Category = "GameObject", Meta = (WorldContext = "WorldContextObject"))
    static UGsGameObjectBase* FindGameObject(UObject* WorldContextObject, EGsGameObjectType ObjectType);
    //GameObject 찾기(복수)
    UFUNCTION(BlueprintPure, Category = "GameObject", Meta = (WorldContext = "WorldContextObject"))
    static TArray<UGsGameObjectBase*> FindGameObjects(UObject* WorldContextObject, EGsGameObjectType ObjectType);	
};
