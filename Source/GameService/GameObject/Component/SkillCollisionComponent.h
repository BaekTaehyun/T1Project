// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GameObject/Define/GsGameObjectDefine.h"
#include "GameObject/Skill/Data/GsSkillNotifyCollisionData.h"
#include "SkillCollisionComponent.generated.h"

USTRUCT(BlueprintType)
struct FSkillCollisionCircle
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float Radius;
};

USTRUCT(BlueprintType)
struct FSkillCollisionFan
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float Radius;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float DegAngle;
};

USTRUCT(BlueprintType)
struct FSkillCollisionConcave
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<FVector2D> ListLine;
};

/**
* ��ų �浹 ���� ���۳�Ʈ
* �� ���۳�Ʈ�� TM�� ���� Offset��Ȱ�� �ϸ�(FTransform)
* Set() �Լ��� ���� ������ �����ѵ� InSideCheck() �Լ��� ���� ó���� ����
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMESERVICE_API USkillCollisionComponent : public USceneComponent
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere)
	ESkillCollisionType Type;

	UPROPERTY(EditAnywhere)
	FSkillCollisionCircle Circle;
	UPROPERTY(EditAnywhere)
	FSkillCollisionFan Fan;
	UPROPERTY(EditAnywhere)
	FSkillCollisionConcave Concave;

public:
	UPROPERTY(EditAnywhere)
	bool UseDebugLine = false;

public:	
	// Sets default values for this component's properties
	USkillCollisionComponent();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

public:
	void Set(const FGsSkillNotifyCollisionData* Data);
	//������ ��ǥ�� ������ ���(ex Ÿ����)
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
