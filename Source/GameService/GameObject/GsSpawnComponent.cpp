#include "GsSpawnComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameService.h"

#include "GameObject/ObjectClass/GsGameObjectLocal.h"
#include "GameObject/ObjectClass/GsGameObjectNonPlayer.h"
#include "GameObject/ObjectClass/GsGameObjectProjectile.h"
#include "GameObject/ObjectClass/GsGameObjectWheelVehicle.h"
#include "../Class/GsSpawn.h"

UGsSpawnComponent::UGsSpawnComponent(const class FObjectInitializer &OBJ) : Super(OBJ)
{
	PrimaryComponentTick.bCanEverTick = true;

	//InitializeComponent 함수를 호출하고 싶을때 다음과 같은 플레그가 설정되어야 호출됩니다.
	bWantsInitializeComponent = true;

	
}

UGsSpawnComponent::~UGsSpawnComponent()
{
	GSLOG(Warning, TEXT("~UGsSpawnComponent()"));
}

void UGsSpawnComponent::InitializeComponent()
{
	Super::InitializeComponent();
	UGsSpawnerSingle::InitInstance(this);

	Spawns.Empty();
	AddSpawns.Empty();
	RemoveSpawns.Empty();
	TypeSpawns.Empty(EGsGameObjectTypeALLCount);

	//타입별 빈공간 생성
	for (auto el : EGsGameObjectTypeALL)
	{
		TypeSpawns.Emplace(el);
	}
	
	PrimaryComponentTick.SetTickFunctionEnable(true);
}

void UGsSpawnComponent::UninitializeComponent()
{
	for (auto el : Spawns)
	{
		el->Finalize();
	}

	TypeSpawns.Empty();
	AddSpawns.Empty();
	RemoveSpawns.Empty();
	Spawns.Empty();

	PrimaryComponentTick.SetTickFunctionEnable(false);

	UGsSpawnerSingle::RemoveInstance();
}

void UGsSpawnComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//대상 리스트 제거
	UpdateRemoveGameObject();
	//대상 추가
	UpdateAddGameObject();

	//위 함수가 제거되면 기존 Update로직으로 이동
	//스폰 오브젝트 업데이트
	for (auto el : Spawns)
	{
		el->Update(DeltaTime);
	}
}


UGsGameObjectBase* UGsSpawnComponent::FindObject(AActor* Actor, EGsGameObjectType Type)
{
	if (Type == EGsGameObjectType::Base)
	{
		if (auto findobj = Spawns.FindByPredicate([=](UGsGameObjectBase* el)
		{
			AActor* a = el->GetActor();
			return NULL != a && a == Actor;
		}))
		{
			return *findobj;
		}
	}
	else
	{
		auto list = TypeSpawns[Type];
		if (auto findobj = list.FindByPredicate([=](UGsGameObjectBase* el)
		{
			AActor* a = el->GetActor();
			return NULL != a && a == Actor;
		}))
		{
			return *findobj;
		}
	}

	return NULL;
}

UGsGameObjectBase* UGsSpawnComponent::FindObject(EGsGameObjectType Type)
{
    return TypeSpawns[Type].Num() > 0 ? TypeSpawns[Type].Top() : NULL;
}

TArray<UGsGameObjectBase*> UGsSpawnComponent::FindObjects(EGsGameObjectType Type)
{
	return TypeSpawns[Type];
}

template <>
UGsGameObjectProjectile* UGsSpawnComponent::SpawnObject(UClass* Uclass, const FVector& Pos, const FRotator& Rot, bool IsOnGround)
{
	if (auto projectile = NewObject<UGsGameObjectProjectile>())
	{
		SpawnObjectInternal(projectile, Uclass, Pos, Rot, IsOnGround);

		if (UPrimitiveComponent* collider = projectile->
			GetActor()->FindComponentByClass<UPrimitiveComponent>())
		{
			collider->OnComponentHit.AddDynamic(this, &UGsSpawnComponent::CallbackCompHit);
		}
		return projectile;
	}
	return NULL;
}

void UGsSpawnComponent::DespawnObject(UGsGameObjectBase* Despawn)
{
	if (Spawns.Contains(Despawn))
	{
		//액터 소멸
		GetWorld()->DestroyActor(Despawn->GetActor());

		//액터 소멸시 일단 관리대상 에서 제거
		RemoveSpawns.Emplace(Despawn);
	}
}

void UGsSpawnComponent::UpdateAddGameObject()
{
	//대상 추가
	if (0 < AddSpawns.Num())
	{
		for (auto el : AddSpawns)
		{
			uint8 key = el->GetObjectTypeMask();
			for (auto el2 : EGsGameObjectTypeALL)
			{
				if (CHECK_FLAG_TYPE(key, el2))
				{
					TypeSpawns[el2].Emplace(el);
				}
			}
			Spawns.Emplace(el);
		}
		AddSpawns.Empty();
	}
}


void UGsSpawnComponent::UpdateRemoveGameObject()
{
	if (0 < RemoveSpawns.Num())
	{
		for (auto el : RemoveSpawns)
		{
			uint8 key = el->GetObjectTypeMask();
			for (auto el2 : EGsGameObjectTypeALL)
			{
				if (CHECK_FLAG_TYPE(key, el2))
				{
					TypeSpawns[el2].RemoveSwap(el);
				}
			}

			Spawns.RemoveSwap(el);
		}
		RemoveSpawns.Empty();
	}
}


FVector UGsSpawnComponent::CalcOnGround(UClass* Uclass, const FVector& Pos)
{
	//[Todo] Class UCharacterMovementComponent
			//void UCharacterMovementComponent::FindFloor(const FVector& CapsuleLocation,
			// FFindFloorResult& OutFloorResult, bool bCanUseCachedLocation, const FHitResult* DownwardSweepResult) const
			//내부 구현을 참고 하여 개선한다.
	FVector rayOri = Pos + FVector(0.f, 0.f, 1000.f);
	FVector rayDes = rayOri + FVector(0.f, 0.f, -1.f)  * 1200.f;

	FHitResult HitResult;
	TArray<AActor*> actorsToIgnore;
	//생성도 되기전에 얻어오는게 맞는지 모르것음...
	if (AActor* castActor = Uclass->GetDefaultObject<AActor>())
	{
		UKismetSystemLibrary::LineTraceSingle(GetWorld(), rayOri, rayDes, UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
			false, actorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true, FLinearColor::Green, FLinearColor::Red);
		if (HitResult.bBlockingHit)
		{
			FVector location = HitResult.Location + FVector(0.f, 0.f, 0.5f);
			if (UCapsuleComponent* Capshule = castActor->FindComponentByClass<UCapsuleComponent>())
			{
				location.Z += Capshule->GetUnscaledCapsuleHalfHeight();
			}

			return location;
		}
	}
	return Pos;
}
void UGsSpawnComponent::CallbackCompHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{
		auto hitObj = FindObject(HitComponent->GetOwner(), EGsGameObjectType::Projectile);
		//피격 알림
		if (auto findObj = FindObject(OtherActor, EGsGameObjectType::NonPlayer))
		{
			findObj->OnHit(hitObj);
		}

		if (hitObj)
		{
			//충돌 객체 성향에 따라 Destory여부 설정하기
			//현재는 무조건 삭제
			DespawnObject(hitObj);
		}
	}
}


void UGsSpawnComponent::CallbackActorDeSpawn(AActor* Despawn)
{
	//관리 대상인가 찾음
	if (auto findObj = FindObject(Despawn))
	{
		RemoveSpawns.Emplace(findObj);
	}
}

