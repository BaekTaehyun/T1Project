// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GameObject/Define/GsGameObjectDefine.h"
#include "GameObject/Skill/Data/GsSkillNotifyCollisionData.h"
#include "GsSkillCollisionComponent.generated.h"

USTRUCT(BlueprintType)
struct FGsSkillCollisionCircle
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float Radius;
};

USTRUCT(BlueprintType)
struct FGsSkillCollisionFan
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float Radius;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float DegAngle;
};

USTRUCT(BlueprintType)
struct FGsSkillCollisionConcave
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<FVector2D> ListLine;
};

/**
* 스킬 충돌 판정 컴퍼넌트
* 이 컴퍼넌트의 TM이 실제 Offset역활을 하며(FTransform)
* Set() 함수를 통해 정보를 설정한뒤 InSideCheck() 함수로 판정 처리를 진행
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMESERVICE_API UGsSkillCollisionComponent : public USceneComponent
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere)
	EGsSkillCollisionType Type;

	UPROPERTY(EditAnywhere)
	FGsSkillCollisionCircle Circle;
	UPROPERTY(EditAnywhere)
	FGsSkillCollisionFan Fan;
	UPROPERTY(EditAnywhere)
	FGsSkillCollisionConcave Concave;

public:
	UPROPERTY(EditAnywhere)
	bool UseDebugLine = false;

public:	
	// Sets default values for this component's properties
	UGsSkillCollisionComponent();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

public:
	void Set(const FGsSkillNotifyCollisionData* Data);
	//임의의 좌표로 설정할 경우(ex 타겟팅)
	void Set(FTransform Tm, const FGsSkillNotifyCollisionData* Data);

protected:
	void SetCircle(const FGsSkillNotifyCollisionData* Data);
	void SetFan(const FGsSkillNotifyCollisionData* Data);
	void SetConcave(const FGsSkillNotifyCollisionData* Data);

public:
	bool InSideCheck(FVector Point);

protected:
	bool InSideCircle(FVector Point);
	bool InSideFan(FVector Point);
	bool InSideConcave(FVector Point);
	
	bool InSideCircle(FVector2D Point);
	bool InSideFan(FVector2D Point);
	bool InSideConcave(FVector2D Point);

public:
	UFUNCTION(BlueprintCallable, meta = (Tooltip = ""), Category = "SkillCollision")
	void TestCollision(AActor* ActorClass);

#if WITH_EDITOR
private:
	void DrawCollision();
	void DrawCircle();
	void DrawConcave();
	void DrawFan();
#endif
};
