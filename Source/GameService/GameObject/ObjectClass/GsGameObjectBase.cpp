// Fill out your copyright notice in the Description page of Project Settings.

#include "GsGameObjectBase.h"
#include "Runtime/Engine/Classes/Engine/World.h"

UGsGameObjectBase::~UGsGameObjectBase()
{
	UE_LOG(LogTemp, Warning, TEXT("~UGsGameObjectBase (%s)"), *GetName());
}

//프로퍼티
EGsGameObjectType	UGsGameObjectBase::GetObjectType() const      { return EGsGameObjectType::Base; }
uint32				UGsGameObjectBase::GetObjectTypeMask() const  { return ObjectType; }
AActor*				UGsGameObjectBase::GetActor() const           { return NULL; }
//

FVector UGsGameObjectBase::GetLocation() const
{
    return (GetActor()) ? GetActor()->GetActorLocation() : FVector::ZeroVector;
}

FRotator UGsGameObjectBase::GetRotation() const
{
    return  (GetActor()) ? GetActor()->GetActorRotation() : FRotator::ZeroRotator;
}

FVector UGsGameObjectBase::GetScale() const
{
    return (GetActor()) ? GetActor()->GetActorScale() : FVector::OneVector;
}

void UGsGameObjectBase::Initialize()
{
    SET_FLAG_TYPE(ObjectType, UGsGameObjectBase::GetObjectType());
}

void UGsGameObjectBase::Finalize()
{
	//액터가 존재한다면 소멸
	if (AActor* actor = GetActor())
	{
		actor->GetWorld()->DestroyActor(actor);
	}
}

void UGsGameObjectBase::Update(float Delta)
{
}

void UGsGameObjectBase::OnHit(UGsGameObjectBase* Target)
{
}

AActor* UGsGameObjectBase::Spawn(UClass* Instance, UWorld* World, const FVector& Position, const FRotator& Dir)
{
	FActorSpawnParameters SpawnInfo;
	//스폰완료 델리게이트 연결
	//이 델리게이트는 브로드 캐스팅이다. 즉 다른 액터 스폰시에도 응답
	//World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateRaw(this, &UGsGameObjectBase::ActorSpawned));
	AActor* actor = World->SpawnActor(Instance, &Position, &Dir, SpawnInfo);
	ActorSpawned(actor);

	return actor;
}

void UGsGameObjectBase::ActorSpawned(AActor* Spawn)
{
	if (Spawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s Actor Spawn Complete"), *Spawn->GetName());
	}
}