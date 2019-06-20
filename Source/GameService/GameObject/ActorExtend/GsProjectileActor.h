// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GsProjectileActor.generated.h"

/**
* 발사체 처리 AActor 클래스
* 현재는 (StaticMesh or SkeletalMesh), ProjectileMovement 등의 컴퍼넌트를 내부 로직으로 생성하고 있지 않는다(BP에서 진행)
* 추후 관련 내용 협의 필요
*/
UCLASS()
class GAMESERVICE_API AGsProjectileActor : public AActor
{
	GENERATED_BODY()
	
	//UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//UProjectileMovementComponent* ProjectileMovementComponent;
	
public:	
	// Sets default values for this actor's properties
	AGsProjectileActor();
	virtual ~AGsProjectileActor();

	virtual void PostInitializeComponents() override;
    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
