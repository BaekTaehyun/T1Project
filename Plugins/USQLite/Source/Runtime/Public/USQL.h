//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "sqlite3.h"
#include "USQLReflector.h"

#include "Runtime/Core/Public/Core.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Core/Public/Async/AsyncWork.h"
#include "Runtime/Engine/Classes/Engine/Player.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Engine/EngineBaseTypes.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Components/ActorComponent.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerState.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"

#include "USQL.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DECLARE_LOG_CATEGORY_EXTERN(USQLog,Log,All);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DBSQL sqlite3*
#define DBSTM sqlite3_stmt*
#define DB_FINAL(Statement) sqlite3_finalize(Statement)
#define DB_CACHE(DB,Cache) sqlite3_exec(DB,TCHAR_TO_UTF8(Cache),nullptr,nullptr,nullptr)
#define DBT_END(DB,ErrorMSG) sqlite3_exec(DB,TCHAR_TO_UTF8(TEXT("END TRANSACTION")),nullptr,nullptr,&ErrorMSG)
#define DBT_BEGIN(DB,ErrorMSG) sqlite3_exec(DB,TCHAR_TO_UTF8(TEXT("BEGIN TRANSACTION")),nullptr,nullptr,&ErrorMSG)

#define DBNULL TEXT("NULL,")

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class USQL;
class USQLite;
class DBS_ExecuteQueue_Task;
class DBL_ExecuteQueue_Task;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ESQLSeverity : uint8 {
	Fatal				= 0,
	Error				= 1,
	Warning				= 2,
	Info				= 3
};

UENUM(meta=(RestrictedToClasses=SQLite,BlueprintInternalUseOnly=true))
enum class ESQLType : uint8 {
	/// Return Value is of Unknown Data Type.
	Unknown				= 0,
	/// Return Value is of Integer Data Type.
	Integer				= 1,
	/// Return Value is of String Data Type.
	Text				= 2,
	/// Return Value is of Float Data Type.
	Float				= 3
};

UENUM(BlueprintType,meta=(DisplayName="SQL Result",RestrictedToClasses=SQLite,BlueprintInternalUseOnly=true))
enum class ESQLResult : uint8 {
	OK					= 0,				/// Successful result.
	ERROR				= 1,				/// SQL error or missing database.
	INTERNAL			= 2,				/// Internal logic error in SQLite.
	PERM				= 3,				/// Access permission denied.
	ABORT				= 4,				/// Callback routine requested an abort.
	BUSY				= 5,				/// The database file is locked.
	LOCKED				= 6,				/// A table in the database is locked.
	NOMEM				= 7,				/// A malloc() failed.
	READONLY			= 8,				/// Attempt to write a read-only database.
	INTERRUPT			= 9,				/// Operation terminated by sqlite3_interrupt().
	IOERR				= 10,				/// Some kind of disk I/O error occurred.
	CORRUPT				= 11,				/// The database disk image is malformed.
	NOTFOUND			= 12,				/// Unknown opcode in sqlite3_file_control().
	FULL				= 13,				/// Insertion failed because database is full.
	CANTOPEN			= 14,				/// Unable to open the database file.
	PROTOCOL			= 15,				/// Database lock protocol error.
	EMPTY				= 16,				/// Database is empty.
	SCHEMA				= 17,				/// The database schema changed.
	TOOBIG				= 18,				/// String or BLOB exceeds size limit.
	CONSTRAINT			= 19,				/// Abort due to constraint violation.
	MISMATCH			= 20,				/// Data type mismatch.
	MISUSE				= 21,				/// Library used incorrectly.
	NOLFS				= 22,				/// Uses OS features not supported on host.
	AUTH				= 23,				/// Authorization denied.
	FORMAT				= 24,				/// Auxiliary database format error.
	RANGE				= 25,				/// 2nd parameter to sqlite3_bind out of range.
	NOTADB				= 26,				/// File opened that is not a database file.
	NOTICE				= 27,				/// Notifications from sqlite3_log().
	WARNING				= 28,				/// Warnings from sqlite3_log().
	ROW					= 100,				/// sqlite3_step() has another row ready.
	DONE				= 101,				/// sqlite3_step() has finished executing.
	QUEUED				= 200				/// ExecSql has been added to execution queue.
};

