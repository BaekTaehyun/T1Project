// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsGameObjectBase.h"
#include "GsGameObjectProjectile.generated.h"

/**
 * 발사체 클래스
 */
UCLASS()
class GAMESERVICE_API UGsGameObjectProjectile : public UGsGameObjectBase
{
	GENERATED_BODY()
	
public:
	virtual void Initialize() override;
	virtual void Finalize() override;

    virtual EGsGameObjectType GetObjectType() const override;
	virtual AActor* GetActor() const override;

protected:
	virtual void ActorSpawned(AActor* Spawn) override;

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, Category = GameObject, Meta = (AllowPrivateAccess = true))
	class AGsProjectileActor* Actor;
};
