// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsGameObjectPlayer.h"
#include "GsGameObjectLocal.generated.h"

class FGsGameObjectEventBase;
class FGsSkillBase;
class UGsInputBindingLocalPlayer;

/**
 * 내플레이어
 */
UCLASS()
class GAMESERVICE_API UGsGameObjectLocal : public UGsGameObjectPlayer
{
	GENERATED_BODY()
	
public:
	virtual ~UGsGameObjectLocal();

	virtual void Initialize() override;
	virtual void Finalize() override;
    virtual void Update(float Delta) override;

public:
    virtual EGsGameObjectType GetObjectType() const override;
	virtual AActor* GetActor() const override;
    UFUNCTION(BlueprintCallable, Category = "GameObject")
	AGsLocalCharacter* GetLocalCharacter() const;

	virtual FGsFSMManager* GetBaseFSM() const override;
	virtual FGsFSMManager* GetUpperFSM() const override;
	virtual FGsSkillBase* GetSkill()  const override;
	virtual FGsPartsBase* GetParts() const override;
	FGsGameObjectEventBase* GetEvent() const;

public:
	virtual void ActorSpawned(AActor* Spawn) override;

protected:
	virtual void RegistEvent() override;

private:
	UPROPERTY(Transient, VisibleInstanceOnly, Category = GameObject, Meta = (AllowPrivateAccess = true))
 	AGsLocalCharacter* Actor;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = GameObject, Meta = (AllowPrivateAccess = true))
	UGsInputBindingLocalPlayer* InputBinder;

	FGsGameObjectEventBase* Event;
};
