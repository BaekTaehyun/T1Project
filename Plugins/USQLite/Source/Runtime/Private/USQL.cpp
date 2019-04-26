//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "USQL.h"


extern "C"
{
	/** Perform additional initialization during sqlite3_initialize */
	SQLITE_API int sqlite3_os_init()
	{
		return SQLITE_OK;
	}

	/** Perform additional shutdown during sqlite3_shutdown */
	SQLITE_API int sqlite3_os_end()
	{
		return SQLITE_OK;
	}

}	// extern "C"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DEFINE_LOG_CATEGORY(USQLog);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

USQL::USQL() {
	SetFlagsTo(GetFlags()|RF_MarkAsRootSet);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FSQL_GeneralProgressReport USQL::EVENT_ProgressReportSAVE;
FSQL_GeneralProgressReport USQL::EVENT_ProgressReportLOAD;

FSQL_BeginDataSAVE USQL::EVENT_OnBeginDataSAVE;
FSQL_FinishDataSAVE USQL::EVENT_OnFinishDataSAVE;
FSQL_ProgressDataSAVE USQL::EVENT_OnProgressDataSAVE;

FSQL_BeginDataLOAD USQL::EVENT_OnBeginDataLOAD;
FSQL_FinishDataLOAD USQL::EVENT_OnFinishDataLOAD;
FSQL_ProgressDataLOAD USQL::EVENT_OnProgressDataLOAD;

ESQLThreadSafety USQL::LastThreadState;
ESQLThreadSafety USQL::ThreadSafety;
FSQLTypes USQL::USQLTypes;

float USQL::DBS_Progress = 100.f;
float USQL::DBL_Progress = 100.f;
float USQL::DBS_Workload = 0.f;
float USQL::DBL_Workload = 0.f;
float USQL::DBS_Complete = 0.f;
float USQL::DBL_Complete = 0.f;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool USQL::DB_DatabaseExists(const FString Path) {
#if WITH_SERVER_CODE
	return FPlatformFileManager::Get().GetPlatformFile().FileExists(*Path);
#endif
	return false;
}

bool USQL::DB_OpenDatabase(sqlite3 *& DB, const FString Path) {
#if WITH_SERVER_CODE
	if (*&DB!=nullptr) {DB_CloseDatabase(*&DB); *&DB=nullptr;}
	if (sqlite3_open(TCHAR_TO_UTF8(*Path),&DB)==SQLITE_OK) {return true;}
#endif
	return false;
}

bool USQL::DB_CloseDatabase(sqlite3* DB) {
#if WITH_SERVER_CODE
	if (DB==nullptr) {return true;}
	if (sqlite3_close(DB)==SQLITE_OK) {return true;}
#endif
	return false;
}

ESQLResult USQL::DB_Execute(sqlite3* DB, const FString File, const FString Query) {
#if WITH_SERVER_CODE
	char* ErrorMSG = nullptr;
	auto Result = ESQLResult::BUSY;
	//
	if (DB_OpenDatabase(DB,File)) {
		DBT_BEGIN(DB,ErrorMSG);
		Result = static_cast<ESQLResult>(sqlite3_exec(DB,TCHAR_TO_UTF8(*Query),nullptr,nullptr,&ErrorMSG));
		DBT_END(DB,ErrorMSG);
		//
		if (Result!=ESQLResult::OK) {LOG_DB(Logs,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
	DB_CloseDatabase(DB);} return Result;
#endif
	return ESQLResult::ABORT;
}

void USQL::DB_AddToQueue(const FString Query, TSet<FString> &QUEUE) {
	if (!QUEUE.Contains(Query)) {QUEUE.Add(Query);}
}

void USQL::DB_RemoveFromQueue(const FString Query, TSet<FString> &QUEUE) {
	QUEUE.Remove(Query);
	QUEUE.CompactStable();
	QUEUE.Shrink();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USQL::DB_Save(USQLite* Database) {
#if WITH_SERVER_CODE
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	if ((DBS_Progress<100.f)&&(DBS_Complete>0)) {return;}
	//
	const UWorld* World = Database->GetWorld();
	if (World!=nullptr) {
		const ENetMode NET = World->GetNetMode();
		if (NET==NM_Client) {return;}
	}///
	//
	EVENT_OnBeginDataSAVE.Broadcast(Database);
	DBS_Workload = Database->DBS_QUEUE.Num();
	DBS_Progress = 0.f;
	DBS_Complete = 0.f;
	//
	switch (Database->DB_MODE) {
		case ESQLRuntimeMode::SynchronousTask:
		{
			DBS_ExecuteQueue(Database);
		}	break;
		case ESQLRuntimeMode::BackgroundTask:
		{
			LastThreadState = ThreadSafety;
			ThreadSafety = ESQLThreadSafety::AsynchronousSaving;
			(new FAutoDeleteAsyncTask<DBS_ExecuteQueue_Task>(Database))->StartBackgroundTask();
		}	break;
	default: break;}
#endif
}

void USQL::DB_ImmediateSave(USQLite* Database, const FString SQL) {
#if WITH_SERVER_CODE
	if (SQL.IsEmpty()) {LOG_DB(Logs,ESQLSeverity::Warning,TEXT("DBS Empty Command.")); return;}
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	//
	const UWorld* World = Database->GetWorld();
	if (World!=nullptr) {
		const ENetMode NET = World->GetNetMode();
		if (NET==NM_Client) {return;}
	}///
	//
	ESQLResult Result = ESQLResult::ABORT;
	char* ErrorMSG = nullptr;
	//
	if (DB_OpenDatabase(Database->DB_RUNTIME,Database->DB_FILE)) {
		DBT_BEGIN(Database->DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_exec(Database->DB_RUNTIME,TCHAR_TO_UTF8(*SQL),nullptr,nullptr,&ErrorMSG));
			if (Result!=ESQLResult::OK) {LOG_DB(Logs,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(Database->DB_RUNTIME,ErrorMSG);
	DB_CloseDatabase(Database->DB_RUNTIME);}
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TArray<FSQLRow> USQL::DB_Load__Internal(USQLite* Database) {
#if WITH_SERVER_CODE
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return TArray<FSQLRow>();}
	ThreadSafety = ESQLThreadSafety::IsPreparingToSaveOrLoad;
	return DBL_ExecuteQueue(Database,ESQLoadMode::Internal);
#endif
	//
	return TArray<FSQLRow>();
}

void USQL::DB_Load(USQLite* Database) {
#if WITH_SERVER_CODE
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	if ((DBL_Progress<100.f)&&(DBL_Complete>0)) {return;}
	//
	EVENT_OnBeginDataLOAD.Broadcast(Database);
	DBL_Workload = Database->DBL_QUEUE.Num();
	DBL_Progress = 0.f;
	DBL_Complete = 0.f;
	//
	switch (Database->DB_MODE) {
		case ESQLRuntimeMode::SynchronousTask:
		{
			DBL_ExecuteQueue(Database,ESQLoadMode::Runtime);
		}	break;
		case ESQLRuntimeMode::BackgroundTask:
		{
			LastThreadState = ThreadSafety;
			ThreadSafety = ESQLThreadSafety::AsynchronousLoading;
			(new FAutoDeleteAsyncTask<DBL_ExecuteQueue_Task>(Database))->StartBackgroundTask();
		}	break;
	default: break;}
#endif
}

void USQL::DB_ImmediateLoad(USQLite* Database, UObject* Outer, const FString SQL) {
#if WITH_SERVER_CODE
	if (SQL.IsEmpty()) {LOG_DB(Logs,ESQLSeverity::Warning,TEXT("DBS Empty Command.")); return;}
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	if (!Outer||!Outer->IsValidLowLevelFast()||Outer->IsPendingKill()) {return;}
	//
	ESQLResult Result = ESQLResult::ABORT;
	char* ErrorMSG = nullptr;
	//
	if (DB_OpenDatabase(Database->DB_RUNTIME,Database->DB_FILE)) {
		DBT_BEGIN(Database->DB_RUNTIME,ErrorMSG);
			DBSTM Statement;
			Result = static_cast<ESQLResult>(sqlite3_prepare(Database->DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			//
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						switch (Type) {
							case SQLITE_INTEGER:
							{
								Field.Type = ESQLType::Integer;
								Field.Cast = ESQLReadMode::Integer;
								Field.Integer = sqlite3_column_int64(Statement,Column);
							}	break;
							case SQLITE_FLOAT:
							{
								Field.Type = ESQLType::Float;
								Field.Cast = ESQLReadMode::Float;
								Field.Float = sqlite3_column_double(Statement,Column);
							}	break;
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("USet>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Set;}
								if (Value.Split(TEXT("UMap>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Map;}
								if (Value.Split(TEXT("UEnum>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Enum;}
								if (Value.Split(TEXT("UName>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Name;}
								if (Value.Split(TEXT("UText>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Text;}
								if (Value.Split(TEXT("UArray>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Array;}
								if (Value.Split(TEXT("UColor>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Color;}
								if (Value.Split(TEXT("UBool>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Boolean;}
								if (Value.Split(TEXT("UString>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::String;}
								if (Value.Split(TEXT("UStruct>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Struct;}
								if (Value.Split(TEXT("URotator>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Rotator;}
								if (Value.Split(TEXT("UObject>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::ObjectPtr;}
								if (Value.Split(TEXT("UVector2D>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Vector2D;}
								if (Value.Split(TEXT("UVector3D>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Vector3D;}
								if (Value.Split(TEXT("UFloatCurve>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::FloatCurve;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					Row.DB = Database->GetFName();
					Row.ID = FName(*Row.Values[0].ValueToString());
					if (Row.ID==*MakeObjectDBID(Outer)) {Database->DB_UnpackObjectDATA(Outer,Row,Database->Debug);}
				} else {
					const char* MSG = sqlite3_errmsg(Database->DB_RUNTIME);
				LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("SQL ERROR: %s"),ANSI_TO_TCHAR(MSG)));}
		} DBT_END(Database->DB_RUNTIME,ErrorMSG);
	DB_CloseDatabase(Database->DB_RUNTIME);}
#endif
}

void USQL::DB_ImmediateLoad(USQLite* Database, UActorComponent* Outer, const FString SQL) {
#if WITH_SERVER_CODE
	if (SQL.IsEmpty()) {LOG_DB(Logs,ESQLSeverity::Warning,TEXT("DBS Empty Command.")); return;}
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	if (!Outer||!Outer->IsValidLowLevelFast()||Outer->IsPendingKill()) {return;}
	//
	ESQLResult Result = ESQLResult::ABORT;
	char* ErrorMSG = nullptr;
	//
	if (DB_OpenDatabase(Database->DB_RUNTIME,Database->DB_FILE)) {
		DBT_BEGIN(Database->DB_RUNTIME,ErrorMSG);
			DBSTM Statement;
			Result = static_cast<ESQLResult>(sqlite3_prepare(Database->DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			//
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						switch (Type) {
							case SQLITE_INTEGER:
							{
								Field.Type = ESQLType::Integer;
								Field.Cast = ESQLReadMode::Integer;
								Field.Integer = sqlite3_column_int64(Statement,Column);
							}	break;
							case SQLITE_FLOAT:
							{
								Field.Type = ESQLType::Float;
								Field.Cast = ESQLReadMode::Float;
								Field.Float = sqlite3_column_double(Statement,Column);
							}	break;
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("USet>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Set;}
								if (Value.Split(TEXT("UMap>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Map;}
								if (Value.Split(TEXT("UEnum>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Enum;}
								if (Value.Split(TEXT("UName>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Name;}
								if (Value.Split(TEXT("UText>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Text;}
								if (Value.Split(TEXT("UArray>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Array;}
								if (Value.Split(TEXT("UColor>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Color;}
								if (Value.Split(TEXT("UBool>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Boolean;}
								if (Value.Split(TEXT("UString>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::String;}
								if (Value.Split(TEXT("UStruct>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Struct;}
								if (Value.Split(TEXT("URotator>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Rotator;}
								if (Value.Split(TEXT("UObject>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::ObjectPtr;}
								if (Value.Split(TEXT("UVector2D>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Vector2D;}
								if (Value.Split(TEXT("UVector3D>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Vector3D;}
								if (Value.Split(TEXT("UFloatCurve>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::FloatCurve;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					Row.DB = Database->GetFName();
					Row.ID = FName(*Row.Values[0].ValueToString());
					if (Row.ID==*MakeComponentDBID(Outer)) {Database->DB_UnpackComponentDATA(Outer,Row,Database->Debug);}
				} else {
					const char* MSG = sqlite3_errmsg(Database->DB_RUNTIME);
				LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("SQL ERROR: %s"),ANSI_TO_TCHAR(MSG)));}
		} DBT_END(Database->DB_RUNTIME,ErrorMSG);
	DB_CloseDatabase(Database->DB_RUNTIME);}
#endif
}

void USQL::DB_ImmediateLoad(USQLite* Database, AActor* Outer, const FString SQL) {
#if WITH_SERVER_CODE
	if (SQL.IsEmpty()) {LOG_DB(Logs,ESQLSeverity::Warning,TEXT("DBS Empty Command.")); return;}
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	if (!Outer||!Outer->IsValidLowLevelFast()||Outer->IsPendingKill()) {return;}
	//
	ESQLResult Result = ESQLResult::ABORT;
	char* ErrorMSG = nullptr;
	//
	if (DB_OpenDatabase(Database->DB_RUNTIME,Database->DB_FILE)) {
		DBT_BEGIN(Database->DB_RUNTIME,ErrorMSG);
			DBSTM Statement;
			Result = static_cast<ESQLResult>(sqlite3_prepare(Database->DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			//
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						switch (Type) {
							case SQLITE_INTEGER:
							{
								Field.Type = ESQLType::Integer;
								Field.Cast = ESQLReadMode::Integer;
								Field.Integer = sqlite3_column_int64(Statement,Column);
							}	break;
							case SQLITE_FLOAT:
							{
								Field.Type = ESQLType::Float;
								Field.Cast = ESQLReadMode::Float;
								Field.Float = sqlite3_column_double(Statement,Column);
							}	break;
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("USet>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Set;}
								if (Value.Split(TEXT("UMap>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Map;}
								if (Value.Split(TEXT("UEnum>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Enum;}
								if (Value.Split(TEXT("UName>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Name;}
								if (Value.Split(TEXT("UText>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Text;}
								if (Value.Split(TEXT("UArray>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Array;}
								if (Value.Split(TEXT("UColor>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Color;}
								if (Value.Split(TEXT("UBool>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Boolean;}
								if (Value.Split(TEXT("UString>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::String;}
								if (Value.Split(TEXT("UStruct>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Struct;}
								if (Value.Split(TEXT("URotator>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Rotator;}
								if (Value.Split(TEXT("UObject>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::ObjectPtr;}
								if (Value.Split(TEXT("UVector2D>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Vector2D;}
								if (Value.Split(TEXT("UVector3D>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Vector3D;}
								if (Value.Split(TEXT("UFloatCurve>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::FloatCurve;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					Row.DB = Database->GetFName();
					Row.ID = FName(*Row.Values[0].ValueToString());
					if (Row.ID==*MakeActorDBID(Outer)) {Database->DB_UnpackActorDATA(Outer,Row,Database->Debug);}
				} else {
					const char* MSG = sqlite3_errmsg(Database->DB_RUNTIME);
				LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("SQL ERROR: %s"),ANSI_TO_TCHAR(MSG)));}
		} DBT_END(Database->DB_RUNTIME,ErrorMSG);
	DB_CloseDatabase(Database->DB_RUNTIME);}
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USQL::DBS_ExecuteQueue(USQLite* Database) {
#if WITH_SERVER_CODE
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	if (USQL::ThreadSafety!=ESQLThreadSafety::IsPreparingToSaveOrLoad) {return;}
	ESQLResult Result = ESQLResult::ABORT;
	char* ErrorMSG = nullptr;
	//
	if (DB_OpenDatabase(Database->DB_RUNTIME,Database->DB_FILE)) {
		DB_CACHE(Database->DB_RUNTIME,TEXT("PRAGMA cache_size=1024"));
		DBT_BEGIN(Database->DB_RUNTIME,ErrorMSG);
		//
		for (auto &SQL : Database->DBS_QUEUE.Array()) {
			if (SQL.IsEmpty()) {
				DBS_Complete++;
				DB_RemoveFromQueue(SQL,Database->DBS_QUEUE);
				DBS_Progress = ((DBS_Complete/DBS_Workload)*100);
				LOG_DB(Logs,ESQLSeverity::Warning,TEXT("DBS Empty Command, skipping..."));
			continue;}
			Result = static_cast<ESQLResult>(sqlite3_exec(Database->DB_RUNTIME,TCHAR_TO_UTF8(*SQL),nullptr,nullptr,&ErrorMSG));
			if (Result==ESQLResult::OK) {
				DBS_Complete++;
				DB_RemoveFromQueue(SQL,Database->DBS_QUEUE);
				DBS_Progress = ((DBS_Complete/DBS_Workload)*100);
				EVENT_OnProgressDataSAVE.Broadcast(Database,DBS_Progress);
				LOG_DB(Logs,ESQLSeverity::Info,FString::Printf(TEXT("SAVE PROGRESS::  %4.2f"),DBS_Progress));
			} else {
				DBS_Complete++;
				DB_RemoveFromQueue(SQL,Database->DBS_QUEUE);
				DBS_Progress = ((DBS_Complete/DBS_Workload)*100);
				LOG_DB(Logs,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));
			continue;}
	} DBT_END(Database->DB_RUNTIME,ErrorMSG);
	DB_CloseDatabase(Database->DB_RUNTIME);}
	//
	LastThreadState = ThreadSafety;
	ThreadSafety = ESQLThreadSafety::IsCurrentlyThreadSafe;
	EVENT_OnFinishDataSAVE.Broadcast(Database,Result==ESQLResult::OK);
	if (Database->DBS_QUEUE.Array().Num()==0) {DBS_Workload=0; DBS_Complete=0; DBS_Progress=100.f;}
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TArray<FSQLRow> USQL::DBL_ExecuteQueue(USQLite* Database, ESQLoadMode LoadMode) {
#if WITH_SERVER_CODE
	TArray<FSQLRow>Data; char* ErrorMSG = nullptr;
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return Data;}
	if ((USQL::ThreadSafety!=ESQLThreadSafety::IsPreparingToSaveOrLoad)&&(LoadMode==ESQLoadMode::Runtime)) {return Data;}
	//
	ESQLResult Result = ESQLResult::ABORT;
	if (DB_OpenDatabase(Database->DB_RUNTIME,Database->DB_FILE)) {
		DB_CACHE(Database->DB_RUNTIME,TEXT("PRAGMA cache_size=1024"));
		DBT_BEGIN(Database->DB_RUNTIME,ErrorMSG);
		for (const auto &SQL : Database->DBL_QUEUE.Array()) {
			if (SQL.IsEmpty()) {
				DBL_Complete++;
				DB_RemoveFromQueue(SQL,Database->DBL_QUEUE);
				DBL_Progress = ((DBL_Complete/DBL_Workload)*100);
				LOG_DB(Logs,ESQLSeverity::Warning,TEXT("DBL Empty Command, skipping..."));
			continue;} DBSTM Statement;
			//
			Result = static_cast<ESQLResult>(sqlite3_prepare(Database->DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				DB_RemoveFromQueue(SQL,Database->DBL_QUEUE);
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						switch (Type) {
							case SQLITE_INTEGER:
							{
								Field.Type = ESQLType::Integer;
								Field.Cast = ESQLReadMode::Integer;
								Field.Integer = sqlite3_column_int64(Statement,Column);
							}	break;
							case SQLITE_FLOAT:
							{
								Field.Type = ESQLType::Float;
								Field.Cast = ESQLReadMode::Float;
								Field.Float = sqlite3_column_double(Statement,Column);
							}	break;
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("USet>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Set;}
								if (Value.Split(TEXT("UMap>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Map;}
								if (Value.Split(TEXT("UEnum>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Enum;}
								if (Value.Split(TEXT("UName>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Name;}
								if (Value.Split(TEXT("UText>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Text;}
								if (Value.Split(TEXT("UArray>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Array;}
								if (Value.Split(TEXT("UColor>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Color;}
								if (Value.Split(TEXT("UBool>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Boolean;}
								if (Value.Split(TEXT("UString>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::String;}
								if (Value.Split(TEXT("UStruct>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Struct;}
								if (Value.Split(TEXT("URotator>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Rotator;}
								if (Value.Split(TEXT("UObject>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::ObjectPtr;}
								if (Value.Split(TEXT("UVector2D>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Vector2D;}
								if (Value.Split(TEXT("UVector3D>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Vector3D;}
								if (Value.Split(TEXT("UFloatCurve>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::FloatCurve;}
								Field.Text = Value;
							}	break;
							case SQLITE_BLOB:
							case SQLITE_NULL:
							{
								#if WITH_EDITOR
								Field.Cast=ESQLReadMode::String;
								Field.Type = ESQLType::Text;
								Field.Text = TEXT("NULL");
								#endif
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
					//
					#if WITH_EDITOR
					if (Field.Type!=ESQLType::Unknown) {
						Row.PreviewValues.Add(Field.ValueToString());
					} 	Row.PreviewTypes.Add(Field.Name.ToString());
					#endif
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					Row.ID = FName(*Row.Values[0].ValueToString());
					Row.DB = Database->GetFName(); Data.Add(Row);
					//
					DBL_Complete++;
					DBL_Progress = ((DBL_Complete/DBL_Workload)*100);
					if (LoadMode==ESQLoadMode::Runtime) {EVENT_OnProgressDataLOAD.Broadcast(Database,DBL_Progress,Row);}
					//
					for (const auto &VAL : Row.Values) {
						LOG_DB(Logs,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
						switch (VAL.Type) {
							case ESQLType::Integer:
								LOG_DB(Logs,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %i"),VAL.Integer)); break;
							case ESQLType::Float:
								LOG_DB(Logs,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %f"),VAL.Float)); break;
							case ESQLType::Text:
								LOG_DB(Logs,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text)); break;
					default: break;}}
				LOG_DB(Logs,ESQLSeverity::Info,FString::Printf(TEXT("LOAD PROGRESS::  %4.2f"),DBL_Progress));}
			} else {
				DBL_Complete++;
				DB_RemoveFromQueue(SQL,Database->DBL_QUEUE);
				DBL_Progress = ((DBL_Complete/DBL_Workload)*100);
				const char* MSG = sqlite3_errmsg(Database->DB_RUNTIME);
				LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("SQL ERROR: %s"),ANSI_TO_TCHAR(MSG)));
			continue;}
		} DBT_END(Database->DB_RUNTIME,ErrorMSG);
	DB_CloseDatabase(Database->DB_RUNTIME);}
	//
	LastThreadState = ThreadSafety;
	ThreadSafety = ESQLThreadSafety::IsCurrentlyThreadSafe;
	if (Database->DBL_QUEUE.Array().Num()==0) {DBL_Workload=0; DBL_Complete=0; DBL_Progress=100.f;}
	if (LoadMode==ESQLoadMode::Runtime) {EVENT_OnFinishDataLOAD.Broadcast(Database,Result==ESQLResult::OK);}
	//
	return Data;
#endif
	//
	return TArray<FSQLRow>();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USQL::DBS_ExecuteQueue__Threaded(USQLite* Database) {
#if WITH_SERVER_CODE
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	ESQLResult Result = ESQLResult::ABORT;
	char* ErrorMSG = nullptr;
	//
	if (DB_OpenDatabase(Database->DB_RUNTIME,Database->DB_FILE)) {
		DB_CACHE(Database->DB_RUNTIME,TEXT("PRAGMA cache_size=1024"));
		DBT_BEGIN(Database->DB_RUNTIME,ErrorMSG);
		//
		for (auto &SQL : Database->DBS_QUEUE.Array()) {
			if (SQL.IsEmpty()) {
				DBS_Complete++;
				DB_RemoveFromQueue(SQL,Database->DBS_QUEUE);
				DBS_Progress = ((DBS_Complete/DBS_Workload)*100);
			continue;}
			Result = static_cast<ESQLResult>(sqlite3_exec(Database->DB_RUNTIME,TCHAR_TO_UTF8(*SQL),nullptr,nullptr,&ErrorMSG));
			if (Result==ESQLResult::OK) {
				DBS_Complete++;
				DB_RemoveFromQueue(SQL,Database->DBS_QUEUE);
				DBS_Progress = ((DBS_Complete/DBS_Workload)*100);
				EVENT_ProgressReportSAVE.Broadcast(Database,DBS_Progress);
				Database->DB_OnProgressSave__Threaded(Database,DBS_Progress);
				LOG_DB(Logs,ESQLSeverity::Info,FString::Printf(TEXT("SAVE PROGRESS::  %4.2f"),DBS_Progress));
			} else {
				DBS_Complete++;
				DB_RemoveFromQueue(SQL,Database->DBS_QUEUE);
				DBS_Progress = ((DBS_Complete/DBS_Workload)*100);
				LOG_DB(Logs,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));
			continue;}
	} DBT_END(Database->DB_RUNTIME,ErrorMSG);
	DB_CloseDatabase(Database->DB_RUNTIME);}
	//
	LastThreadState = ThreadSafety;
	ThreadSafety = ESQLThreadSafety::IsCurrentlyThreadSafe;
	Database->DB_OnFinishSave__Threaded(Database,Result==ESQLResult::OK);
	if (Database->DBS_QUEUE.Array().Num()==0) {DBS_Workload=0; DBS_Complete=0; DBS_Progress=100.f;}
#endif
}

void USQL::DBL_ExecuteQueue__Threaded(USQLite* Database) {
#if WITH_SERVER_CODE
	ESQLResult Result = ESQLResult::ABORT; char* ErrorMSG = nullptr;
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	//
	if (DB_OpenDatabase(Database->DB_RUNTIME,Database->DB_FILE)) {
		DB_CACHE(Database->DB_RUNTIME,TEXT("PRAGMA cache_size=1024"));
		DBT_BEGIN(Database->DB_RUNTIME,ErrorMSG);
		for (const auto &SQL : Database->DBL_QUEUE.Array()) {
			if (SQL.IsEmpty()) {
				DBL_Complete++;
				DB_RemoveFromQueue(SQL,Database->DBL_QUEUE);
				DBL_Progress = ((DBL_Complete/DBL_Workload)*100);
				EVENT_ProgressReportLOAD.Broadcast(Database,DBL_Progress);
			continue;} DBSTM Statement;
			//
			Result = static_cast<ESQLResult>(sqlite3_prepare(Database->DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				DB_RemoveFromQueue(SQL,Database->DBL_QUEUE);
				//
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						switch (Type) {
							case SQLITE_INTEGER:
							{
								Field.Type = ESQLType::Integer;
								Field.Cast = ESQLReadMode::Integer;
								Field.Integer = sqlite3_column_int64(Statement,Column);
							}	break;
							case SQLITE_FLOAT:
							{
								Field.Type = ESQLType::Float;
								Field.Cast = ESQLReadMode::Float;
								Field.Float = sqlite3_column_double(Statement,Column);
							}	break;
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("USet>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Set;}
								if (Value.Split(TEXT("UMap>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Map;}
								if (Value.Split(TEXT("UEnum>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Enum;}
								if (Value.Split(TEXT("UName>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Name;}
								if (Value.Split(TEXT("UText>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Text;}
								if (Value.Split(TEXT("UArray>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Array;}
								if (Value.Split(TEXT("UColor>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Color;}
								if (Value.Split(TEXT("UBool>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Boolean;}
								if (Value.Split(TEXT("UString>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::String;}
								if (Value.Split(TEXT("UStruct>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Struct;}
								if (Value.Split(TEXT("URotator>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Rotator;}
								if (Value.Split(TEXT("UObject>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::ObjectPtr;}
								if (Value.Split(TEXT("UVector2D>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Vector2D;}
								if (Value.Split(TEXT("UVector3D>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Vector3D;}
								if (Value.Split(TEXT("UFloatCurve>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::FloatCurve;}
								Field.Text = Value;
							}	break;
							case SQLITE_BLOB:
							case SQLITE_NULL:
							{
								#if WITH_EDITOR
								Field.Cast=ESQLReadMode::String;
								Field.Type = ESQLType::Text;
								Field.Text = TEXT("NULL");
								#endif
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
					//
					#if WITH_EDITOR
					if (Field.Type!=ESQLType::Unknown) {
						Row.PreviewValues.Add(Field.ValueToString());
					} 	Row.PreviewTypes.Add(Field.Name.ToString());
					#endif
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					Row.ID = FName(*Row.Values[0].ValueToString());
					Row.DB = Database->GetFName();
					//
					DBL_Complete++;
					DBL_Progress = ((DBL_Complete/DBL_Workload)*100);
					EVENT_ProgressReportLOAD.Broadcast(Database,DBL_Progress);
					Database->DB_OnProgressLoad__Threaded(Database,Row,DBL_Progress);
					//
					for (const auto &VAL : Row.Values) {
						LOG_DB(Logs,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
						switch (VAL.Type) {
							case ESQLType::Integer:
								LOG_DB(Logs,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %i"),VAL.Integer)); break;
							case ESQLType::Float:
								LOG_DB(Logs,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %f"),VAL.Float)); break;
							case ESQLType::Text:
								LOG_DB(Logs,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text)); break;
					default: break;}}
				LOG_DB(Logs,ESQLSeverity::Info,FString::Printf(TEXT("LOAD PROGRESS::  %4.2f"),DBL_Progress));}
			} else {
				DBL_Complete++;
				DB_RemoveFromQueue(SQL,Database->DBL_QUEUE);
				DBL_Progress = ((DBL_Complete/DBL_Workload)*100);
				const char* MSG = sqlite3_errmsg(Database->DB_RUNTIME);
				EVENT_ProgressReportLOAD.Broadcast(Database,DBL_Progress);
				LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("SQL ERROR: %s"),ANSI_TO_TCHAR(MSG)));
			continue;}
		} DBT_END(Database->DB_RUNTIME,ErrorMSG);
	DB_CloseDatabase(Database->DB_RUNTIME);}
	//
	LastThreadState = ThreadSafety;
	ThreadSafety = ESQLThreadSafety::IsCurrentlyThreadSafe;
	Database->DB_OnFinishLoad__Threaded(Database,Result==ESQLResult::OK);
	if (Database->DBL_QUEUE.Array().Num()==0) {DBL_Workload=0; DBL_Complete=0; DBL_Progress=100.f;}
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DB ID Interface:

bool USQL::GetPlayerNetworkID(const APlayerController* PlayerController, FString &ID, const bool AppendPort) {
	if (!PlayerController) {return false;}
	if (!PlayerController->PlayerState->IsValidLowLevelFast()) {return false;}
	if (!PlayerController->PlayerState->UniqueId.IsValid()) {return false;}
#if WITH_SERVER_CODE
	ID = PlayerController->PlayerState->UniqueId->ToString();
	if ((!ID.IsEmpty())&&(!AppendPort)) {ID.Split(FString(":"),&ID,nullptr,ESearchCase::IgnoreCase,ESearchDir::FromEnd);}
#endif
	return false;
}

///	(C) 2018 - Bruno Xavier B. Leite
FString USQL::MakeObjectDBID(UObject* OBJ) {
#if WITH_SERVER_CODE
	if (!OBJ||!OBJ->IsValidLowLevelFast()||OBJ->IsPendingKill()) {return TEXT("ERROR:ID");}
	const auto Owner = OBJ->GetTypedOuter<AActor>();
	//
	FString OwnerID = (Owner) ? Owner->GetFName().ToString()+FString(TEXT("_")) : TEXT("");
	FString ID = OwnerID+OBJ->GetFName().ToString();
	FString NetworkID = TEXT("");
	//
	if (Owner && Owner->GetNetOwningPlayer()) {
		if (!GetPlayerNetworkID(Owner->GetNetOwningPlayer()->GetPlayerController(Owner->GetWorld()),NetworkID,false)) {
			if (Owner->GetNetOwningPlayer()->GetPlayerController(Owner->GetWorld())) {
				ID = FString(TEXT("P"))+FString::FromInt(UGameplayStatics::GetPlayerControllerID(Owner->GetNetOwningPlayer()->GetPlayerController(Owner->GetWorld())))+(FString(TEXT("_"))+ID);
	}} else {ID = NetworkID+FString(TEXT("_"))+ID;}} return ID;
#endif
	return FString();
}

///	(C) 2018 - Bruno Xavier B. Leite
FString USQL::MakeActorDBID(AActor* Actor) {
#if WITH_SERVER_CODE
	if (!Actor||!Actor->IsValidLowLevelFast()||Actor->IsPendingKill()) {return TEXT("ERROR:ID");}
	//
	FString ID = Actor->GetFName().ToString();
	FString NetworkID = TEXT("");
	//
	if (Actor->GetNetOwningPlayer()) {
		if (!GetPlayerNetworkID(Actor->GetNetOwningPlayer()->GetPlayerController(Actor->GetWorld()),NetworkID,false)) {
			if (Actor->GetNetOwningPlayer()->GetPlayerController(Actor->GetWorld())) {
				ID = FString(TEXT("P"))+FString::FromInt(UGameplayStatics::GetPlayerControllerID(Actor->GetNetOwningPlayer()->GetPlayerController(Actor->GetWorld())))+(FString(TEXT("_"))+ID);
	}} else {ID = NetworkID+FString(TEXT("_"))+ID;}} return ID;
#endif
	return FString();
}

///	(C) 2018 - Bruno Xavier B. Leite
FString USQL::MakeComponentDBID(const UActorComponent* CMP) {
#if WITH_SERVER_CODE
	if (!CMP||!CMP->IsValidLowLevelFast()||CMP->IsPendingKill()) {return TEXT("ERROR:ID");}
	const auto &Owner = CMP->GetOwner();
	//
	FString OwnerID = (Owner) ? Owner->GetFName().ToString()+FString(TEXT("_")) : TEXT("");
	FString ID = OwnerID+CMP->GetFName().ToString();
	FString NetworkID = TEXT("");
	//
	if (Owner && Owner->GetNetOwningPlayer()) {
		if (!GetPlayerNetworkID(Owner->GetNetOwningPlayer()->GetPlayerController(Owner->GetWorld()),NetworkID,false)) {
			if (Owner->GetNetOwningPlayer()->GetPlayerController(Owner->GetWorld())) {
				ID = FString(TEXT("P"))+FString::FromInt(UGameplayStatics::GetPlayerControllerID(Owner->GetNetOwningPlayer()->GetPlayerController(Owner->GetWorld())))+(FString(TEXT("_"))+ID);
	}} else {ID = NetworkID+FString(TEXT("_"))+ID;}} return ID;
#endif
	return FString();
}

FString USQL::SanitizeString(const FString String) {
	auto STR = String.Replace(TEXT("'"),TEXT(""));
	//
	STR = STR.Replace(TEXT("||"),TEXT(""));
	STR = STR.Replace(TEXT("\\"),TEXT(""));
	STR = STR.Replace(TEXT("-"),TEXT(""));
	STR = STR.Replace(TEXT("*"),TEXT(""));
	STR = STR.Replace(TEXT("/"),TEXT(""));
	STR = STR.Replace(TEXT("<"),TEXT(""));
	STR = STR.Replace(TEXT(">"),TEXT(""));
	STR = STR.Replace(TEXT(","),TEXT(""));
	STR = STR.Replace(TEXT("="),TEXT(""));
	STR = STR.Replace(TEXT("~"),TEXT(""));
	STR = STR.Replace(TEXT("!"),TEXT(""));
	STR = STR.Replace(TEXT("^"),TEXT(""));
	STR = STR.Replace(TEXT("("),TEXT(""));
	STR = STR.Replace(TEXT(")"),TEXT(""));
	STR = STR.Replace(TEXT("["),TEXT(""));
	STR = STR.Replace(TEXT("]"),TEXT(""));
	STR = STR.Replace(TEXT("{"),TEXT(""));
	STR = STR.Replace(TEXT("}"),TEXT(""));
	STR = STR.Replace(TEXT(" "),TEXT(""));
	//
	return STR;
}

FString USQL::StringToMD5(const FString String) {
	return FMD5::HashAnsiString(*String);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LOG_DB(const bool Debug, const ESQLSeverity Severity, const FString Message) {
	if (!Debug) {return;}
	//
	switch (Severity) {
		case ESQLSeverity::Info:
			UE_LOG(USQLog,Log,TEXT("{USQL}:: %s"),*Message); break;
		case ESQLSeverity::Warning:
			UE_LOG(USQLog,Warning,TEXT("{USQL}:: %s"),*Message); break;
		case ESQLSeverity::Error:
			UE_LOG(USQLog,Error,TEXT("{USQL}:: %s"),*Message); break;
		case ESQLSeverity::Fatal:
			UE_LOG(USQLog,Fatal,TEXT("{USQL}:: %s"),*Message); break;
	default: break;}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////