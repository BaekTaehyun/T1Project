// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "GameObject/Define/GsGameObjectDefine.h"
#include "Class/GsManager.h"
#include "Class/GsSingleton.h"
#include "GsGameObjectManager.generated.h"

class UGsGameObjectBase;

/**
 * 임시 스폰 담당 클래스
 * 액터 디스폰 델리게이트 연결 이슈로 UObject형으로 처리(다른 방법이 있는지 확인)
 */
UCLASS()
class GAMESERVICE_API AGsGameObjectManager : 
	public AActor,
	public TGsSingleton<AGsGameObjectManager>,
	public IGsManager
{
	GENERATED_BODY()
	
public:

	AGsGameObjectManager();
	virtual ~AGsGameObjectManager();

	virtual void Initialize() override;
	virtual void Finalize() override;
	virtual void Tick(float DeltaTime) override;

	UGsGameObjectBase* FindObject(AActor* Actor, EGsGameObjectType Type = EGsGameObjectType::Base);
    UGsGameObjectBase* FindObject(EGsGameObjectType Type);
	TArray<UGsGameObjectBase*> FindObjects(EGsGameObjectType Type);

	//[Todo] 임시 오브젝트 스폰 처리 메서드들..
	//추후 특정 스폰 데이터형을 입력받아 처리하게 변경 필요
	UGsGameObjectBase* SpawnPlayer(UClass* Uclass, const FVector& Pos, const FRotator& Rot);
	UGsGameObjectBase* SpawnNpc(UClass* Uclass, const FVector& Pos, const FRotator& Rot);
	UGsGameObjectBase* SpawnProjectile(UClass* Uclass, const FVector& Pos, const FRotator& Rot);
	UGsGameObjectBase* SpawnVehicle(UClass* Uclass, const FVector& Pos, const FRotator& Rot);

	//이 메서드는 아직 GameObject타입별 후처리에대한 처리가 지원되지 않음
	template<class tGameobject>
	tGameobject* SpawnObject(UClass* Uclass, const FVector& Pos, const FRotator& Rot, bool CalcOnGround = false);

	void DespawnObject(UGsGameObjectBase* Despawn);

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //
protected:	
	void UpdateAddGameObject();
	void UpdateRemoveGameObject();
	FVector CalcOnGround(UClass* Uclass, const FVector& Pos);

	UFUNCTION()
	void CallbackCompHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void CallbackActorDeSpawn(AActor* Despawn);

private:
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

template<class tGameobject>
tGameobject* AGsGameObjectManager::SpawnObject(UClass* Uclass, const FVector& Pos, const FRotator& Rot, bool IsOnGround)
{
	if (auto object = NewObject<tGameobject>())
	{
		object->Initialize();
		FVector spawnPos = Pos;

		if (IsOnGround)
		{
			spawnPos = CalcOnGround(Uclass, Pos);
		}

		if (auto actor = TGsSpawn::BPClass(GetWorld(), Uclass, spawnPos, Rot))
		{
			AddSpawns.Emplace(object);
			object->ActorSpawned(actor);
			actor->OnDestroyed.AddDynamic(this, &AGsGameObjectManager::CallbackActorDeSpawn);
		}

		return object;
	}

	return NULL;
}

typedef TGsSingleton<AGsGameObjectManager>	AGsGameObjectSingle;
AGsGameObjectManager* AGsGameObjectSingle::Instance = NULL;
#define GGameObj() AGsGameObjectSingle::Instance

