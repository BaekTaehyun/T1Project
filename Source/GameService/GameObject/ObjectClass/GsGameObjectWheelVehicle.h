// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObject/ObjectClass/GsGameObjectBase.h"
#include "GsGameObjectWheelVehicle.generated.h"

class UGsGameObjectPlayer;
class UGsInputBindingVehicle;

/**
 * 탈것 클래스
 * 탑승자는 UGsGameObjectPlayer 타입으로 임의 제한둠 @see : TArray<UGsGameObjectPlayer*> ListPassengers;
 */
UCLASS()
class GAMESERVICE_API UGsGameObjectWheelVehicle : public UGsGameObjectBase
{
	GENERATED_BODY()
	
public:
	virtual ~UGsGameObjectWheelVehicle();

	virtual void Initialize() override;
	virtual void Finalize() override;

public:
	virtual EGsGameObjectType GetObjectType() const override;
	virtual AActor* GetActor() const override;
	UFUNCTION(BlueprintCallable, Category = "GameObject")
	virtual AGsWheelVehicle* GetWhellVehicle() const;
	TArray<UGsGameObjectPlayer*> GetPassengers() const;
	void SetPassenger(UGsGameObjectPlayer* Passenger);
	void RemovePassenger(UGsGameObjectPlayer* Passenger);

public:
	virtual void ActorSpawned(AActor* Spawn) override;

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, Category = GameObject, Meta = (AllowPrivateAccess = true))
	AGsWheelVehicle* Actor;
	UPROPERTY(Transient, VisibleInstanceOnly, Category = GameObject, Meta = (AllowPrivateAccess = true))
	UGsInputBindingVehicle* InputBinder;

	//탑승자 정보
	TArray<UGsGameObjectPlayer*> ListPassengers;
};
