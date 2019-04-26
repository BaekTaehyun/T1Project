//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "USQLite_Shared.h"

#include "Runtime/Engine/Classes/Curves/CurveFloat.h"
#include "Runtime/GameplayTags/Public/GameplayTags.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "Runtime/CoreUObject/Public/UObject/UnrealType.h"
#include "Runtime/CoreUObject/Public/UObject/SoftObjectPtr.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"

#include "USQLReflector.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class USQLite;
struct FSQLField;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UENUM(meta=(RestrictedToClasses=SQLite,BlueprintInternalUseOnly=true))
enum class ESQLWriteMode : uint8 {
	String,
	Struct,
	Vector2D,
	Vector3D,
	Rotator,
	Color,
	DateTime,
	TimeStamp,
	ObjectPtr
};

UENUM(meta=(RestrictedToClasses=SQLite,BlueprintInternalUseOnly=true))
enum class ESQLReadMode : uint8 {
	Map,
	Set,
	Array,
	Boolean,
	Integer,
	Float,
	Enum,
	Name,
	Text,
	String,
	Struct,
	Vector2D,
	Vector3D,
	Rotator,
	Color,
	FloatCurve,
	TimeStamp,
	ObjectPtr
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FORCEINLINE FArchive &operator << (FArchive &Ar, ESQLWriteMode &UEN) { 
	uint8 Bytes = (uint8)UEN; Ar << Bytes;
	if (Ar.IsLoading()) {UEN = (ESQLWriteMode)UEN;} return Ar;
}

FORCEINLINE FArchive &operator << (FArchive &Ar, ESQLReadMode &UEN) { 
	uint8 Bytes = (uint8)UEN; Ar << Bytes;
	if (Ar.IsLoading()) {UEN = (ESQLReadMode)UEN;} return Ar;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UProperty Reflection Core Interface:

UCLASS(ClassGroup = Synaptech, Category="Synaptech", Abstract, HideDropdown, hideCategories = ("Cooking", "Variable"))
class USQLITE_API USQLReflector : public UObject {
	GENERATED_BODY()
	//
	friend class USQLite;
	friend class ISQLiteDriver;
public:
	static bool Logs;
	static bool IsSupportedProperty(const UProperty* Property);
	//
	static FString UPropertyToSQL(UArrayProperty* Property, UObject* Container);
	static FString UPropertyToSQL(USetProperty* Property, const UObject* Container);
	static FString UPropertyToSQL(UMapProperty* Property, const UObject* Container);
	static FString UPropertyToSQL(UIntProperty* Property, const UObject* Container);
	static FString UPropertyToSQL(UBoolProperty* Property, const UObject* Container);
	static FString UPropertyToSQL(UByteProperty* Property, const UObject* Container);
	static FString UPropertyToSQL(UEnumProperty* Property, const UObject* Container);
	static FString UPropertyToSQL(UNameProperty* Property, const UObject* Container);
	static FString UPropertyToSQL(UTextProperty* Property, const UObject* Container);
	static FString UPropertyToSQL(UFloatProperty* Property, const UObject* Container);
	static FString UPropertyToSQL(UStrProperty* Property, const UObject* Container, const ESQLWriteMode Mode);
	static FString UPropertyToSQL(UStructProperty* Property, const UObject* Container, const ESQLWriteMode Mode);
	static FString UPropertyToSQL(UObjectProperty* Property, const UObject* Container, const ESQLWriteMode Mode);
	//
	static void SQLToUProperty(FSQLField &Field, UObjectProperty* Property, UObject* Container);
	static void SQLToUProperty(const FSQLField &Field, UIntProperty* Property, UObject* Container);
	static void SQLToUProperty(const FSQLField &Field, UStrProperty* Property, UObject* Container);
	static void SQLToUProperty(const FSQLField &Field, USetProperty* Property, UObject* Container);
	static void SQLToUProperty(const FSQLField &Field, UMapProperty* Property, UObject* Container);
	static void SQLToUProperty(const FSQLField &Field, UBoolProperty* Property, UObject* Container);
	static void SQLToUProperty(const FSQLField &Field, UByteProperty* Property, UObject* Container);
	static void SQLToUProperty(const FSQLField &Field, UNameProperty* Property, UObject* Container);
	static void SQLToUProperty(const FSQLField &Field, UTextProperty* Property, UObject* Container);
	static void SQLToUProperty(const FSQLField &Field, UEnumProperty* Property, UObject* Container);
	static void SQLToUProperty(const FSQLField &Field, UFloatProperty* Property, UObject* Container);
	static void SQLToUProperty(const FSQLField &Field, UArrayProperty* Property, UObject* Container);
	static void SQLToUProperty(const FSQLField &Field, UStructProperty* Property, UObject* Container);
	//
	static TSharedPtr<FJsonObject> ParseStructToJSON(UStructProperty* Property, const void* StructPtr);
	static void ParseStructToJSON(TSharedPtr<FJsonObject> &JSON, UStructProperty* Property, const void* StructPtr);
	static TSharedPtr<FJsonObject> ParseStructToJSON(FString &Output, UStructProperty* Property, const void* StructPtr);
	static void ParsePropertyToJSON(TSharedPtr<FJsonObject> &JSON, UProperty* Property, const void* ValuePtr, int64 CheckFlags, int64 SkipFlags);
	//
	static void ParseJSONtoStruct(TSharedPtr<FJsonObject> &JSON, UStructProperty* Property, void* StructPtr, UObject* Container);
	static void ParseJSONtoProperty(TSharedPtr<FJsonObject> &JSON, UProperty* Property, void* ValuePtr, UObject* Container, int64 CheckFlags, int64 SkipFlags);
	//
	//
	/** Loads the Default (Original, Root) Instance of an Object. */
	UFUNCTION(Category="Object", BlueprintCallable, meta = (DisplayName="Get Object of Class", Keywords = "Get Default Object"))
	static UObject* GetClassDefaultObject(UClass* Class);
	//
	/** Creates a Runtime New Instance of an Object. */
	UFUNCTION(Category="Object", BlueprintCallable, meta = (WorldContext = "Context", DisplayName="New Object Instance", Keywords = "New Object Instance"))
	static UObject* NewObjectInstance(UObject* Context, UClass* Class);
	//
	/** Creates a Runtime New Named Instance of an Object. */
	UFUNCTION(Category="Object", BlueprintCallable, meta = (WorldContext = "Context", DisplayName="New Object Instance", Keywords = "New Object Instance"))
	static UObject* NewONamedbjectInstance(UObject* Context, UClass* Class, FName Name);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UProperty Reflection Threaded Interface:

static void RestoreActorReferenceInGameThread(UObjectProperty* Property, UObject* Outer, const FString FullName) {
	if (!IsInGameThread()||(Outer==nullptr)||(Property==nullptr)) {return;}
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Outer);
	for (TActorIterator<AActor>Actor(Outer->GetWorld()); Actor; ++Actor) {
		if ((*Actor)->GetFullName()==FullName) {Property->SetPropertyValue(ValuePtr,(*Actor)); break;}
	}///
}

static void RestoreActorReferenceIntoArrayInGameThread(UArrayProperty* Property, UObject* Outer, const FString FullName, const int32 Index) {
	if (!IsInGameThread()||(Outer==nullptr)||(Property==nullptr)) {return;}
	//
	TArray<UObject*>&Array = *Property->ContainerPtrToValuePtr<TArray<UObject*>>(Outer);
	if (!Array.IsValidIndex(Index)) {return;}
	//
	for (TActorIterator<AActor>Actor(Outer->GetWorld()); Actor; ++Actor) {
		if ((*Actor)->GetFullName()==FullName) {Array[Index]=(*Actor); break;}
	}///
}

static void RestoreActorReferenceIntoSetInGameThread(USetProperty* Property, UObject* Outer, const FString FullName) {
	if (!IsInGameThread()||(Outer==nullptr)||(Property==nullptr)) {return;}
	//
	TSet<UObject*>&Set = *Property->ContainerPtrToValuePtr<TSet<UObject*>>(Outer);
	//
	for (TActorIterator<AActor>Actor(Outer->GetWorld()); Actor; ++Actor) {
		if ((*Actor)->GetFullName()==FullName) {Set.Add(*Actor); break;}
	}///
}

static void RestoreActorReferenceIntoMapKeyInGameThread(UMapProperty* Property, UObject* Outer, const FString FullName) {
	if (!IsInGameThread()||(Outer==nullptr)||(Property==nullptr)) {return;}
	//
	TMap<UObject*,void*>&Map = *Property->ContainerPtrToValuePtr<TMap<UObject*,void*>>(Outer);
	//
	for (TActorIterator<AActor>Actor(Outer->GetWorld()); Actor; ++Actor) {
		if ((*Actor)->GetFullName()==FullName) {Map.Add(*Actor); break;}
	}///
}

static void RestoreActorReferenceIntoMapValueInGameThread(UMapProperty* Property, UObject* Outer, UObject* Key, const FString FullName) {
	if (!IsInGameThread()||(Outer==nullptr)||(Property==nullptr)) {return;}
	//
	TMap<UObject*,UObject*>&Map = *Property->ContainerPtrToValuePtr<TMap<UObject*,UObject*>>(Outer);
	//
	for (TActorIterator<AActor>Actor(Outer->GetWorld()); Actor; ++Actor) {
		if ((*Actor)->GetFullName()==FullName) {Map.Emplace(Key,*Actor); break;}
	}///
}

static void RestoreActorReferenceIntoNamedMapValueInGameThread(UMapProperty* Property, UObject* Outer, FName Key, const FString FullName) {
	if (!IsInGameThread()||(Outer==nullptr)||(Property==nullptr)) {return;}
	//
	TMap<FName,UObject*>&Map = *Property->ContainerPtrToValuePtr<TMap<FName,UObject*>>(Outer);
	//
	for (TActorIterator<AActor>Actor(Outer->GetWorld()); Actor; ++Actor) {
		if ((*Actor)->GetFullName()==FullName) {Map.Emplace(Key,*Actor); break;}
	}///
}

static void NewObjectReferenceInGameThread(UObjectProperty* Property, UObject* Outer, UObject* CDO, const FString FullName) {
	if (!IsInGameThread()||(Outer==nullptr)||(CDO==nullptr)||(Property==nullptr)) {return;}
	if (CDO->IsA(AActor::StaticClass())) {return;}
	//
	bool IsSet = false;
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Outer);
	//
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if ((*OBJ)->GetFullName()==FullName) {Property->SetPropertyValue(ValuePtr,(*OBJ)); IsSet=true; break;}
	} if (!IsSet) {Property->SetPropertyValue(ValuePtr,NewObject<UObject>(Outer,CDO->GetClass(),NAME_None,RF_NoFlags,CDO));}
}