UENUM(BlueprintType,meta=(DisplayName="SQL Load Mode",RestrictedToClasses=SQLite,BlueprintInternalUseOnly=true))
enum class ESQLoadMode : uint8 {
	Internal,
	Runtime
};

UENUM(BlueprintType,meta=(DisplayName="SQL Save Mode",RestrictedToClasses=SQLite,BlueprintInternalUseOnly=true))
enum class ESQLSaveMode : uint8 {
	Insert,
	Update
};

UENUM(BlueprintType,meta=(DisplayName="SQL Runtime Mode",RestrictedToClasses=SQLite,BlueprintInternalUseOnly=true))
enum class ESQLRuntimeMode : uint8 {
	/// Executes SQL Commands within Game Thread.
	SynchronousTask,
	/// Executes SQL Commands from its own Asynchronous Threaded.
	BackgroundTask
};

UENUM(BlueprintType,meta=(DisplayName="SQL Thread Safety",RestrictedToClasses=SQLite,BlueprintInternalUseOnly=true))
enum class ESQLThreadSafety : uint8 {
	/// Is Safe to Bind any Interface Calls.
	IsCurrentlyThreadSafe,
	/// Is about to perform runtime expensive operations.
	IsPreparingToSaveOrLoad,
	/// Is Unsafe Loading. Any interaction with Main Game's Thread will crash!
	AsynchronousLoading,
	/// Is Unsafe Saving. Any interaction with Main Game's Thread will crash!
	AsynchronousSaving
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FORCEINLINE FArchive &operator << (FArchive &Ar, ESQLResult &UEN) { 
	uint8 Bytes = (uint8)UEN; Ar << Bytes;
	if (Ar.IsLoading()) {UEN = (ESQLResult)UEN;} return Ar;
}

FORCEINLINE FArchive &operator << (FArchive &Ar, ESQLType &UEN) { 
	uint8 Bytes = (uint8)UEN; Ar << Bytes;
	if (Ar.IsLoading()) {UEN = (ESQLType)UEN;} return Ar;
}

FORCEINLINE FArchive &operator << (FArchive &Ar, ESQLoadMode &UEN) { 
	uint8 Bytes = (uint8)UEN; Ar << Bytes;
	if (Ar.IsLoading()) {UEN = (ESQLoadMode)UEN;} return Ar;
}

FORCEINLINE FArchive &operator << (FArchive &Ar, ESQLSaveMode &UEN) { 
	uint8 Bytes = (uint8)UEN; Ar << Bytes;
	if (Ar.IsLoading()) {UEN = (ESQLSaveMode)UEN;} return Ar;
}

FORCEINLINE FArchive &operator << (FArchive &Ar, ESQLRuntimeMode &UEN) { 
	uint8 Bytes = (uint8)UEN; Ar << Bytes;
	if (Ar.IsLoading()) {UEN = (ESQLRuntimeMode)UEN;} return Ar;
}

FORCEINLINE FArchive &operator << (FArchive &Ar, ESQLThreadSafety &UEN) { 
	uint8 Bytes = (uint8)UEN; Ar << Bytes;
	if (Ar.IsLoading()) {UEN = (ESQLThreadSafety)UEN;} return Ar;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

USTRUCT(meta=(RestrictedToClasses=SQLite,BlueprintInternalUseOnly=true))
struct FSQLTypes {
	GENERATED_USTRUCT_BODY()
	//
	//
	UPROPERTY()
	FString Bools;
	//
	UPROPERTY()
	FString Bytes;
	//
	UPROPERTY()
	FString Enums;
	//
	UPROPERTY()
	FString Ints;
	//
	UPROPERTY()
	FString Floats;
	//
	UPROPERTY()
	FString Names;
	//
	UPROPERTY()
	FString Texts;
	//
	UPROPERTY()
	FString Strings;
	//
	UPROPERTY()
	FString Colors;
	//
	UPROPERTY()
	FString Structs;
	//
	UPROPERTY()
	FString Vector2D;
	//
	UPROPERTY()
	FString Vector3D;
	//
	UPROPERTY()
	FString Rotators;
	//
	UPROPERTY()
	FString DateTimes;
	//
	UPROPERTY()
	FString TimeStamps;
	//
	UPROPERTY()
	FString ObjectPtrs;
	//
	UPROPERTY()
	FString Sets;
	//
	UPROPERTY()
	FString Maps;
	//
	UPROPERTY()
	FString Arrays;
	//
	//
	/** Converts Unreal Type String to a SQLite Type String Equivalent. */
	FString Downcast(FString &Input, bool NotNull) {
		FString _Input = *Input;
		if (NotNull) {
			if (Input.Equals(Ints)) {_Input = Ints + TEXT(" NOT NULL");}
			if (Input.Equals(Sets)) {_Input = Texts + TEXT(" NOT NULL");}
			if (Input.Equals(Maps)) {_Input = Texts + TEXT(" NOT NULL");}
			if (Input.Equals(Bytes)) {_Input = Ints + TEXT(" NOT NULL");}
			if (Input.Equals(Enums)) {_Input = Texts + TEXT(" NOT NULL");}
			if (Input.Equals(Bools)) {_Input = Texts + TEXT(" NOT NULL");}
			if (Input.Equals(Names)) {_Input = Texts + TEXT(" NOT NULL");}
			if (Input.Equals(Texts)) {_Input = Texts + TEXT(" NOT NULL");}
			if (Input.Equals(Colors)) {_Input = Texts + TEXT(" NOT NULL");}
			if (Input.Equals(Arrays)) {_Input = Texts + TEXT(" NOT NULL");}
			if (Input.Equals(Structs)) {_Input = Texts + TEXT(" NOT NULL");}
			if (Input.Equals(Strings)) {_Input = Texts + TEXT(" NOT NULL");}
			if (Input.Equals(DateTimes)) {_Input = Ints + TEXT(" NOT NULL");}
			if (Input.Equals(Vector2D)) {_Input = Texts + TEXT(" NOT NULL");}
			if (Input.Equals(Vector3D)) {_Input = Texts + TEXT(" NOT NULL");}
			if (Input.Equals(Rotators)) {_Input = Texts + TEXT(" NOT NULL");}
			if (Input.Equals(TimeStamps)) {_Input = Texts + TEXT(" NOT NULL");}
			if (Input.Equals(ObjectPtrs)) {_Input = Texts + TEXT(" NOT NULL");}
			if (Input.Equals(Floats)) {_Input = FString(TEXT("REAL"))+(TEXT(" NOT NULL"));}
		} else {
			if (Input.Equals(Ints)) {_Input = Ints;}
			if (Input.Equals(Sets)) {_Input = Texts;}
			if (Input.Equals(Maps)) {_Input = Texts;}
			if (Input.Equals(Bytes)) {_Input = Ints;}
			if (Input.Equals(Enums)) {_Input = Texts;}
			if (Input.Equals(Bools)) {_Input = Texts;}
			if (Input.Equals(Names)) {_Input = Texts;}
			if (Input.Equals(Texts)) {_Input = Texts;}
			if (Input.Equals(Arrays)) {_Input = Texts;}
			if (Input.Equals(Colors)) {_Input = Texts;}
			if (Input.Equals(Structs)) {_Input = Texts;}
			if (Input.Equals(Strings)) {_Input = Texts;}
			if (Input.Equals(DateTimes)) {_Input = Ints;}
			if (Input.Equals(Vector2D)) {_Input = Texts;}
			if (Input.Equals(Vector3D)) {_Input = Texts;}
			if (Input.Equals(Rotators)) {_Input = Texts;}
			if (Input.Equals(TimeStamps)) {_Input = Texts;}
			if (Input.Equals(ObjectPtrs)) {_Input = Texts;}
			if (Input.Equals(Floats)) {_Input = TEXT("REAL");}
	} return _Input;}
	//
	FSQLTypes()
		: Bools(TEXT("BOOL"))
		, Bytes(TEXT("BYTE"))
		, Enums(TEXT("ENUM"))
		, Ints(TEXT("INT"))
		, Floats(TEXT("FLOAT"))
		, Names(TEXT("NAME"))
		, Texts(TEXT("TEXT"))
		, Strings(TEXT("STRING"))
		, Colors(TEXT("COLOR"))
		, Structs(TEXT("STRUCT"))
		, Vector2D(TEXT("VECTOR 2D"))
		, Vector3D(TEXT("VECTOR 3D"))
		, Rotators(TEXT("ROTATOR"))
		, DateTimes(TEXT("DATETIME"))
		, TimeStamps(TEXT("TIMESTAMP"))
		, ObjectPtrs(TEXT("OBJECT"))
		, Sets(TEXT("SET"))
		, Maps(TEXT("MAP"))
		, Arrays(TEXT("ARRAY"))
	{}//
};

USTRUCT(meta=(RestrictedToClasses=SQLite,BlueprintInternalUseOnly=true))
struct FSQLField {
	GENERATED_USTRUCT_BODY()
	//
	UPROPERTY() FName Name;
	UPROPERTY() ESQLType Type;
	UPROPERTY() ESQLReadMode Cast;
	//
	UPROPERTY() double Float;
	UPROPERTY() FString Text;
	UPROPERTY() int64 Integer;
	UPROPERTY() TArray<uint8> Data;
	//
	FString ValueToString() {
		if (Type==ESQLType::Integer) {return FString::Printf(TEXT("%i"),Integer);}
		if (Type==ESQLType::Float) {return FString::Printf(TEXT("%f"),Float);}
		if (Type==ESQLType::Unknown) {return TEXT("[UNKNOWN]");}
		if (Type==ESQLType::Text) {return Text;}
	return TEXT("");}
	//
	FString TypeToString() {
		if (Type==ESQLType::Unknown) {return TEXT("Unknown");}
		if (Type==ESQLType::Integer) {return TEXT("Integer");}
		if (Type==ESQLType::Float) {return TEXT("Float");}
		if (Type==ESQLType::Text) {return TEXT("Text");}
	return TEXT("");}
	//
	FORCEINLINE bool operator == (const FSQLField &Field) {
		return (Name==Field.Name);
	}///
};

USTRUCT(BlueprintType,meta=(RestrictedToClasses=SQLite,BlueprintInternalUseOnly=true))
struct FSQLRow {
	GENERATED_USTRUCT_BODY()
	//
	//
	UPROPERTY(Category="USQL", BlueprintReadOnly)
	FName DB;
	//
	UPROPERTY(Category="USQL", BlueprintReadOnly)
	FName ID;
	//
	UPROPERTY()
	TArray<FSQLField> Values;
	//
	//
#if WITH_EDITORONLY_DATA
	UPROPERTY(Category="USQL", EditDefaultsOnly)
	TArray<FString> PreviewValues;
	//
	UPROPERTY(Category="USQL", EditDefaultsOnly)
	TArray<FString> PreviewTypes;
#endif
	//
	FORCEINLINE FSQLRow &operator = (const FSQLRow &Row) {
		DB = Row.DB;
		ID = Row.ID;
		Values = Row.Values;
		#if WITH_EDITOR
		 PreviewValues = Row.PreviewValues;
		 PreviewTypes = Row.PreviewTypes;
		#endif
	return *this;}
	//
	FORCEINLINE bool operator == (const FSQLRow &Row) {
		return (
			(Values.GetAllocatedSize()==Row.Values.GetAllocatedSize()) &&
			(ID == Row.ID) && (DB == Row.DB)
		);//
	}///
};

USTRUCT(meta=(RestrictedToClasses=SQLite,BlueprintInternalUseOnly=true))
struct USQLITE_API FSQLTable {
	GENERATED_USTRUCT_BODY()
	//
	//
	UPROPERTY(Category="USQL", EditDefaultsOnly)
	FString Header;
	//
	UPROPERTY(Category="USQL", EditDefaultsOnly)
	TArray<FString> ColumnTypes;
	//
	UPROPERTY(Category="USQL", EditDefaultsOnly)
	TArray<FString> Columns;
	//
	UPROPERTY(Category="USQL", EditDefaultsOnly)
	FString Footer;
	//
	UPROPERTY(Category="USQL", EditDefaultsOnly)
	bool Lock;
};

USTRUCT(meta=(RestrictedToClasses=SQLite,BlueprintInternalUseOnly=true))
struct USQLITE_API FSQLRedirect {
	GENERATED_USTRUCT_BODY()
	//
	UPROPERTY(Category="USQL", EditDefaultsOnly)
	TMap<FName,FName> Redirectors;
	//
	//
	friend FORCEINLINE uint32 GetTypeHash(const FSQLRedirect &RED) {
		return FCrc::MemCrc32(&RED,sizeof(FSQLRedirect));
	}///
	//
	FORCEINLINE FSQLRedirect &operator = (const FSQLRedirect &RED) {
		Redirectors = RED.Redirectors;
	return *this;}
};

USTRUCT(meta=(RestrictedToClasses=SQLite,BlueprintInternalUseOnly=true))
struct USQLITE_API FSQLVersion {
	GENERATED_USTRUCT_BODY()
	//
	UPROPERTY(Category="USQL", EditDefaultsOnly)
	FString CurrentVersion;
	//
	UPROPERTY(Category="USQL", EditDefaultsOnly)
	TArray<FString> TargetVersions;
};

USTRUCT(meta=(RestrictedToClasses=SQLite,BlueprintInternalUseOnly=true))
struct USQLITE_API FSQLPreview {
	GENERATED_USTRUCT_BODY()
	//
	UPROPERTY(Category="USQL", EditDefaultsOnly)
	TArray<FSQLRow> Rows;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FORCEINLINE FArchive &operator << (FArchive &Ar, FSQLTypes &UST) {
	Ar << UST.Arrays;
	Ar << UST.Bools;
	Ar << UST.Bytes;
	Ar << UST.Colors;
	Ar << UST.DateTimes;
	Ar << UST.Enums;
	Ar << UST.Floats;
	Ar << UST.Ints;
	Ar << UST.Maps;
	Ar << UST.Names;
	Ar << UST.ObjectPtrs;
	Ar << UST.Rotators;
	Ar << UST.Sets;
	Ar << UST.Strings;
	Ar << UST.Structs;
	Ar << UST.Texts;
	Ar << UST.TimeStamps;
	Ar << UST.Vector2D;
	Ar << UST.Vector3D;
	//
	return Ar;
}

FORCEINLINE FArchive &operator << (FArchive &Ar, FSQLField &UST) {
	Ar << UST.Cast;
	Ar << UST.Data;
	Ar << UST.Float;
	Ar << UST.Integer;
	Ar << UST.Name;
	Ar << UST.Text;
	Ar << UST.Type;
	//
	return Ar;
}

FORCEINLINE FArchive &operator << (FArchive &Ar, FSQLRow &UST) {
	Ar << UST.DB;
	Ar << UST.ID;
	Ar << UST.Values;
	//
	return Ar;
}

FORCEINLINE FArchive &operator << (FArchive &Ar, FSQLTable &UST) {
	Ar << UST.Columns;
	Ar << UST.ColumnTypes;
	Ar << UST.Footer;
	Ar << UST.Header;
	Ar << UST.Lock;
	//
	return Ar;
}

FORCEINLINE FArchive &operator << (FArchive &Ar, FSQLRedirect &UST) {
	Ar << UST.Redirectors;
	//
	return Ar;
}

FORCEINLINE FArchive &operator << (FArchive &Ar, FSQLVersion &UST) {
	Ar << UST.CurrentVersion;
	Ar << UST.TargetVersions;
	//
	return Ar;
}

FORCEINLINE FArchive &operator << (FArchive &Ar, FSQLPreview &UST) {
	Ar << UST.Rows;
	//
	return Ar;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSQL_GeneralProgressReport,USQLite*,Database,const float,Progress);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSQL_BeginDataSAVE,USQLite*,Database);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSQL_FinishDataSAVE,USQLite*,Database,const bool,Success);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSQL_ProgressDataSAVE,USQLite*,Database,const float,Progress);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSQL_BeginDataLOAD,USQLite*,Database);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSQL_FinishDataLOAD,USQLite*,Database,const bool,Success);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSQL_ProgressDataLOAD,USQLite*,Database,const float,Progress,FSQLRow,Data);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SQLite Core Interface:

