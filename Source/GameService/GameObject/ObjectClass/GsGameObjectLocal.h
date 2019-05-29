// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsGameObjectPlayer.h"
#include "GsGameObjectLocal.generated.h"

/**
 * 내플레이어
 */
UCLASS()
class GAMESERVICE_API UGsGameObjectLocal : public UGsGameObjectPlayer
{
	GENERATED_BODY()
	
public:
	virtual void Initialize() override;
	virtual void Finalize() override;
    virtual void Update(float Delta) override;

public:
    virtual EGsGameObjectType GetObjectType() const override;
	virtual AActor* GetActor() const override;
    UFUNCTION(BlueprintCallable, Category = "GameObject")
	AGsLocalCharacter* GetLocalCharacter() const;

	virtual class FGsFSMManager* GetBaseFSM() const override;
	virtual class FGsFSMManager* GetUpperFSM() const override;
	virtual class FGsSkillBase* GetSkill()  const override;
	virtual class FGsPartsBase* GetParts() const override;
	class FGsGameObjectEventBase* GetEvent() const;

public:
	virtual void ActorSpawned(AActor* Spawn) override;

protected:
	virtual void RegistEvent() override;

private:
	UPROPERTY(Transient, VisibleInstanceOnly, Category = GameObject, Meta = (AllowPrivateAccess = true))
 	AGsLocalCharacter* Actor;

	FGsGameObjectEventBase* Event;
};