static void NewObjectIntoArrayInGameThread(UArrayProperty* Property, UObject* Outer, UObject* CDO, const FString FullName, const int32 Index) {
	if (!IsInGameThread()||(Outer==nullptr)||(CDO==nullptr)||(Property==nullptr)) {return;}
	if (CDO->IsA(AActor::StaticClass())) {return;}
	//
	TArray<UObject*>&Array = *Property->ContainerPtrToValuePtr<TArray<UObject*>>(Outer);
	if (!Array.IsValidIndex(Index)) {return;}
	//
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if ((*OBJ)->GetFullName()==FullName) {Array[Index]=(*OBJ); break;}
	} if (Array[Index]==nullptr) {Array[Index]=NewObject<UObject>(Outer,CDO->GetClass(),NAME_None,RF_NoFlags,CDO);}
}

static void NewObjectIntoSetInGameThread(USetProperty* Property, UObject* Outer, UObject* CDO, const FString FullName) {
	if (!IsInGameThread()||(Outer==nullptr)||(CDO==nullptr)||(Property==nullptr)) {return;}
	if (CDO->IsA(AActor::StaticClass())) {return;}
	//
	bool IsSet = false;
	TSet<UObject*>&Set = *Property->ContainerPtrToValuePtr<TSet<UObject*>>(Outer);
	//
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if ((*OBJ)->GetFullName()==FullName) {Set.Add(*OBJ); IsSet=true; break;}
	} if (!IsSet) {Set.Add(NewObject<UObject>(Outer,CDO->GetClass(),NAME_None,RF_NoFlags,CDO));}
}