UCLASS(ClassGroup = Synaptech, Category = "Synaptech", Abstract, HideDropdown, hideCategories = ("Cooking", "Variable"))
class USQLITE_API USQL : public USQLReflector {
	GENERATED_BODY()
	friend class USQLite;
	friend class ISQLiteDriver;
	friend class DBS_ExecuteQueue_Task;
	friend class DBL_ExecuteQueue_Task;
	//
	USQL();
private:
	static ESQLThreadSafety LastThreadState;
	static ESQLThreadSafety ThreadSafety;
	//
	static float DBS_Progress;
	static float DBS_Workload;
	static float DBS_Complete;
	static float DBL_Progress;
	static float DBL_Workload;
	static float DBL_Complete;
	//
	static TArray<FSQLRow> DB_Load__Internal(USQLite* Database);
	static void DBS_ExecuteQueue__Threaded(USQLite* Database);
	static void DBL_ExecuteQueue__Threaded(USQLite* Database);
protected:
	static void DB_Save(USQLite* Database);
	static void DB_Load(USQLite* Database);
	static void DB_ImmediateSave(USQLite* Database, const FString SQL);
	static void DB_ImmediateLoad(USQLite* Database, AActor* Outer, const FString SQL);
	static void DB_ImmediateLoad(USQLite* Database, UObject* Outer, const FString SQL);
	static void DB_ImmediateLoad(USQLite* Database, UActorComponent* Outer, const FString SQL);
	//
	static bool DB_CloseDatabase(sqlite3* DB);
	static bool DB_DatabaseExists(const FString Path);
	static bool DB_OpenDatabase(sqlite3 *& DB, const FString Path);
	static void DB_AddToQueue(const FString Query, TSet<FString> &QUEUE);
	static void DB_RemoveFromQueue(const FString Query, TSet<FString> &QUEUE);
	static ESQLResult DB_Execute(sqlite3* DB, const FString File, const FString Query);
	//
	static void DBS_ExecuteQueue(USQLite* Database);
	static TArray<FSQLRow> DBL_ExecuteQueue(USQLite* Database, ESQLoadMode LoadMode);
public:
	static FSQLTypes USQLTypes;
	//
	//
	static FSQL_GeneralProgressReport EVENT_ProgressReportSAVE;
	static FSQL_GeneralProgressReport EVENT_ProgressReportLOAD;
	//
	static FSQL_BeginDataSAVE EVENT_OnBeginDataSAVE;
	static FSQL_FinishDataSAVE EVENT_OnFinishDataSAVE;
	static FSQL_ProgressDataSAVE EVENT_OnProgressDataSAVE;
	//
	static FSQL_BeginDataLOAD EVENT_OnBeginDataLOAD;
	static FSQL_FinishDataLOAD EVENT_OnFinishDataLOAD;
	static FSQL_ProgressDataLOAD EVENT_OnProgressDataLOAD;
	//
	//
	UFUNCTION(Category="USQL", BlueprintCallable)
	static FString SanitizeString(const FString String);
	//
	UFUNCTION(Category="USQL", BlueprintCallable)
	static FString StringToMD5(const FString String);
	//
	UFUNCTION(Category="USQL", BlueprintCallable)
	static FString MakeObjectDBID(UObject* OBJ);
	//
	UFUNCTION(Category="USQL", BlueprintCallable)
	static FString MakeActorDBID(AActor* Actor);
	//
	UFUNCTION(Category="USQL", BlueprintCallable)
	static FString MakeComponentDBID(const UActorComponent* CMP);
	//
	UFUNCTION(Category="USQL", BlueprintCallable)
	static bool GetPlayerNetworkID(const APlayerController* PlayerController, FString &ID, const bool AppendPort);
	//
	//
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[Database] Get Thread Safety"))
	static const ESQLThreadSafety GetThreadSafety() {return ThreadSafety;}
	//
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[Database] Get Save Progress"))
	static const float GetSaveProgress() {return DBS_Progress;}
	//
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[Database] Get Load Progress"))
	static const float GetLoadProgress() {return DBL_Progress;}
	//
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[Database] Get Saves Done"))
	static const float GetSavesDone() {return DBS_Complete;}
	//
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[Database] Get Loads Done"))
	static const float GetLoadsDone() {return DBL_Complete;}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

USQLITE_API void LOG_DB(const bool Debug, const ESQLSeverity Severity, const FString Message);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////