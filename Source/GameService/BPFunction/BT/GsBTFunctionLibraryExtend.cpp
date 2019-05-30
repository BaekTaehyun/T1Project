// Fill out your copyright notice in the Description page of Project Settings.

#include "GsBTFunctionLibraryExtend.h"
#include "AIModule/Classes/BehaviorTree/BTNode.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Class.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_String.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Name.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "AIModule/Classes/BehaviorTree/Blackboard/BlackboardKeyType_Rotator.h"
#include "GameObject/ObjectClass/GsGameObjectBase.h"
#include "GameObject/GsSpawnComponent.h"


#define DEF_SETBLACKBOARDDATA(bc, name, p, v)       if(!p.##name.IsEmpty()) { bc->SetValueAs##name(*p.##name, v.##name); }
#define DEF_SETREGBLACKBOARDDATA(bc, name, p, v)    if (!p.##name.IsEmpty()) { int key = sizeof(UBlackboardKeyType_##name); \
                                                    if (v<UBlackboardKeyType_##name>.Contains(key)) \
                                                        { bc->SetValueAs##name(*p.##name, v<UBlackboardKeyType_##name>[key]); } }
#define DEF_CLEARBLACKBOARDDATA(bc, name, p)        if(!p.##name.IsEmpty()) { bc->ClearValue(*p.##name); }


template<class TDataClass> TMap<int, typename TDataClass::FDataType> UGsBTFunctionLibraryExtend::BlackboardRegister;
FGsBTBlackboardDataResult UGsBTFunctionLibraryExtend::BlackboardData;

void FGsBTBlackboardDataResult::Reset()
{
    Object  = NULL;
    Class   = NULL;
    Enum    = 0;
    Int     = 0; 
    Float   = 0.f;
    Bool    = false;
    Name    = TEXT("");
    Vector  = FVector::ZeroVector;
    Rotator = FRotator::ZeroRotator;
    String.Empty();
}

FGsBTBlackboardDataResult UGsBTFunctionLibraryExtend::GetRegistBlackboardData()
{
    FGsBTBlackboardDataResult result;
#if WITH_EDITOR
#endif
    return result;
}

UGsGameObjectBase* UGsBTFunctionLibraryExtend::GetBlackboardValueAsGameObject(UBTNode* NodeOwner, const FBlackboardKeySelector& Key)
{
    return Cast<UGsGameObjectBase>(Super::GetBlackboardValueAsObject(NodeOwner, Key));
}

UObject* UGsBTFunctionLibraryExtend::GetBlackboardValueAsObjectEx(UBTNode* NodeOwner, const FName& KeyName)
{
    UBlackboardComponent* BlackboardComp = GetOwnersBlackboard(NodeOwner);
    return BlackboardComp ? BlackboardComp->GetValueAsObject(KeyName) : nullptr;
}

AActor* UGsBTFunctionLibraryExtend::GetBlackboardValueAsActorEx(UBTNode* NodeOwner, const FName& KeyName)
{
    return Cast<AActor>(GetBlackboardValueAsObjectEx(NodeOwner, KeyName));
}

UClass* UGsBTFunctionLibraryExtend::GetBlackboardValueAsClassEx(UBTNode* NodeOwner, const FName& KeyName)
{
    UBlackboardComponent* BlackboardComp = GetOwnersBlackboard(NodeOwner);
    return BlackboardComp ? BlackboardComp->GetValueAsClass(KeyName) : nullptr;
}

uint8 UGsBTFunctionLibraryExtend::GetBlackboardValueAsEnumEx(UBTNode* NodeOwner, const FName& KeyName)
{
    UBlackboardComponent* BlackboardComp = GetOwnersBlackboard(NodeOwner);
    return BlackboardComp ? BlackboardComp->GetValueAsEnum(KeyName) : 0;
}

int32 UGsBTFunctionLibraryExtend::GetBlackboardValueAsIntEx(UBTNode* NodeOwner, const FName& KeyName)
{
    UBlackboardComponent* BlackboardComp = GetOwnersBlackboard(NodeOwner);
    return BlackboardComp ? BlackboardComp->GetValueAsInt(KeyName) : 0;
}

float UGsBTFunctionLibraryExtend::GetBlackboardValueAsFloatEx(UBTNode* NodeOwner, const FName& KeyName)
{
    UBlackboardComponent* BlackboardComp = GetOwnersBlackboard(NodeOwner);
    return BlackboardComp ? BlackboardComp->GetValueAsFloat(KeyName) : 0.0f;
}

bool UGsBTFunctionLibraryExtend::GetBlackboardValueAsBoolEx(UBTNode* NodeOwner, const FName& KeyName)
{
    UBlackboardComponent* BlackboardComp = GetOwnersBlackboard(NodeOwner);
    return BlackboardComp ? BlackboardComp->GetValueAsBool(KeyName) : false;
}

FString UGsBTFunctionLibraryExtend::GetBlackboardValueAsStringEx(UBTNode* NodeOwner, const FName& KeyName)
{
    UBlackboardComponent* BlackboardComp = GetOwnersBlackboard(NodeOwner);
    return BlackboardComp ? BlackboardComp->GetValueAsString(KeyName) : FString();
}

FName UGsBTFunctionLibraryExtend::GetBlackboardValueAsNameEx(UBTNode* NodeOwner, const FName& KeyName)
{
    UBlackboardComponent* BlackboardComp = GetOwnersBlackboard(NodeOwner);
    return BlackboardComp ? BlackboardComp->GetValueAsName(KeyName) : NAME_None;
}

FVector UGsBTFunctionLibraryExtend::GetBlackboardValueAsVectorEx(UBTNode* NodeOwner, const FName& KeyName)
{
    UBlackboardComponent* BlackboardComp = GetOwnersBlackboard(NodeOwner);
    return BlackboardComp ? BlackboardComp->GetValueAsVector(KeyName) : FVector::ZeroVector;
}

FRotator UGsBTFunctionLibraryExtend::GetBlackboardValueAsRotatorEx(UBTNode* NodeOwner, const FName& KeyName)
{
    UBlackboardComponent* BlackboardComp = GetOwnersBlackboard(NodeOwner);
    return BlackboardComp ? BlackboardComp->GetValueAsRotator(KeyName) : FRotator::ZeroRotator;
}

UGsGameObjectBase* UGsBTFunctionLibraryExtend::GetBlackboardValueAsGameObjectEx(UBTNode* NodeOwner, const FName& KeyName)
{
    return Cast<UGsGameObjectBase>(GetBlackboardValueAsObjectEx(NodeOwner, KeyName));
}

void UGsBTFunctionLibraryExtend::SetBlackboardData(UBTNode* NodeOwner, const FGsBTBlackboardDataResult& Result, const FGsBTBlackboardDataParser& Parser)
{
    if (UBlackboardComponent* bb = GetOwnersBlackboard(NodeOwner))
    {
        DEF_SETBLACKBOARDDATA(bb, Object, Parser, Result);
        DEF_SETBLACKBOARDDATA(bb, Class, Parser, Result);
        DEF_SETBLACKBOARDDATA(bb, Enum, Parser, Result);
        DEF_SETBLACKBOARDDATA(bb, Int, Parser, Result);
        DEF_SETBLACKBOARDDATA(bb, Float, Parser, Result);
        DEF_SETBLACKBOARDDATA(bb, Bool, Parser, Result);
        DEF_SETBLACKBOARDDATA(bb, String, Parser, Result);
        DEF_SETBLACKBOARDDATA(bb, Name, Parser, Result);
        DEF_SETBLACKBOARDDATA(bb, Vector, Parser, Result);
        DEF_SETBLACKBOARDDATA(bb, Rotator, Parser, Result);
    }
}

void UGsBTFunctionLibraryExtend::SetBlackboardEmptyData(UBTNode* NodeOwner, const FGsBTBlackboardDataParser& Parser)
{
    if (UBlackboardComponent* bb = GetOwnersBlackboard(NodeOwner))
    {
        DEF_CLEARBLACKBOARDDATA(bb, Object, Parser);
        DEF_CLEARBLACKBOARDDATA(bb, Class, Parser);
        DEF_CLEARBLACKBOARDDATA(bb, Enum, Parser);
        DEF_CLEARBLACKBOARDDATA(bb, Int, Parser);
        DEF_CLEARBLACKBOARDDATA(bb, Float, Parser);
        DEF_CLEARBLACKBOARDDATA(bb, Bool, Parser);
        DEF_CLEARBLACKBOARDDATA(bb, String, Parser);
        DEF_CLEARBLACKBOARDDATA(bb, Name, Parser);
        DEF_CLEARBLACKBOARDDATA(bb, Vector, Parser);
        DEF_CLEARBLACKBOARDDATA(bb, Rotator, Parser);
    }
}

void UGsBTFunctionLibraryExtend::SetRegistBlackboardData(UBTNode* NodeOwner, const FGsBTBlackboardDataParser& Parser)
{
    if (UBlackboardComponent* bb = GetOwnersBlackboard(NodeOwner))
    {
        DEF_SETREGBLACKBOARDDATA(bb, Object, Parser, BlackboardRegister);
        DEF_SETREGBLACKBOARDDATA(bb, Class, Parser, BlackboardRegister);
        DEF_SETREGBLACKBOARDDATA(bb, Enum, Parser, BlackboardRegister);
        DEF_SETREGBLACKBOARDDATA(bb, Int, Parser, BlackboardRegister);
        DEF_SETREGBLACKBOARDDATA(bb, Float, Parser, BlackboardRegister);
        DEF_SETREGBLACKBOARDDATA(bb, Bool, Parser, BlackboardRegister);
        DEF_SETREGBLACKBOARDDATA(bb, String, Parser, BlackboardRegister);
        DEF_SETREGBLACKBOARDDATA(bb, Name, Parser, BlackboardRegister);
        DEF_SETREGBLACKBOARDDATA(bb, Vector, Parser, BlackboardRegister);
        DEF_SETREGBLACKBOARDDATA(bb, Rotator, Parser, BlackboardRegister);
    }
}

TArray<UGsGameObjectBase*> UGsBTFunctionLibraryExtend::FindObject(UBTNode* NodeOwner, AActor* ActorOwner, EGsGameObjectType ObjectType, float Radius)
{
    TArray<UGsGameObjectBase*> finds;

	finds = GSpawner()->FindObjectArray(ObjectType);
	if (Radius > 0.f)
	{
		return finds.FilterByPredicate([=](UGsGameObjectBase* el)
			{
				//Movement등에서 제공되는 함수로 대체가 필요할수 있다.
				if (AActor* a = el->GetActor())
				{
					if (Radius > a->GetDistanceTo(ActorOwner))
					{
						return true;
					}
				}
				return false;
			});
	}
    return finds;
}

FGsBTBlackboardDataResult& UGsBTFunctionLibraryExtend::BB_FindFirstObject(UBTNode* NodeOwner, AActor* ActorOwner, EGsGameObjectType ObjectType, float Radius)
{
    TArray<UGsGameObjectBase*> finds = FindObject(NodeOwner, ActorOwner, ObjectType, Radius);

    BlackboardData.Reset();
    if (finds.Num() <= 0)
    {
        //블랙보드 정보 자동 갱신관련 등록
        RegisterBlackboardData<UBlackboardKeyType_Object>(NULL);
        RegisterBlackboardData<UBlackboardKeyType_Vector>(FVector::ZeroVector);

        //블랙보드 아웃 데이터 설정
        BlackboardData.Object = NULL;
        BlackboardData.Vector = FVector::ZeroVector;
    }
    else
    {
        UGsGameObjectBase* object = finds[0];
        FVector pos = object->GetActor()->GetActorLocation();

        //블랙보드 정보 자동 갱신관련 등록
        RegisterBlackboardData<UBlackboardKeyType_Object>(object);
        RegisterBlackboardData<UBlackboardKeyType_Vector>(pos);

        //블랙보드 아웃 데이터 설정
        BlackboardData.Object = object;
        BlackboardData.Vector = pos;
    }

    return BlackboardData;
}