static void NewObjectIntoMapKeyInGameThread(UMapProperty* Property, UObject* Outer, UObject* CDO, const FString FullName) {
	if (!IsInGameThread()||(Outer==nullptr)||(CDO==nullptr)||(Property==nullptr)) {return;}
	if (CDO->IsA(AActor::StaticClass())) {return;}
	//
	bool IsSet = false;
	TMap<UObject*,void*>&Map = *Property->ContainerPtrToValuePtr<TMap<UObject*,void*>>(Outer);
	//
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if ((*OBJ)->GetFullName()==FullName) {Map.Add(*OBJ); IsSet=true; break;}
	} if (!IsSet) {Map.Add(NewObject<UObject>(Outer,CDO->GetClass(),NAME_None,RF_NoFlags,CDO));}
}

static void NewObjectIntoMapValueInGameThread(UMapProperty* Property, UObject* Outer, UObject* Key, UObject* CDO, const FString FullName) {
	if (!IsInGameThread()||(Outer==nullptr)||(CDO==nullptr)||(Property==nullptr)) {return;}
	if (CDO->IsA(AActor::StaticClass())) {return;}
	//
	bool IsSet = false;
	TMap<UObject*,UObject*>&Map = *Property->ContainerPtrToValuePtr<TMap<UObject*,UObject*>>(Outer);
	//
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if ((*OBJ)->GetFullName()==FullName) {Map.Emplace(Key,*OBJ); IsSet=true; break;}
	} if (!IsSet) {Map.Emplace(Key,NewObject<UObject>(Outer,CDO->GetClass(),NAME_None,RF_NoFlags,CDO));}
}

static void NewObjectIntoNamedMapValueInGameThread(UMapProperty* Property, UObject* Outer, FName Key, UObject* CDO, const FString FullName) {
	if (!IsInGameThread()||(Outer==nullptr)||(CDO==nullptr)||(Property==nullptr)) {return;}
	if (CDO->IsA(AActor::StaticClass())) {return;}
	//
	bool IsSet = false;
	TMap<FName,UObject*>&Map = *Property->ContainerPtrToValuePtr<TMap<FName,UObject*>>(Outer);
	//
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if ((*OBJ)->GetFullName()==FullName) {Map.Emplace(Key,*OBJ); IsSet=true; break;}
	} if (!IsSet) {Map.Emplace(Key,NewObject<UObject>(Outer,CDO->GetClass(),NAME_None,RF_NoFlags,CDO));}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////