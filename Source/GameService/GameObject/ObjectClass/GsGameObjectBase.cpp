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
UWorld*				UGsGameObjectBase::GetWorld() const			  { return GetActor()->GetWorld(); }
//

bool UGsGameObjectBase::IsObjectType(EGsGameObjectType Type)
{
	return CHECK_FLAG_TYPE(GetObjectTypeMask(), Type);
}

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

	RegistEvent();
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

void UGsGameObjectBase::ActorSpawned(AActor* Spawn)
{
	if (Spawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s Actor Spawn Complete"), *Spawn->GetName());
	}
}

void UGsGameObjectBase::RegistEvent()
{
}