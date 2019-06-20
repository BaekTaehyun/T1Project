// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObject/Define/GsGameObjectDefine.h"
#include "GsSkillNotifyCollisionData.generated.h"

/**
 * 충돌 실행 Notify 데이터
 * 주의 : 데이터 입력 방식에 대한 R&D가 필요
 * 현재는 FGsSkillNotifyCollisionData 구조체에 모든 정보를 모아 놓음..
 * (FGsSkillNotifyCollisionData 하위에 정의된 구조체 사용안하고 있음)
 */
USTRUCT(BlueprintType)
struct GAMESERVICE_API FGsSkillNotifyCollisionData
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKillNotify")
	int ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKillNotify")
	EGsSkillCollisionType Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKillNotify")
	FTransform Tm;

	//---------------------------------------------------------------------
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKillNotify")
	float Radius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKillNotify")
	float DegAngle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKillNotify")
	TArray<FVector2D> ListPoint;
	//
	//---------------------------------------------------------------------
};

/**
* ESkillCollisionType::Circle
*/
USTRUCT(BlueprintType)
struct GAMESERVICE_API FGsSkillNotifyCircleCollisionData //: public FGsSkillNotifyCollisionData
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKillNotify")
	float Radius;
};

/**
* ESkillCollisionType::Fan
*/
USTRUCT(BlueprintType)
struct GAMESERVICE_API FGsSkillNotifyFanCollisionData //: public FGsSkillNotifyCircleCollisionData
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKillNotify")
	float DegAngle;
};

/**
* ESkillCollisionType::Concave
*/
USTRUCT(BlueprintType)
struct GAMESERVICE_API FGsSkillNotifyConcaveCollisionData //: public FGsSkillNotifyCollisionData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SKillNotify")
	TArray<FVector2D> ListPoint;
};