// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObject/Define/GsGameObjectDefine.h"
#include "GsSkillNotifyCollisionData.generated.h"

/**
 * �浹 ���� Notify ������
 * ���� : ������ �Է� ��Ŀ� ���� R&D�� �ʿ�
 * ����� FGsSkillNotifyCollisionData ����ü�� ��� ������ ��� ����..
 * (FGsSkillNotifyCollisionData ������ ���ǵ� ����ü �����ϰ� ����)
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