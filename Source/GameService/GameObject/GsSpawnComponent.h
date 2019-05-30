// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "GameObject/Define/GsGameObjectDefine.h"
#include "Class/GsManager.h"
#include "Class/GsSingleton.h"
#include "Class/GsSpawn.h"
#include "GsSpawnComponent.generated.h"

class UGsGameObjectBase;

UCLASS()
class GAMESERVICE_API UGsSpawnComponent : 
	public UActorComponent,
	public TGsSingleton<UGsSpawnComponent>
{
public:
	GENERATED_UCLASS_BODY()
	virtual ~UGsSpawnComponent();

	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void UninitializeComponent() override;

    UGsGameObjectBase* FindObject(EGsGameObjectType Type);
	UGsGameObjectBase* FindObject(class AActor*, EGsGameObjectType type = EGsGameObjectType::Base);
	TArray<UGsGameObjectBase*> FindObjectArray(EGsGameObjectType Type);
		
	//이 메서드는 아직 GameObject타입별 후처리에대한 처리가 지원되지 않음
	template<class tGameobject>
	tGameobject* SpawnObject(UClass* Uclass, const FVector& Pos, const FRotator& Rot, bool IsOnGround = false)
	{
		if (auto object = NewObject<tGameobject>())
		{
			return SpawnObjectInternal(object, Uclass, Pos, Rot, IsOnGround);
		}
		return NULL;
	}
	void DespawnObject(UGsGameObjectBase* Despawn);

protected:
	template<class tGameobject>
	tGameobject* SpawnObjectInternal(tGameobject* Instance, UClass* Uclass, const FVector& Pos, const FRotator& Rot, bool IsOnGround = false)
	{
		Instance->Initialize();
		FVector spawnPos = Pos;
		if (IsOnGround)
		{
			spawnPos = CalcOnGround(Uclass, Pos);
		}
		if (auto actor = TGsSpawn::BPClass(GetWorld(), Uclass, spawnPos, Rot))
		{
			AddSpawns.Emplace(Instance);
			Instance->ActorSpawned(actor);
			actor->OnDestroyed.AddDynamic(this, &UGsSpawnComponent::CallbackActorDeSpawn);
		}
		return Instance;
	}

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


typedef TGsSingleton<UGsSpawnComponent>	UGsSpawnerSingle;
UGsSpawnComponent* UGsSpawnerSingle::Instance = NULL;
#define GSpawner() UGsSpawnerSingle::Instance



