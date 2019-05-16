// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObject/Define/GsGameObjectDefine.h"
#include "GsGameObjectBase.generated.h"

/**
 * 모든 스폰 관련 오브젝트들의 base클래스
 * 언리얼엔진에서 제공하는 로직을 제외한 오브젝트에 대한 클라이언트 로직 처리 담당 
 */
UCLASS(BlueprintType)
class GAMESERVICE_API UGsGameObjectBase : public UObject
{
	GENERATED_BODY()
	
public:
	virtual ~UGsGameObjectBase();

	virtual void Initialize();
	virtual void DeInitialize();
    virtual void Update(float Delta);

    virtual AActor* Spawn(UClass* Instance, UWorld* World, const FVector& Position, const FRotator& Dir);
    //충돌 이벤트
    virtual void OnHit(UGsGameObjectBase* Target);

//프로퍼티
public:
    //GameObject 타입값 얻기
    UFUNCTION(BlueprintCallable, Category = "GameObject")
    virtual EGsGameObjectType GetObjectType() const;
	//타입 조합값 얻기
	virtual uint32 GetObjectTypeMask() const;

	//기본적인 Actor클래스 얻기
    UFUNCTION(BlueprintCallable, Category = "GameObject")
	virtual AActor* GetActor() const;

    //BP에서 간략한 접근을 위해 기본 TM정보 제공
    UFUNCTION(BlueprintCallable, Category = "GameObject")
    virtual FVector GetLocation() const;
    UFUNCTION(BlueprintCallable, Category = "GameObject")
    virtual FRotator GetRotation() const;
    UFUNCTION(BlueprintCallable, Category = "GameObject")
    virtual FVector GetScale() const;
    //

protected:
	//스폰 완료시 호출
	virtual void ActorSpawned(AActor* Spawn);

//맴버 변수
protected:
	uint8 ObjectType;
};
