// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "GameObject/Define/GsGameObjectDefine.h"
#include "Class/GsManager.h"
#include "Class/GsSingleton.h"
#include "GsObjectSpawner.generated.h"

/**
 * 임시 스폰 담당 클래스
 * 액터 디스폰 델리게이트 연결 이슈로 UObject형으로 처리(다른 방법이 있는지 확인)
 */
UCLASS()
class GAMESERVICE_API UGsObjectSpawner : 
	public UObject,
	public TGsSingleton<UGsObjectSpawner>,
	public IGsManager
{
	GENERATED_BODY()
	
public:

	virtual ~UGsObjectSpawner();

	virtual void Initialize() override;
	virtual void Finalize() override;
	virtual void Update() override;

	void SetWorld(class UWorld* World);

	class UGsGameObjectBase* FindObject(class AActor* Actor, EGsGameObjectType Type = EGsGameObjectType::Base);
    UGsGameObjectBase* FindObject(EGsGameObjectType Type);
	TArray<UGsGameObjectBase*> FindObjects(EGsGameObjectType Type);

	//[Todo] 임시 오브젝트 스폰 처리 메서드들..
	//추후 특정 스폰 데이터형을 입력받아 처리하게 변경 필요
	UGsGameObjectBase* SpawnPlayer(UClass* Uclass, const FVector& Pos, const FRotator& Rot);
	UGsGameObjectBase* SpawnNpc(UClass* Uclass, const FVector& Pos, const FRotator& Rot);
	UGsGameObjectBase* SpawnProjectile(UClass* Uclass, const FVector& Pos, const FRotator& Rot);

	void DespawnObject(UGsGameObjectBase* Despawn);
    //
protected:
	bool UpdateGameObject(float Delta);
	void UpdateAddGameObject();
	void UpdateRemoveGameObject();

	UFUNCTION()
	void CallbackCompHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void CallbackActorDeSpawn(AActor* Despawn);

private:
	//임의 틱관리 객체 생성
	//추후 Update에서 Delta값을 받는게 좋을것 같다.
	FDelegateHandle TickDelegate;

	UWorld* World;

	//액터 객체 관리
	//전체 대상 시리얼라이즈 포함
	UPROPERTY(Transient, VisibleInstanceOnly, Meta = (AllowPrivateAccess = true))
	TArray<UGsGameObjectBase*> Spawns;

	//빠른 검색정보
	TMap<EGsGameObjectType, TArray<UGsGameObjectBase*>> TypeSpawns;

	//추가/삭제 대상 관리
	TArray<UGsGameObjectBase*> AddSpawns;
	TArray<UGsGameObjectBase*> RemoveSpawns;
};

typedef TGsSingleton<UGsObjectSpawner>	UGsObjectSpawnerSingle;
UGsObjectSpawner* UGsObjectSpawnerSingle::Instance = NULL;
#define GSpawner() UGsObjectSpawnerSingle::Instance
