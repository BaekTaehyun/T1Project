// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsGameObjectBase.h"
#include "GsGameObjectProjectile.generated.h"

class AGsProjectileActor;

/**
 * 발사체 클래스
 */
UCLASS()
class GAMESERVICE_API UGsGameObjectProjectile : public UGsGameObjectBase
{
	GENERATED_BODY()
	
public:
	virtual ~UGsGameObjectProjectile();

	virtual void Initialize() override;
	virtual void Finalize() override;

    virtual EGsGameObjectType GetObjectType() const override;
	virtual AActor* GetActor() const override;

public:
	virtual void ActorSpawned(AActor* Spawn) override;

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, Category = GameObject, Meta = (AllowPrivateAccess = true))
	AGsProjectileActor* Actor;
};
