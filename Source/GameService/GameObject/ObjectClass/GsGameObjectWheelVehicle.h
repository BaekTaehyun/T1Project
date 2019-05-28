// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObject/ObjectClass/GsGameObjectBase.h"
#include "GsGameObjectWheelVehicle.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API UGsGameObjectWheelVehicle : public UGsGameObjectBase
{
	GENERATED_BODY()
	
public:
	virtual void Initialize() override;
	virtual void Finalize() override;

	virtual EGsGameObjectType GetObjectType() const override;
	virtual AActor* GetActor() const override;
	UFUNCTION(BlueprintCallable, Category = "GameObject")
	virtual AGsWheelVehicle* GetWhellVehicle() const;

public:
	virtual void ActorSpawned(AActor* Spawn) override;

	//[임시] 탑승자 정보 처리
	void AttachGameObject(UGsGameObjectBase* Go, FName SocketName);

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, Category = GameObject, Meta = (AllowPrivateAccess = true))
	AGsWheelVehicle* Actor;
};
