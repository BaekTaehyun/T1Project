// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsGameObjectDynamic.h"
#include "GsGameObjectNonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class GAMESERVICE_API UGsGameObjectNonPlayer : public UGsGameObjectDynamic
{
	GENERATED_BODY()
	
public:
	virtual ~UGsGameObjectNonPlayer();

	virtual void Initialize() override;
	virtual void Finalize() override;

public:
    virtual EGsGameObjectType GetObjectType() const override;
	virtual AActor* GetActor() const override;
    UFUNCTION(BlueprintCallable, Category = "GameObject")
	virtual AGsNpcPawn* GetNpcPawn() const;

public:
	virtual void OnHit(UGsGameObjectBase* Target) override;

public:
	virtual void ActorSpawned(AActor* Spawn) override;

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, Category = GameObject, Meta = (AllowPrivateAccess = true))
	AGsNpcPawn* Actor;
};
