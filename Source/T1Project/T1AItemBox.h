// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "T1Project.h"
#include "GameFramework/Actor.h"
#include "T1AItemBox.generated.h"
//#include "Components/PrimitiveComponent.h"
class UBoxComponent;

UCLASS()
class T1PROJECT_API AT1AItemBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AT1AItemBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, Category = Box)
	UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere, Category = Box)
	UStaticMeshComponent* Box;

	UPROPERTY(VisibleAnywhere, Category = Effect)
	UParticleSystemComponent* Effect;

	UPROPERTY(EditInstanceOnly, Category = BOX)
	TSubclassOf<class AT1AWeapon> WeaponItemClass;

private:
	//PrimitiveComponent.h 헤더파일의 FComponentBeginOverlapSignature는 멀티캐스트 다이내믹 델리게이트임을 확인가능하다
	
	UFUNCTION()
	void OnCharacterOverlap(UPrimitiveComponent* OverlappedComponent, AActor*
			OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEffectFinish(class UParticleSystemComponent* pSystem);
	
};
