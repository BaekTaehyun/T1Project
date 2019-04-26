//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "USQLite.h"

#include "Engine/Font.h"
#include "Modules/ModuleManager.h"
#include "UObject/ConstructorHelpers.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UObject Routines:

USQLite_Settings::USQLite_Settings() {
	LoadConfig();
}

USQLite::USQLite() {
#if WITH_SERVER_CODE
	USQL::EVENT_OnProgressDataSAVE.AddDynamic(this,&USQLite::DB_OnProgressSave);
	USQL::EVENT_OnProgressDataLOAD.AddDynamic(this,&USQLite::DB_OnProgressLoad);
	USQL::EVENT_OnFinishDataSAVE.AddDynamic(this,&USQLite::DB_OnFinishSave);
	USQL::EVENT_OnFinishDataLOAD.AddDynamic(this,&USQLite::DB_OnFinishLoad);
	USQL::EVENT_OnBeginDataSAVE.AddDynamic(this,&USQLite::DB_OnBeginSave);
	USQL::EVENT_OnBeginDataLOAD.AddDynamic(this,&USQLite::DB_OnBeginLoad);
	//
	USQL::EVENT_ProgressReportSAVE.AddDynamic(this,&USQLite::DB_DoSaveProgressReport);
	USQL::EVENT_ProgressReportLOAD.AddDynamic(this,&USQLite::DB_DoLoadProgressReport);
#endif
	//
	if (GetWorld()&&(GetWorld()->GetNetMode()!=NM_DedicatedServer)&&(!FeedbackFont.HasValidFont())) {
		static ConstructorHelpers::FObjectFinder<UFont>FFontOBJ(TEXT("/Engine/EngineFonts/Roboto"));
		if (FFontOBJ.Succeeded()) {
			FeedbackFont = FSlateFontInfo(FFontOBJ.Object,34,FName("Bold"));
		}///
	}///
	//
	FeedbackLOAD = FText::FromString(TEXT("LOADING"));
	FeedbackSAVE = FText::FromString(TEXT("SAVING"));
	ProgressBarTint = FLinearColor::White;
	SplashStretch = EStretch::Fill;
	ProgressBarOnMovie = true;
	PauseGameOnLoad = false;
	BackBlurPower = 10.f;
	//
	//
	const auto &Settings = GetMutableDefault<USQLite_Settings>();
	USQL::Logs = Settings->DeepLogs;
	DB_RUNTIME = nullptr;
	DB_FILE.Empty();
	//
	if (DB_TABLE.Columns.Num()==0) {
		DB_TABLE.Columns.Add(TEXT("ID"));
		DB_TABLE.ColumnTypes.Add(USQL::USQLTypes.Texts+TEXT(" PRIMARY KEY NOT NULL"));
	}///
	//
	SetFlagsTo(GetFlags()|RF_MarkAsRootSet); 
	GetWorld();
	//
	//
#if UE_BUILD_SHIPPING
	USQL::Logs = false;
#endif
}

void USQLite::PostLoad() {
	Super::PostLoad();
	//
	DB_Setup();
	//
#if WITH_EDITOR
	DB_RefreshPreview();
#endif
}

void USQLite::DB_Setup() {
#if WITH_SERVER_CODE
	const auto &Settings = GetMutableDefault<USQLite_Settings>();
	if (HasAnyFlags(RF_ArchetypeObject)) {return;}
	//
	if (DB_TABLE.Columns.Num()==0) {
		DB_TABLE.Columns.Add(TEXT("ID"));
		DB_TABLE.ColumnTypes.Add(USQL::USQLTypes.Texts+TEXT(" PRIMARY KEY NOT NULL"));
	LOG_DB(true,ESQLSeverity::Error,TEXT("DB Table should never be empty prior to package cooking!"));}
	//
	DB_FILE = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()+GetNameSanitized()+FString(TEXT(".db")));
	DB_TABLE.Header = FString::Printf(TEXT("CREATE TABLE IF NOT EXISTS %s ("),*GetNameSanitized());
	DB_TABLE.Footer = FString(TEXT(");"));
	//
	if (DBS_UpdateCondition.IsEmpty()) {DBS_UpdateCondition = TEXT("WHERE {Key} = {ID} AND {TimeStamp} <= DATETIME('now','localtime')");}
	if (DBL_SelectCondition.IsEmpty()) {DBL_SelectCondition = TEXT("FROM {Table} WHERE {Key} = {ID}");}
	//
	ESQLResult Result = ESQLResult::EMPTY;
	Result = USQL::DB_Execute(DB_RUNTIME,DB_FILE,DB_CreateTableSQL());
	if (Result==ESQLResult::OK) {LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[%s] DB Table : OK"),*GetNameSanitized()));}
	else {LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] DB :: ERROR initializing Table:"),*GetNameSanitized()));}
	//
	for (auto VS : DB_VERSIONS) {
		FString Header = FString::Printf(TEXT("CREATE TABLE IF NOT EXISTS %s ("),*((GetNameSanitized()+TEXT("_"))+VS));
		Result = USQL::DB_Execute(DB_RUNTIME,DB_FILE,DB_CreateTableSQL(Header));
		if (Result==ESQLResult::OK) {LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[%s] DB Table : OK"),*((GetNameSanitized()+TEXT("_"))+VS)));}
		else {LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] DB :: ERROR initializing Table:"),*((GetNameSanitized()+TEXT("_"))+VS)));}
	}///
	//
	check((DB_TABLE.ColumnTypes.Num()==DB_TABLE.Columns.Num()));
#endif
	//
	DBS_QUEUE.Empty();
	DBL_QUEUE.Empty();
}

void USQLite::BeginDestroy() {
	Super::BeginDestroy();
	//
	DB_TABLE.ColumnTypes.Empty();
	DB_TABLE.Columns.Empty();
	DBS_QUEUE.Empty();
	DBL_QUEUE.Empty();
	//
	if (DB_RUNTIME!=nullptr) {USQL::DB_CloseDatabase(DB_RUNTIME);}
}

UWorld* USQLite::GetWorld() const {
	if (World!=nullptr) {return World;}
	//
	const auto Owner = GetTypedOuter<UObject>();
	if (Owner) {return Owner->GetWorld();}
	//
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
//
void USQLite::DB_RefreshPreview() {
	if ((USQL::GetLoadProgress()<100.f)&&(USQL::GetLoadsDone()>0)) {return;}
	const auto &Settings = GetMutableDefault<USQLite_Settings>();
	USQL::Logs = false; bool Debugging = Debug; Debug = false;
	//
	DBL_QUEUE.Empty();
	for (TObjectIterator<UObject>Object; Object; ++Object) {
		if ((*Object)->HasAnyFlags(RF_ArchetypeObject)) {continue;}
		//
		const auto &Interface = Cast<ISQLiteData>((*Object));
		if (Interface||(*Object)->GetClass()->ImplementsInterface(USQLiteData::StaticClass())) {
			const auto SQL = DB_GenerateSQL_Object_SELECT((*Object));
		DB_EnqueueLOAD(SQL);}
	}///
	//
	DB_DataPreview.Rows.Empty();
	auto Data = USQL::DB_Load__Internal(const_cast<USQLite*>(this));
	for (const auto Record : Data) {DB_DataPreview.Rows.Add(Record);}
	//
	if (DB_DataPreview.Rows.Num()==0) {
		FSQLRow NoData;
		 NoData.DB = GetFName();
		 NoData.ID = FName(TEXT("Preview"));
		 NoData.PreviewTypes = DB_TABLE.Columns;
		 for (auto V : NoData.PreviewTypes) {NoData.PreviewValues.Add(TEXT("NULL"));}
		 NoData.PreviewValues[0] = TEXT("Preview");
		DB_DataPreview.Rows.Add(NoData);
	}///
	//
	USQL::Logs = Settings->DeepLogs;
	Debug = Debugging;
}///
//
void USQLite::DB_UpsertPreview(const FString SQL) {
	if (SQL.IsEmpty()) {LOG_DB(true,ESQLSeverity::Warning,TEXT("SQL Empty Command.")); return;}
	//
	ESQLResult Result = ESQLResult::ABORT;
	char* ErrorMSG = nullptr;
	//
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_exec(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),nullptr,nullptr,&ErrorMSG));
			if (Result!=ESQLResult::OK) {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
		//
		USQL::DB_CloseDatabase(DB_RUNTIME);
	}///
	//
	DB_RefreshPreview();
}///
//
const bool USQLite::DB_IsPreviewDirty() {
	const bool IsPreviewDirty = (DB_DataPreview.Rows.Num()>0);
	if (IsPreviewDirty) {DB_DataPreview.Rows.Empty();}
	//
	return IsPreviewDirty;
}///
//
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DB Class Methods:

FString USQLite::GetNameSanitized() const {
	auto Name = GetName().Replace(TEXT("'"),TEXT(""));
	Name = Name.Replace(TEXT("||"),TEXT(""));
	Name = Name.Replace(TEXT("-"),TEXT(""));
	Name = Name.Replace(TEXT("*"),TEXT(""));
	Name = Name.Replace(TEXT("/"),TEXT(""));
	Name = Name.Replace(TEXT("<"),TEXT(""));
	Name = Name.Replace(TEXT(">"),TEXT(""));
	Name = Name.Replace(TEXT(","),TEXT(""));
	Name = Name.Replace(TEXT("="),TEXT(""));
	Name = Name.Replace(TEXT("~"),TEXT(""));
	Name = Name.Replace(TEXT("!"),TEXT(""));
	Name = Name.Replace(TEXT("^"),TEXT(""));
	Name = Name.Replace(TEXT("("),TEXT(""));
	Name = Name.Replace(TEXT(")"),TEXT(""));
	Name = Name.Replace(TEXT("["),TEXT(""));
	Name = Name.Replace(TEXT("]"),TEXT(""));
	Name = Name.Replace(TEXT("{"),TEXT(""));
	Name = Name.Replace(TEXT("}"),TEXT(""));
	//
	return Name;
}

FString USQLite::DB_CreateTableSQL() {
	FString SQL, Query;
#if WITH_SERVER_CODE
	for (int32 I=0; I<DB_TABLE.Columns.Num(); ++I) {
		Query += FString::Printf(TEXT("%s %s,"),*DB_TABLE.Columns[I],*USQL::USQLTypes.Downcast(DB_TABLE.ColumnTypes[I],false));
	} SQL += DB_TABLE.Header + Query + DB_TABLE.Footer; SQL = SQL.Replace(TEXT(",)"),TEXT(")"));
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
#endif
	return SQL;
}

FString USQLite::DB_CreateTableSQL(const FString Header) {
	FString SQL, Query;
#if WITH_SERVER_CODE
	for (int32 I=0; I<DB_TABLE.Columns.Num(); ++I) {
		Query += FString::Printf(TEXT("%s %s,"),*DB_TABLE.Columns[I],*USQL::USQLTypes.Downcast(DB_TABLE.ColumnTypes[I],false));
	} SQL += Header + Query + DB_TABLE.Footer; SQL = SQL.Replace(TEXT(",)"),TEXT(")"));
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
#endif
	return SQL;
}

FString USQLite::DB_GetMappedColumnID(TSubclassOf<UObject> Class, const FString ColumnID) const {
	auto Map = DB_REDIRECTORS.Find(Class);
	//
	if (Map!=nullptr) {
		auto Redirectors = (*Map).Redirectors;
		if (Redirectors.Contains(*ColumnID)) {
			return Redirectors.Find(*ColumnID)->ToString();
		}///
	}///
	//
	return ColumnID;
}

FString USQLite::DB_GenerateSQL_Object_INSERT(UObject* Object) {
#if WITH_SERVER_CODE
	if (!Object||!Object->IsValidLowLevelFast()||Object->IsPendingKill()) {LOG_DB(Debug,ESQLSeverity::Warning,TEXT("INSERT ERROR : OBJECT")); return FString();}
	if (DB_TABLE.Columns.Num()<=0) {LOG_DB(Debug,ESQLSeverity::Warning,TEXT("INSERT ERROR : NO PRIMARY KEY")); return FString();}
	if (Object->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {return FString();}
	check(DB_TABLE.ColumnTypes.Num()==DB_TABLE.Columns.Num());
	//
	FString ID;
	if (Object->IsA(UActorComponent::StaticClass())) {ID = FString::Printf(TEXT("'%s'"),*USQL::MakeComponentDBID(CastChecked<UActorComponent>(Object)));} else
	if (Object->IsA(AActor::StaticClass())) {ID = FString::Printf(TEXT("'%s'"),*USQL::MakeActorDBID(CastChecked<AActor>(Object)));}
	else {ID = FString::Printf(TEXT("'%s'"),*USQL::MakeObjectDBID(Object));}
	if (ID.IsEmpty()) {return TEXT("ERROR:NOID");}
	//
	FString SQL, Header, Values;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header = FString::Printf(TEXT("INSERT OR IGNORE INTO %s ("),*GetNameSanitized());}
	else {Header = FString::Printf(TEXT("INSERT OR IGNORE INTO %s ("),*((GetNameSanitized()+TEXT("_"))+DB_VERSION.CurrentVersion));}
	//
	for (int32 I=0; I<DB_TABLE.Columns.Num(); ++I) {
		Header += FString::Printf(TEXT("%s,"),*DB_TABLE.Columns[I]);
	} Header += TEXT(") "); SQL = SQL.Replace(TEXT(",)"),TEXT(")"));
	//
	Values = FString(TEXT("VALUES (")); Values += (ID+TEXT(","));
	//
	for (int32 I=1; I<DB_TABLE.ColumnTypes.Num(); ++I) {
		Values += DBNULL;
		//
		for (TFieldIterator<UProperty>PIT(Object->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
			UProperty* Property = *PIT; const bool IsSaveGame = Property->HasAnyPropertyFlags(CPF_SaveGame);
			auto ColumnID = DB_GetMappedColumnID(Object->GetClass(),DB_TABLE.Columns[I]);
			if (!Property->GetName().Equals(ColumnID,ESearchCase::IgnoreCase)) {continue;}
			if (!IsSaveGame) {continue;}
			//
			const bool IsSet = Property->IsA(USetProperty::StaticClass());
			const bool IsMap = Property->IsA(UMapProperty::StaticClass());
			const bool IsInt = Property->IsA(UIntProperty::StaticClass());
			const bool IsBool = Property->IsA(UBoolProperty::StaticClass());
			const bool IsByte = Property->IsA(UByteProperty::StaticClass());
			const bool IsEnum = Property->IsA(UEnumProperty::StaticClass());
			const bool IsName = Property->IsA(UNameProperty::StaticClass());
			const bool IsText = Property->IsA(UTextProperty::StaticClass());
			const bool IsString = Property->IsA(UStrProperty::StaticClass());
			const bool IsArray = Property->IsA(UArrayProperty::StaticClass());
			const bool IsFloat = Property->IsA(UFloatProperty::StaticClass());
			const bool IsStruct = Property->IsA(UStructProperty::StaticClass());
			const bool IsObject = Property->IsA(UObjectProperty::StaticClass());
			//
			if ((IsSet)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Sets)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<USetProperty>(Property),Object)+TEXT(","); continue;}
			if ((IsMap)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Maps)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UMapProperty>(Property),Object)+TEXT(","); continue;}
			if ((IsInt)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Ints)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UIntProperty>(Property),Object)+TEXT(","); continue;}
			if ((IsBool)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Bools)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UBoolProperty>(Property),Object)+TEXT(","); continue;}
			if ((IsByte)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Bytes)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UByteProperty>(Property),Object)+TEXT(","); continue;}
			if ((IsEnum)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Enums)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UEnumProperty>(Property),Object)+TEXT(","); continue;}
			if ((IsName)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Names)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UNameProperty>(Property),Object)+TEXT(","); continue;}
			if ((IsText)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Texts)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UTextProperty>(Property),Object)+TEXT(","); continue;}
			if ((IsArray)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Arrays)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UArrayProperty>(Property),Object)+TEXT(","); continue;}
			if ((IsFloat)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Floats)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UFloatProperty>(Property),Object)+TEXT(","); continue;}
			if ((IsString)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Strings)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UStrProperty>(Property),Object,ESQLWriteMode::String)+TEXT(","); continue;}
			if ((IsStruct)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Colors)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UStructProperty>(Property),Object,ESQLWriteMode::Color)+TEXT(","); continue;}
			if ((IsStruct)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Structs)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UStructProperty>(Property),Object,ESQLWriteMode::Struct)+TEXT(","); continue;}
			if ((IsStruct)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Rotators)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UStructProperty>(Property),Object,ESQLWriteMode::Rotator)+TEXT(","); continue;}
			if ((IsStruct)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Vector2D)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UStructProperty>(Property),Object,ESQLWriteMode::Vector2D)+TEXT(","); continue;}
			if ((IsStruct)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Vector3D)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UStructProperty>(Property),Object,ESQLWriteMode::Vector3D)+TEXT(","); continue;}
			if ((IsString)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.TimeStamps)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UStrProperty>(Property),Object,ESQLWriteMode::TimeStamp)+TEXT(","); continue;}
			if ((IsStruct)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.DateTimes)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UStructProperty>(Property),Object,ESQLWriteMode::DateTime)+TEXT(","); continue;}
			if ((IsObject)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.ObjectPtrs)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UObjectProperty>(Property),Object,ESQLWriteMode::ObjectPtr)+TEXT(","); continue;}
		Property = nullptr;}
	}///
	//
	Values += FString(TEXT(");"));
	SQL = Header + Values;
	SQL = SQL.Replace(TEXT(",)"),TEXT(")"));
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL); return SQL;
#endif
	return FString();
}

FString USQLite::DB_GenerateSQL_Object_UPDATE(UObject* Object) {
#if WITH_SERVER_CODE
	if (!Object||!Object->IsValidLowLevelFast()||Object->IsPendingKill()) {LOG_DB(Debug,ESQLSeverity::Warning,TEXT("UPDATE ERROR : OBJECT")); return FString();}
	if (DB_TABLE.Columns.Num()<=0) {LOG_DB(Debug,ESQLSeverity::Warning,TEXT("UPDATE ERROR : NO PRIMARY KEY")); return FString();}
	if (Object->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {return FString();}
	check(DB_TABLE.ColumnTypes.Num()==DB_TABLE.Columns.Num());
	//
	FString ID;
	if (Object->IsA(UActorComponent::StaticClass())) {ID = FString::Printf(TEXT("'%s'"),*USQL::MakeComponentDBID(CastChecked<UActorComponent>(Object)));} else
	if (Object->IsA(AActor::StaticClass())) {ID = FString::Printf(TEXT("'%s'"),*USQL::MakeActorDBID(CastChecked<AActor>(Object)));}
	else {ID = FString::Printf(TEXT("'%s'"),*USQL::MakeObjectDBID(Object));}
	if (ID.IsEmpty()) {return TEXT("ERROR:NOID");}
	//
	FString SQL, Header, Values;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header = FString::Printf(TEXT("INSERT OR REPLACE INTO %s ("),*GetNameSanitized());}
	else {Header = FString::Printf(TEXT("INSERT OR REPLACE INTO %s ("),*((GetNameSanitized()+TEXT("_"))+DB_VERSION.CurrentVersion));}
	//
	for (int32 I=0; I<DB_TABLE.Columns.Num(); ++I) {
		Header += FString::Printf(TEXT("%s,"),*DB_TABLE.Columns[I]);
	} Header += TEXT(") "); SQL = SQL.Replace(TEXT(",)"),TEXT(")"));
	//
	Values = FString(TEXT("VALUES (")); Values += (ID+TEXT(","));
	//
	for (int32 I=1; I<DB_TABLE.ColumnTypes.Num(); ++I) {
		Values += DBNULL;
		//
		for (TFieldIterator<UProperty>PIT(Object->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
			UProperty* Property = *PIT; const bool IsSaveGame = Property->HasAnyPropertyFlags(CPF_SaveGame);
			auto ColumnID = DB_GetMappedColumnID(Object->GetClass(),DB_TABLE.Columns[I]);
			if (!Property->GetName().Equals(ColumnID,ESearchCase::IgnoreCase)) {continue;}
			if (!IsSaveGame) {continue;}
			//
			const bool IsSet = Property->IsA(USetProperty::StaticClass());
			const bool IsMap = Property->IsA(UMapProperty::StaticClass());
			const bool IsInt = Property->IsA(UIntProperty::StaticClass());
			const bool IsBool = Property->IsA(UBoolProperty::StaticClass());
			const bool IsByte = Property->IsA(UByteProperty::StaticClass());
			const bool IsEnum = Property->IsA(UEnumProperty::StaticClass());
			const bool IsName = Property->IsA(UNameProperty::StaticClass());
			const bool IsText = Property->IsA(UTextProperty::StaticClass());
			const bool IsString = Property->IsA(UStrProperty::StaticClass());
			const bool IsArray = Property->IsA(UArrayProperty::StaticClass());
			const bool IsFloat = Property->IsA(UFloatProperty::StaticClass());
			const bool IsStruct = Property->IsA(UStructProperty::StaticClass());
			const bool IsObject = Property->IsA(UObjectProperty::StaticClass());
			//
			if ((IsSet)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Sets)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<USetProperty>(Property),Object)+TEXT(","); continue;}
			if ((IsMap)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Maps)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UMapProperty>(Property),Object)+TEXT(","); continue;}
			if ((IsInt)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Ints)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UIntProperty>(Property),Object)+TEXT(","); continue;}
			if ((IsBool)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Bools)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UBoolProperty>(Property),Object)+TEXT(","); continue;}
			if ((IsByte)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Bytes)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UByteProperty>(Property),Object)+TEXT(","); continue;}
			if ((IsEnum)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Enums)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UEnumProperty>(Property),Object)+TEXT(","); continue;}
			if ((IsName)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Names)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UNameProperty>(Property),Object)+TEXT(","); continue;}
			if ((IsText)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Texts)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UTextProperty>(Property),Object)+TEXT(","); continue;}
			if ((IsArray)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Arrays)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UArrayProperty>(Property),Object)+TEXT(","); continue;}
			if ((IsFloat)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Floats)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UFloatProperty>(Property),Object)+TEXT(","); continue;}
			if ((IsString)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Strings)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UStrProperty>(Property),Object,ESQLWriteMode::String)+TEXT(","); continue;}
			if ((IsStruct)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Colors)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UStructProperty>(Property),Object,ESQLWriteMode::Color)+TEXT(","); continue;}
			if ((IsStruct)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Structs)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UStructProperty>(Property),Object,ESQLWriteMode::Struct)+TEXT(","); continue;}
			if ((IsStruct)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Rotators)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UStructProperty>(Property),Object,ESQLWriteMode::Rotator)+TEXT(","); continue;}
			if ((IsStruct)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Vector2D)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UStructProperty>(Property),Object,ESQLWriteMode::Vector2D)+TEXT(","); continue;}
			if ((IsStruct)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.Vector3D)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UStructProperty>(Property),Object,ESQLWriteMode::Vector3D)+TEXT(","); continue;}
			if ((IsString)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.TimeStamps)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UStrProperty>(Property),Object,ESQLWriteMode::TimeStamp)+TEXT(","); continue;}
			if ((IsStruct)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.DateTimes)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UStructProperty>(Property),Object,ESQLWriteMode::DateTime)+TEXT(","); continue;}
			if ((IsObject)&&(DB_TABLE.ColumnTypes[I]==USQL::USQLTypes.ObjectPtrs)) {Values.RemoveFromEnd(DBNULL); Values += USQL::UPropertyToSQL(CastChecked<UObjectProperty>(Property),Object,ESQLWriteMode::ObjectPtr)+TEXT(","); continue;}
		Property = nullptr;}
	}///
	//
	Values += FString(TEXT(");"));
	SQL = Header + Values;
	SQL = SQL.Replace(TEXT(",)"),TEXT(")"));
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL); return SQL;
#endif
	//
	return FString();
}

FString USQLite::DB_GenerateSQL_Object_SELECT(UObject* Object) {
#if WITH_SERVER_CODE
	if (!Object||!Object->IsValidLowLevelFast()||Object->IsPendingKill()) {LOG_DB(Debug,ESQLSeverity::Warning,TEXT("SELECT ERROR : OBJECT")); return FString();}
	if (DB_TABLE.Columns.Num()<=0) {LOG_DB(Debug,ESQLSeverity::Warning,TEXT("SELECT ERROR : NO PRIMARY KEY")); return FString();}
	if (Object->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {return FString();}
	check(DB_TABLE.ColumnTypes.Num()==DB_TABLE.Columns.Num());
	//
	FString ID;
	if (Object->IsA(UActorComponent::StaticClass())) {ID = FString::Printf(TEXT("'%s'"),*USQL::MakeComponentDBID(CastChecked<UActorComponent>(Object)));} else
	if (Object->IsA(AActor::StaticClass())) {ID = FString::Printf(TEXT("'%s'"),*USQL::MakeActorDBID(CastChecked<AActor>(Object)));}
	else {ID = FString::Printf(TEXT("'%s'"),*USQL::MakeObjectDBID(Object));}
	if (ID.IsEmpty()) {return TEXT("ERROR:NOID");}
	//
	FString SQL = FString(TEXT("SELECT "));
	FString Condition = DBL_SelectCondition;
	//
	for (int32 I=0; I<DB_TABLE.Columns.Num(); ++I) {
		SQL += (DB_TABLE.Columns[I]+TEXT(", "));
	}///
	//
	Condition = Condition.Replace(TEXT("{ID}"),*ID);
	Condition = Condition.Replace(TEXT("{Key}"),*DB_TABLE.Columns[0]);
	//
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Condition = Condition.Replace(TEXT("{Table}"),*GetNameSanitized());}
	else {Condition = Condition.Replace(TEXT("{Table}"),*((GetNameSanitized()+TEXT("_"))+DB_VERSION.CurrentVersion));}
	//
	SQL += Condition+TEXT(";");
	SQL = SQL.Replace(TEXT(", FROM"),TEXT(" FROM"));
	LOG_DB(Debug,ESQLSeverity::Info,SQL); return SQL;
#endif
	return FString();
}

FString USQLite::DB_GenerateSQL_Object_DELETE(UObject* Object) {
#if WITH_SERVER_CODE
	if (!Object||!Object->IsValidLowLevelFast()||Object->IsPendingKill()) {LOG_DB(Debug,ESQLSeverity::Warning,TEXT("DELETE ERROR : OBJECT")); return FString();}
	if (DB_TABLE.Columns.Num()<=0) {LOG_DB(Debug,ESQLSeverity::Warning,TEXT("DELETE ERROR : NO PRIMARY KEY")); return FString();}
	if (Object->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {return FString();}
	check(DB_TABLE.ColumnTypes.Num()==DB_TABLE.Columns.Num());
	//
	FString ID;
	if (Object->IsA(UActorComponent::StaticClass())) {ID = USQL::MakeComponentDBID(CastChecked<UActorComponent>(Object));} else
	if (Object->IsA(AActor::StaticClass())) {ID = USQL::MakeActorDBID(CastChecked<AActor>(Object));}
	else {ID = USQL::MakeObjectDBID(Object);} if (ID.IsEmpty()) {return TEXT("ERROR:NOID");}
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {
		SQL = FString::Printf(TEXT("DELETE FROM %s WHERE %s = '%s';"),*GetNameSanitized(),*DB_TABLE.Columns[0],*ID);
	} else {SQL = FString::Printf(TEXT("DELETE FROM %s WHERE %s = '%s';"),*((GetNameSanitized()+TEXT("_"))+DB_VERSION.CurrentVersion),*DB_TABLE.Columns[0],*ID);}
	LOG_DB(Debug,ESQLSeverity::Info,SQL); return SQL;
#endif
	return FString();
}

FString USQLite::DB_GenerateSQL_Actor_INSERT(AActor* Actor) {
#if WITH_SERVER_CODE
	return DB_GenerateSQL_Object_INSERT(Actor);
#endif
	return FString();
}

FString USQLite::DB_GenerateSQL_Actor_UPDATE(AActor* Actor) {
#if WITH_SERVER_CODE
	return DB_GenerateSQL_Object_UPDATE(Actor);
#endif
	return FString();
}

FString USQLite::DB_GenerateSQL_Actor_SELECT(AActor* Actor) {
#if WITH_SERVER_CODE
	return DB_GenerateSQL_Object_SELECT(Actor);
#endif
	return FString();
}

FString USQLite::DB_GenerateSQL_Actor_DELETE(AActor* Actor) {
#if WITH_SERVER_CODE
	return DB_GenerateSQL_Object_DELETE(Actor);
#endif
	return FString();
}

FString USQLite::DB_GenerateSQL_Component_INSERT(UActorComponent* Component) {
#if WITH_SERVER_CODE
	return DB_GenerateSQL_Object_INSERT(Component);
#endif
	return FString();
}

FString USQLite::DB_GenerateSQL_Component_UPDATE(UActorComponent* Component) {
#if WITH_SERVER_CODE
	return DB_GenerateSQL_Object_UPDATE(Component);
#endif
	return FString();
}

FString USQLite::DB_GenerateSQL_Component_SELECT(UActorComponent* Component) {
#if WITH_SERVER_CODE
	return DB_GenerateSQL_Object_SELECT(Component);
#endif
	return FString();
}

FString USQLite::DB_GenerateSQL_Component_DELETE(UActorComponent* Component) {
#if WITH_SERVER_CODE
	return DB_GenerateSQL_Object_DELETE(Component);
#endif
	return FString();
}

FString USQLite::DB_GetVersion() const {
	if (DB_VERSION.CurrentVersion.IsEmpty()) {return TEXT("DEFAULT");}
	return ((GetNameSanitized()+TEXT("_"))+DB_VERSION.CurrentVersion);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USQLite::DB_UnpackObjectDATA(UObject* Object, const FSQLRow Data, const bool Log) {
#if WITH_SERVER_CODE
	for (auto Field : Data.Values) {
		switch (Field.Type) {
			case ESQLType::Integer:
			{
				for (TFieldIterator<UProperty>PIT(Object->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
					UProperty* Property = *PIT;
					auto FieldID = DB_GetMappedColumnID(Object->GetClass(),Field.Name.ToString());
					if (!Property->GetName().Equals(FieldID,ESearchCase::IgnoreCase)) {continue;}
					//
					const bool IsInt = Property->IsA(UIntProperty::StaticClass());
					const bool IsByte = Property->IsA(UByteProperty::StaticClass());
					const bool IsDate = Property->IsA(UStructProperty::StaticClass());
					const bool IsSaveGame = Property->HasAnyPropertyFlags(CPF_SaveGame);
					//
					if (!IsSaveGame) {LOG_DB(Log,ESQLSeverity::Warning,FString::Printf(TEXT("Property not tagged 'SaveGame'! Unpack aborted:  %s"),*Property->GetName())); break;} 
					if (!(IsInt||IsByte||IsDate)) {LOG_DB(Log,ESQLSeverity::Warning,FString::Printf(TEXT("[Field Type <-> Property Type] mismatch:  %s"),*Property->GetName())); break;}
					//
					if (IsInt) {USQL::SQLToUProperty(Field,CastChecked<UIntProperty>(Property),Object);}
					if (IsByte) {USQL::SQLToUProperty(Field,CastChecked<UByteProperty>(Property),Object);}
					if (IsDate) {USQL::SQLToUProperty(Field,CastChecked<UStructProperty>(Property),Object);}
				}///
			}	break;
			//
			case ESQLType::Float:
			{
				for (TFieldIterator<UProperty>PIT(Object->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
					UProperty* Property = *PIT;
					auto FieldID = DB_GetMappedColumnID(Object->GetClass(),Field.Name.ToString());
					if (!Property->GetName().Equals(FieldID,ESearchCase::IgnoreCase)) {continue;}
					//
					const bool IsFloat = Property->IsA(UFloatProperty::StaticClass());
					const bool IsSaveGame = Property->HasAnyPropertyFlags(CPF_SaveGame);
					//
					if (!IsFloat) {LOG_DB(Log,ESQLSeverity::Warning,FString::Printf(TEXT("[Field Type <-> Property Type] mismatch:  %s"),*Property->GetName())); break;}
					if (!IsSaveGame) {LOG_DB(Log,ESQLSeverity::Warning,FString::Printf(TEXT("Property not tagged 'SaveGame'! Unpack aborted:  %s"),*Property->GetName())); break;} 
					//
					if (IsFloat) {USQL::SQLToUProperty(Field,CastChecked<UFloatProperty>(Property),Object);}
				}///
			}	break;
			//
			case ESQLType::Text:
			{
				for (TFieldIterator<UProperty>PIT(Object->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
					UProperty* Property = *PIT;
					auto FieldID = DB_GetMappedColumnID(Object->GetClass(),Field.Name.ToString());
					if (!Property->GetName().Equals(FieldID,ESearchCase::IgnoreCase)) {continue;}
					//
					const bool IsSet = Property->IsA(USetProperty::StaticClass());
					const bool IsMap = Property->IsA(UMapProperty::StaticClass());
					const bool IsBool = Property->IsA(UBoolProperty::StaticClass());
					const bool IsEnum = Property->IsA(UEnumProperty::StaticClass());
					const bool IsName = Property->IsA(UNameProperty::StaticClass());
					const bool IsText = Property->IsA(UTextProperty::StaticClass());
					const bool IsString = Property->IsA(UStrProperty::StaticClass());
					const bool IsArray = Property->IsA(UArrayProperty::StaticClass());
					const bool IsStruct = Property->IsA(UStructProperty::StaticClass());
					const bool IsObject = Property->IsA(UObjectProperty::StaticClass());
					const bool IsSaveGame = Property->HasAnyPropertyFlags(CPF_SaveGame);
					if (!IsSaveGame) {LOG_DB(Log,ESQLSeverity::Warning,FString::Printf(TEXT("Property not tagged 'SaveGame'! Unpack aborted:  %s"),*Property->GetName())); break;} 
					//
					if ((IsSet)&&(Field.Cast==ESQLReadMode::Set)) {USQL::SQLToUProperty(Field,CastChecked<USetProperty>(Property),Object);}
					if ((IsMap)&&(Field.Cast==ESQLReadMode::Map)) {USQL::SQLToUProperty(Field,CastChecked<UMapProperty>(Property),Object);}
					if ((IsEnum)&&(Field.Cast==ESQLReadMode::Enum)) {USQL::SQLToUProperty(Field,CastChecked<UEnumProperty>(Property),Object);}
					if ((IsName)&&(Field.Cast==ESQLReadMode::Name)) {USQL::SQLToUProperty(Field,CastChecked<UNameProperty>(Property),Object);}
					if ((IsText)&&(Field.Cast==ESQLReadMode::Text)) {USQL::SQLToUProperty(Field,CastChecked<UTextProperty>(Property),Object);}
					if ((IsBool)&&(Field.Cast==ESQLReadMode::Boolean)) {USQL::SQLToUProperty(Field,CastChecked<UBoolProperty>(Property),Object);}
					if ((IsString)&&(Field.Cast==ESQLReadMode::String)) {USQL::SQLToUProperty(Field,CastChecked<UStrProperty>(Property),Object);}
					if ((IsArray)&&(Field.Cast==ESQLReadMode::Array)) {USQL::SQLToUProperty(Field,CastChecked<UArrayProperty>(Property),Object);}
					if ((IsString)&&(Field.Cast==ESQLReadMode::TimeStamp)) {USQL::SQLToUProperty(Field,CastChecked<UStrProperty>(Property),Object);}
					if ((IsObject)&&(Field.Cast==ESQLReadMode::ObjectPtr)) {USQL::SQLToUProperty(Field,CastChecked<UObjectProperty>(Property),Object);}
					//
					if ((IsStruct)&&(Field.Cast==ESQLReadMode::Color)) {USQL::SQLToUProperty(Field,CastChecked<UStructProperty>(Property),Object);}
					if ((IsStruct)&&(Field.Cast==ESQLReadMode::Struct)) {USQL::SQLToUProperty(Field,CastChecked<UStructProperty>(Property),Object);}
					if ((IsStruct)&&(Field.Cast==ESQLReadMode::Rotator)) {USQL::SQLToUProperty(Field,CastChecked<UStructProperty>(Property),Object);}
					if ((IsStruct)&&(Field.Cast==ESQLReadMode::Vector2D)) {USQL::SQLToUProperty(Field,CastChecked<UStructProperty>(Property),Object);}
					if ((IsStruct)&&(Field.Cast==ESQLReadMode::Vector3D)) {USQL::SQLToUProperty(Field,CastChecked<UStructProperty>(Property),Object);}
					if ((IsStruct)&&(Field.Cast==ESQLReadMode::FloatCurve)) {USQL::SQLToUProperty(Field,CastChecked<UStructProperty>(Property),Object);}
				}///
			}	break;
		case ESQLType::Unknown:
	default: break;}}
	//
	LOG_DB(Log,ESQLSeverity::Info,FString::Printf(TEXT("Unpacking Complete:  %s"),*Object->GetName()));
#endif
}

void USQLite::DB_UnpackActorDATA(AActor* Actor, const FSQLRow Data, const bool Log) {
#if WITH_SERVER_CODE
	DB_UnpackObjectDATA(Actor,Data,Log);
#endif
}

void USQLite::DB_UnpackComponentDATA(UActorComponent* Component, const FSQLRow Data, const bool Log) {
#if WITH_SERVER_CODE
	DB_UnpackObjectDATA(Component,Data,Log);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DB Driver Interface Methods:

void USQLite::DB_Save_Implementation(UObject* Context, const ESQLSaveMode Mode) {
#if WITH_SERVER_CODE
	if (USQL::ThreadSafety!=ESQLThreadSafety::IsCurrentlyThreadSafe) {return;}
	USQL::ThreadSafety = ESQLThreadSafety::IsPreparingToSaveOrLoad;
	//
	if ((USQL::GetSaveProgress()<100.f)&&(USQL::GetSavesDone()>0)||(USQL::GetLoadProgress()<100.f)&&(USQL::GetLoadsDone()>0))
	{LOG_DB(Debug,ESQLSeverity::Warning,TEXT("DB Save action already in progress... Cannot begin another Save action, Save aborted!")); return;}
	//
	if (!Context||!Context->GetWorld()||!Context->IsValidLowLevelFast()||Context->IsPendingKill())
	{LOG_DB(Debug,ESQLSeverity::Error,TEXT("Context Object is invalid to start a DB Save process... Save aborted!")); return;}
	if (!Context->GetWorld()||Context->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {
		LOG_DB(Debug,ESQLSeverity::Error,TEXT("Context Game World is invalid to start a DB Save process... Save aborted!"));
	return;} World = Context->GetWorld();
	//
	World = Context->GetWorld();
	const ENetMode NET = World->GetNetMode();
	//
	if (NET != NM_Client) {
		for (TObjectIterator<UObject>Object; Object; ++Object) {
			if (!(*Object)||!(*Object)->IsValidLowLevelFast()||(*Object)->IsPendingKill()) {continue;}
			if ((*Object)->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
			//
			if ((*Object)->IsA(AActor::StaticClass())) {
				const auto &Actor = CastChecked<AActor>(*Object);
				if (Actor->Role != ROLE_Authority) {continue;}
			} else {
				const auto &Owner = (*Object)->GetTypedOuter<AActor>();
				if (Owner) {
					if (Owner->Role != ROLE_Authority) {continue;}
				}///
			}///
			//
			const auto &Interface = Cast<ISQLiteData>(*Object);
			//
			if (Interface) {
				Interface->Execute_DB_PrepareToSave(*Object,const_cast<USQLite*>(this),Mode);
			} else if (Object->GetClass()->ImplementsInterface(USQLiteData::StaticClass())) {
				ISQLiteData::Execute_DB_PrepareToSave(*Object,const_cast<USQLite*>(this),Mode);
			}///
		}///
	}///
	//
	DB_LaunchLoadScreen(ESQLThreadSafety::AsynchronousSaving,FeedbackSAVE);
	USQL::DB_Save(const_cast<USQLite*>(this));
#endif
}

void USQLite::DB_Load_Implementation(UObject* Context) {
#if WITH_SERVER_CODE
	if (USQL::ThreadSafety!=ESQLThreadSafety::IsCurrentlyThreadSafe) {return;}
	USQL::ThreadSafety = ESQLThreadSafety::IsPreparingToSaveOrLoad;
	//
	if ((USQL::GetSaveProgress()<100.f)&&(USQL::GetSavesDone()>0)||(USQL::GetLoadProgress()<100.f)&&(USQL::GetLoadsDone()>0))
	{LOG_DB(Debug,ESQLSeverity::Warning,TEXT("DB Load action already in progress... Cannot begin another Load action, Load aborted!")); return;}
	//
	if (!Context||!Context->GetWorld()||!Context->IsValidLowLevelFast()||Context->IsPendingKill())
	{LOG_DB(Debug,ESQLSeverity::Error,TEXT("Context Object is invalid to start a DB Load process... Load aborted!")); return;}
	if (!Context->GetWorld()||Context->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {
		LOG_DB(Debug,ESQLSeverity::Error,TEXT("Context Game World is invalid to start a DB Load process... Load aborted!"));
	return;}
	//
	World = Context->GetWorld();
	const ENetMode NET = World->GetNetMode();
	//
	if (NET != NM_Client) {
		for (TObjectIterator<UObject>Object; Object; ++Object) {
			if (!(*Object)||!(*Object)->IsValidLowLevelFast()||(*Object)->IsPendingKill()) {continue;}
			if ((*Object)->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
			const auto &Interface = Cast<ISQLiteData>(*Object);
			//
			if (Interface) {
				Interface->Execute_DB_PrepareToLoad(*Object,const_cast<USQLite*>(this));
			} else if (Object->GetClass()->ImplementsInterface(USQLiteData::StaticClass())) {
				ISQLiteData::Execute_DB_PrepareToLoad(*Object,const_cast<USQLite*>(this));
			}///
		}///
	}///
	//
	DB_LaunchLoadScreen(ESQLThreadSafety::AsynchronousLoading,FeedbackLOAD);
	USQL::DB_Load(const_cast<USQLite*>(this));
#endif
}

void USQLite::DB_OBJ_ImmediateSAVE(UObject* Object) {
#if WITH_SERVER_CODE
	if (!Object||!Object->IsValidLowLevelFast()||Object->IsPendingKill())
	{LOG_DB(Debug,ESQLSeverity::Error,TEXT("Context Object is invalid to start a DB Save process... Save aborted!")); return;}
	//
	if (!Object->GetWorld()||Object->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {
		LOG_DB(Debug,ESQLSeverity::Error,TEXT("Context Game World is invalid to start a DB Save process... Save aborted!"));
	return;}
	//
	const auto SQL = DB_GenerateSQL_Object_UPDATE(Object);
	USQL::DB_ImmediateSave(const_cast<USQLite*>(this),SQL);
#endif
}

void USQLite::DB_CMP_ImmediateSAVE(UActorComponent* Component) {
#if WITH_SERVER_CODE
	if (!Component||!Component->IsValidLowLevelFast()||Component->IsPendingKill())
	{LOG_DB(Debug,ESQLSeverity::Error,TEXT("Context Component is invalid to start a DB Save process... Save aborted!")); return;}
	//
	if (!Component->GetWorld()||Component->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {
		LOG_DB(Debug,ESQLSeverity::Error,TEXT("Context Game World is invalid to start a DB Save process... Save aborted!"));
	return;}
	//
	const auto SQL = DB_GenerateSQL_Component_UPDATE(Component);
	USQL::DB_ImmediateSave(const_cast<USQLite*>(this),SQL);
#endif
}

void USQLite::DB_ACT_ImmediateSAVE(AActor* Actor) {
#if WITH_SERVER_CODE
	if (!Actor||!Actor->IsValidLowLevelFast()||Actor->IsPendingKill())
	{LOG_DB(Debug,ESQLSeverity::Error,TEXT("Context Actor is invalid to start a DB Save process... Save aborted!")); return;}
	//
	if (!Actor->GetWorld()||Actor->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {
		LOG_DB(Debug,ESQLSeverity::Error,TEXT("Context Game World is invalid to start a DB Save process... Save aborted!"));
	return;}
	//
	const auto SQL = DB_GenerateSQL_Actor_UPDATE(Actor);
	USQL::DB_ImmediateSave(const_cast<USQLite*>(this),SQL);
#endif
}

void USQLite::DB_OBJ_ImmediateLOAD(UObject* Object) {
#if WITH_SERVER_CODE
	if (!Object||!Object->IsValidLowLevelFast()||Object->IsPendingKill())
	{LOG_DB(Debug,ESQLSeverity::Error,TEXT("Context Object is invalid to start a DB Load process... Load aborted!")); return;}
	//
	if (!Object->GetWorld()||Object->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {
		LOG_DB(Debug,ESQLSeverity::Error,TEXT("Context Game World is invalid to start a DB Load process... Load aborted!"));
	return;}
	//
	const auto SQL = DB_GenerateSQL_Object_SELECT(Object);
	USQL::DB_ImmediateLoad(const_cast<USQLite*>(this),Object,SQL);
#endif
}

void USQLite::DB_CMP_ImmediateLOAD(UActorComponent* Component) {
#if WITH_SERVER_CODE
	if (!Component||!Component->IsValidLowLevelFast()||Component->IsPendingKill())
	{LOG_DB(Debug,ESQLSeverity::Error,TEXT("Context Component is invalid to start a DB Load process... Load aborted!")); return;}
	//
	if (!Component->GetWorld()||Component->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {
		LOG_DB(Debug,ESQLSeverity::Error,TEXT("Context Game World is invalid to start a DB Load process... Load aborted!"));
	return;}
	//
	const auto SQL = DB_GenerateSQL_Component_SELECT(Component);
	USQL::DB_ImmediateLoad(const_cast<USQLite*>(this),Component,SQL);
#endif
}

void USQLite::DB_ACT_ImmediateLOAD(AActor* Actor) {
#if WITH_SERVER_CODE
	if (!Actor||!Actor->IsValidLowLevelFast()||Actor->IsPendingKill())
	{LOG_DB(Debug,ESQLSeverity::Error,TEXT("Context Actor is invalid to start a DB Load process... Load aborted!")); return;}
	//
	if (!Actor->GetWorld()||Actor->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {
		LOG_DB(Debug,ESQLSeverity::Error,TEXT("Context Game World is invalid to start a DB Load process... Load aborted!"));
	return;}
	//
	const auto SQL = DB_GenerateSQL_Actor_SELECT(Actor);
	USQL::DB_ImmediateLoad(const_cast<USQLite*>(this),Actor,SQL);
#endif
}

void USQLite::DB_DoSaveProgressReport(USQLite* Database, const float Progress) {
	if ((USQL::GetSaveProgress()==100.f)&&(USQL::GetSavesDone()==0)) {return;}
	if (Database!=const_cast<USQLite*>(this)) {return;}
	//
	for (TObjectIterator<UObject>Object; Object; ++Object) {
		if (!(*Object)||!(*Object)->IsValidLowLevelFast()||(*Object)->IsPendingKill()) {continue;}
		if ((*Object)->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
		const auto &Interface = Cast<ISQLiteData>(*Object);
		//
		if (Interface) {
			Interface->Execute_DB_OnGlobalSaveProgressReport__Threaded(*Object,Progress);
		} else if (Object->GetClass()->ImplementsInterface(USQLiteData::StaticClass())) {
			ISQLiteData::Execute_DB_OnGlobalSaveProgressReport__Threaded(*Object,Progress);
		}///
	}///
}

void USQLite::DB_DoLoadProgressReport(USQLite* Database, const float Progress) {
	if (Database!=const_cast<USQLite*>(this)) {return;}
	//
	for (TObjectIterator<UObject>Object; Object; ++Object) {
		if (!(*Object)||!(*Object)->IsValidLowLevelFast()||(*Object)->IsPendingKill()) {continue;}
		if ((*Object)->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
		const auto &Interface = Cast<ISQLiteData>(*Object);
		//
		if (Interface) {
			Interface->Execute_DB_OnGlobalLoadProgressReport__Threaded(*Object,Progress);
		} else if (Object->GetClass()->ImplementsInterface(USQLiteData::StaticClass())) {
			ISQLiteData::Execute_DB_OnGlobalLoadProgressReport__Threaded(*Object,Progress);
		}///
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ESQLResult USQLite::DB_ExecuteCommand(const FString &SQL) {
#if WITH_SERVER_CODE
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
}

ESQLResult USQLite::DB_UPDATE_Boolean(const FString &RowID, const FString &ColumnName, const bool Value) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Bools)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not a Boolean Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	const FString Parse = (Value==true) ? TEXT("UBool>>true") : TEXT("UBool>>false");
	//
	FString SQL, Header, Values;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Values = FString::Printf(TEXT("('%s') "),*Parse);
	SQL = Header+Values+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_Byte(const FString &RowID, const FString &ColumnName, const uint8 Value) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Bytes)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not a Byte Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	const FString Parse = FString::Printf(TEXT("%i"),Value);
	//
	FString SQL, Header, Values;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Values = FString::Printf(TEXT("(%s) "),*Parse);
	SQL = Header+Values+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_Integer(const FString &RowID, const FString &ColumnName, const int32 Value) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Ints)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not a Integer Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	const FString Parse = FString::Printf(TEXT("%i"),Value);
	//
	FString SQL, Header, Values;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Values = FString::Printf(TEXT("(%s) "),*Parse);
	SQL = Header+Values+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_Float(const FString &RowID, const FString &ColumnName, const float Value) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Floats)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not a Float Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	const FString Parse = FString::Printf(TEXT("%f"),Value);
	//
	FString SQL, Header, Values;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Values = FString::Printf(TEXT("(%s) "),*Parse);
	SQL = Header+Values+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_Name(const FString &RowID, const FString &ColumnName, const FName Value) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Names)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not a Name Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	const FString Spell = Value.ToString().Replace(TEXT("'"),TEXT("`"));
	const FString Parse = FString::Printf(TEXT("UName>>%s"),*Spell);
	//
	FString SQL, Header, Values;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Values = FString::Printf(TEXT("('%s') "),*Parse);
	SQL = Header+Values+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_Text(const FString &RowID, const FString &ColumnName, const FText Value) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Texts)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not a Text Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	const FString Spell = Value.ToString().Replace(TEXT("'"),TEXT("`"));
	const FString Parse = FString::Printf(TEXT("UText>>%s"),*Spell);
	//
	FString SQL, Header, Values;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Values = FString::Printf(TEXT("('%s') "),*Parse);
	SQL = Header+Values+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_String(const FString &RowID, const FString &ColumnName, const FString Value) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Strings)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not a String Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	const FString Spell = Value.Replace(TEXT("'"),TEXT("`"));
	const FString Parse = FString::Printf(TEXT("UString>>%s"),*Spell);
	//
	FString SQL, Header, Values;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Values = FString::Printf(TEXT("('%s') "),*Parse);
	SQL = Header+Values+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_Date(const FString &RowID, const FString &ColumnName, const FDateTime Value) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.DateTimes)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not a Date Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	const FString Parse = FString::Printf(TEXT("%i"),Value.ToUnixTimestamp());
	//
	FString SQL, Header, Values;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Values = FString::Printf(TEXT("(%s) "),*Parse);
	SQL = Header+Values+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_Color(const FString &RowID, const FString &ColumnName, const FColor Value) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Colors)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not a Color Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	const FString R = FString::Printf(TEXT("\"r\": %i"),Value.R);
	const FString G = FString::Printf(TEXT("\"g\": %i"),Value.G);
	const FString B = FString::Printf(TEXT("\"b\": %i"),Value.B);
	const FString A = FString::Printf(TEXT("\"a\": %i"),Value.A);
	const FString Parse = FString::Printf(TEXT("UColor>>{%s, %s, %s, %s}"),*B,*G,*R,*A);
	//
	FString SQL, Header, Values;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Values = FString::Printf(TEXT("('%s') "),*Parse);
	SQL = Header+Values+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_Vector2D(const FString &RowID, const FString &ColumnName, const FVector2D Value) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Vector2D)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not a Vector 2D Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	const FString X = FString::Printf(TEXT("\"x\": %f"),Value.X);
	const FString Y = FString::Printf(TEXT("\"y\": %f"),Value.Y);
	const FString Parse = FString::Printf(TEXT("UVector2D>>{%s, %s}"),*X,*Y);
	//
	FString SQL, Header, Values;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Values = FString::Printf(TEXT("('%s') "),*Parse);
	SQL = Header+Values+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_Vector3D(const FString &RowID, const FString &ColumnName, const FVector Value) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Vector3D)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not a Vector 3D Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	const FString X = FString::Printf(TEXT("\"x\": %f"),Value.X);
	const FString Y = FString::Printf(TEXT("\"y\": %f"),Value.Y);
	const FString Z = FString::Printf(TEXT("\"z\": %f"),Value.Z);
	const FString Parse = FString::Printf(TEXT("UVector3D>>{%s, %s, %s}"),*X,*Y,*Z);
	//
	FString SQL, Header, Values;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Values = FString::Printf(TEXT("('%s') "),*Parse);
	SQL = Header+Values+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_Rotator(const FString &RowID, const FString &ColumnName, const FRotator Value) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Rotators)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not a Rotator Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	const FString Yaw = FString::Printf(TEXT("\"yaw\": %f"),Value.Yaw);
	const FString Roll = FString::Printf(TEXT("\"roll\": %f"),Value.Roll);
	const FString Pitch = FString::Printf(TEXT("\"pitch\": %f"),Value.Pitch);
	const FString Parse = FString::Printf(TEXT("URotator>>{%s, %s, %s}"),*Pitch,*Yaw,*Roll);
	//
	FString SQL, Header, Values;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Values = FString::Printf(TEXT("('%s') "),*Parse);
	SQL = Header+Values+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_Object(const FString &RowID, const FString &ColumnName, const UObject* OBJ) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.ObjectPtrs)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not a Object Pointer Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	FString Parse;
	if ((OBJ!=nullptr)&&OBJ->IsValidLowLevelFast()) {
		auto ObjectPath = OBJ->GetFullName();
		auto ClassPath = OBJ->GetClass()->GetDefaultObject()->GetPathName();
		//
		const FString Target = (ClassPath+TEXT("::")+ObjectPath);
		Parse = FString::Printf(TEXT("'UObject>>%s'"),*Target);
	}///
	//
	FString SQL, Header, Values;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Values = FString::Printf(TEXT("('%s') "),*Parse);
	SQL = Header+Values+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ESQLResult USQLite::DB_UPDATE_Boolean_Array(const FString &RowID, const FString &ColumnName, const TArray<bool>&Values) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not an Array of Booleans Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	FString Output; TArray<TSharedPtr<FJsonValue>>JTArray;
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	//
	for (int32 I=0, N=Values.Num()-1; I<=N; I++) {
		bool Value = Values[I];
		TSharedPtr<FJsonValue>JValue = MakeShareable(new FJsonValueBoolean(Value)); JTArray.Push(JValue);
	} if (JTArray.Num()>0) {JSON->SetArrayField(ColumnName,JTArray);}
	//
	FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);
	const FString Parse = FString::Printf(TEXT("'UArray>>%s'"),*Output);
	//
	FString SQL, Header, Body;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Body = FString::Printf(TEXT("('%s') "),*Parse);
	SQL = Header+Body+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_Byte_Array(const FString &RowID, const FString &ColumnName, const TArray<uint8>&Values) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not an Array of Bytes Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	FString Output; TArray<TSharedPtr<FJsonValue>>JTArray;
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	//
	for (int32 I=0, N=Values.Num()-1; I<=N; I++) {
		uint8 Value = Values[I];
		TSharedPtr<FJsonValue>JValue = MakeShareable(new FJsonValueNumber(Value)); JTArray.Push(JValue);
	} if (JTArray.Num()>0) {JSON->SetArrayField(ColumnName,JTArray);}
	//
	FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);
	const FString Parse = FString::Printf(TEXT("'UArray>>%s'"),*Output);
	//
	FString SQL, Header, Body;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Body = FString::Printf(TEXT("('%s') "),*Parse);
	SQL = Header+Body+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_Integer_Array(const FString &RowID, const FString &ColumnName, const TArray<int32>&Values) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not an Array of Integers Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	FString Output; TArray<TSharedPtr<FJsonValue>>JTArray;
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	//
	for (int32 I=0, N=Values.Num()-1; I<=N; I++) {
		int32 Value = Values[I];
		TSharedPtr<FJsonValue>JValue = MakeShareable(new FJsonValueNumber(Value)); JTArray.Push(JValue);
	} if (JTArray.Num()>0) {JSON->SetArrayField(ColumnName,JTArray);}
	//
	FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);
	const FString Parse = FString::Printf(TEXT("'UArray>>%s'"),*Output);
	//
	FString SQL, Header, Body;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Body = FString::Printf(TEXT("('%s') "),*Parse);
	SQL = Header+Body+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_Float_Array(const FString &RowID, const FString &ColumnName, const TArray<float>&Values) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not an Array of Floats Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	FString Output; TArray<TSharedPtr<FJsonValue>>JTArray;
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	//
	for (int32 I=0, N=Values.Num()-1; I<=N; I++) {
		float Value = Values[I];
		TSharedPtr<FJsonValue>JValue = MakeShareable(new FJsonValueNumber(Value)); JTArray.Push(JValue);
	} if (JTArray.Num()>0) {JSON->SetArrayField(ColumnName,JTArray);}
	//
	FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);
	const FString Parse = FString::Printf(TEXT("'UArray>>%s'"),*Output);
	//
	FString SQL, Header, Body;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Body = FString::Printf(TEXT("('%s') "),*Parse);
	SQL = Header+Body+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_Name_Array(const FString &RowID, const FString &ColumnName, const TArray<FName>&Values) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not an Array of Names Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	FString Output; TArray<TSharedPtr<FJsonValue>>JTArray;
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	//
	for (int32 I=0, N=Values.Num()-1; I<=N; I++) {
		FName Value = Values[I];
		TSharedPtr<FJsonValue>JValue = MakeShareable(new FJsonValueString(Value.ToString())); JTArray.Push(JValue);
	} if (JTArray.Num()>0) {JSON->SetArrayField(ColumnName,JTArray);}
	//
	FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);
	const FString Parse = FString::Printf(TEXT("'UArray>>%s'"),*Output);
	//
	FString SQL, Header, Body;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Body = FString::Printf(TEXT("('%s') "),*Parse);
	SQL = Header+Body+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_Text_Array(const FString &RowID, const FString &ColumnName, const TArray<FText>&Values) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not an Array of Texts Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	FString Output; TArray<TSharedPtr<FJsonValue>>JTArray;
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	//
	for (int32 I=0, N=Values.Num()-1; I<=N; I++) {
		FText Value = Values[I];
		TSharedPtr<FJsonValue>JValue = MakeShareable(new FJsonValueString(Value.ToString())); JTArray.Push(JValue);
	} if (JTArray.Num()>0) {JSON->SetArrayField(ColumnName,JTArray);}
	//
	FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);
	const FString Parse = FString::Printf(TEXT("'UArray>>%s'"),*Output);
	//
	FString SQL, Header, Body;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Body = FString::Printf(TEXT("('%s') "),*Parse);
	SQL = Header+Body+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_String_Array(const FString &RowID, const FString &ColumnName, const TArray<FString>&Values) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not an Array of Strings Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	FString Output; TArray<TSharedPtr<FJsonValue>>JTArray;
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	//
	for (int32 I=0, N=Values.Num()-1; I<=N; I++) {
		FString Value = Values[I];
		TSharedPtr<FJsonValue>JValue = MakeShareable(new FJsonValueString(Value)); JTArray.Push(JValue);
	} if (JTArray.Num()>0) {JSON->SetArrayField(ColumnName,JTArray);}
	//
	FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);
	const FString Parse = FString::Printf(TEXT("'UArray>>%s'"),*Output);
	//
	FString SQL, Header, Body;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Body = FString::Printf(TEXT("('%s') "),*Parse);
	SQL = Header+Body+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_Vector2D_Array(const FString &RowID, const FString &ColumnName, const TArray<FVector2D>&Values) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not an Array of Vector 2D Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	FString Output; TArray<TSharedPtr<FJsonValue>>JTArray;
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	//
	for (int32 I=0, N=Values.Num()-1; I<=N; I++) {
		FVector2D Value = Values[I];
		//
		const FString X = FString::Printf(TEXT("\"x\": %f"),Value.X);
		const FString Y = FString::Printf(TEXT("\"y\": %f"),Value.Y);
		const FString V2D = FString::Printf(TEXT("{%s, %s}"),*X,*Y);
		//
		TSharedPtr<FJsonValue>JValue = MakeShareable(new FJsonValueString(V2D)); JTArray.Push(JValue);
	} if (JTArray.Num()>0) {JSON->SetArrayField(ColumnName,JTArray);}
	//
	FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);
	const FString Parse = FString::Printf(TEXT("'UArray>>%s'"),*Output);
	//
	FString SQL, Header, Body;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Body = FString::Printf(TEXT("('%s') "),*Parse);
	SQL = Header+Body+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_Vector3D_Array(const FString &RowID, const FString &ColumnName, const TArray<FVector>&Values) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not an Array of Vector 3D Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	FString Output; TArray<TSharedPtr<FJsonValue>>JTArray;
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	//
	for (int32 I=0, N=Values.Num()-1; I<=N; I++) {
		FVector Value = Values[I];
		//
		const FString X = FString::Printf(TEXT("\"x\": %f"),Value.X);
		const FString Y = FString::Printf(TEXT("\"y\": %f"),Value.Y);
		const FString Z = FString::Printf(TEXT("\"z\": %f"),Value.Z);
		const FString V3D = FString::Printf(TEXT("{%s, %s}"),*X,*Y);
		//
		TSharedPtr<FJsonValue>JValue = MakeShareable(new FJsonValueString(V3D)); JTArray.Push(JValue);
	} if (JTArray.Num()>0) {JSON->SetArrayField(ColumnName,JTArray);}
	//
	FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);
	const FString Parse = FString::Printf(TEXT("'UArray>>%s'"),*Output);
	//
	FString SQL, Header, Body;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Body = FString::Printf(TEXT("('%s') "),*Parse);
	SQL = Header+Body+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_Rotator_Array(const FString &RowID, const FString &ColumnName, const TArray<FRotator>&Values) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not an Array of Rotator Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	FString Output; TArray<TSharedPtr<FJsonValue>>JTArray;
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	//
	for (int32 I=0, N=Values.Num()-1; I<=N; I++) {
		FRotator Value = Values[I];
		//
		const FString Yaw = FString::Printf(TEXT("\"yaw\": %f"),Value.Yaw);
		const FString Roll = FString::Printf(TEXT("\"roll\": %f"),Value.Roll);
		const FString Pitch = FString::Printf(TEXT("\"pitch\": %f"),Value.Pitch);
		const FString Rotator = FString::Printf(TEXT("{%s, %s, %s}"),*Pitch,*Yaw,*Roll);
		//
		TSharedPtr<FJsonValue>JValue = MakeShareable(new FJsonValueString(Rotator)); JTArray.Push(JValue);
	} if (JTArray.Num()>0) {JSON->SetArrayField(ColumnName,JTArray);}
	//
	FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);
	const FString Parse = FString::Printf(TEXT("'UArray>>%s'"),*Output);
	//
	FString SQL, Header, Body;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Body = FString::Printf(TEXT("('%s') "),*Parse);
	SQL = Header+Body+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

ESQLResult USQLite::DB_UPDATE_Object_Array(const FString &RowID, const FString &ColumnName, const TArray<UObject*>&Values) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (UPDATE Column Command): Target Column Type mismatch; Column is not an Array of Object Pointers Field: %s"),*GetName(),*ColumnName));
		return ESQLResult::ABORT;
	}///
	//
	//
	FString Output; TArray<TSharedPtr<FJsonValue>>JTArray;
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	//
	for (int32 I=0, N=Values.Num()-1; I<=N; I++) {
		UObject* OBJ = Values[I];
		//
		if ((OBJ!=nullptr)&&OBJ->IsValidLowLevelFast()) {
			auto ObjectPath = OBJ->GetFullName();
			auto ClassPath = OBJ->GetClass()->GetDefaultObject()->GetPathName();
			//
			const FString Target = (ClassPath+TEXT("::")+ObjectPath);
			TSharedPtr<FJsonValue>JValue = MakeShareable(new FJsonValueString(Target)); JTArray.Push(JValue);
		}///
	} if (JTArray.Num()>0) {JSON->SetArrayField(ColumnName,JTArray);}
	//
	FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);
	const FString Parse = FString::Printf(TEXT("'UArray>>%s'"),*Output);
	//
	FString SQL, Header, Body;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {Header=FString::Printf(TEXT("UPDATE %s SET ("),*GetNameSanitized());}
	else {Header=FString::Printf(TEXT("UPDATE %s SET ("),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	Header += (ColumnName+TEXT(") = "));
	Body = FString::Printf(TEXT("('%s') "),*Parse);
	SQL = Header+Body+FString::Printf(TEXT("WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	return USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
#endif
	//
	return ESQLResult::ABORT;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FSQLRow USQLite::DB_SELECT_Row(const FString &RowID) {
#if WITH_SERVER_CODE
	if (!DB_TABLE.ColumnTypes.IsValidIndex(0)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Row Command): Unnable to read IDs from this Database!"),*GetName()));
		return FSQLRow();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT * FROM %s"),*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT * FROM %s"),*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	FSQLRow Row;
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=1024"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
		//
		Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
		if (Result==ESQLResult::OK) {
			int32 Step=0;
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
			}} DB_FINAL(Statement);
			//
			if (Row.Values.Num()>0) {
				Row.ID = FName(*Row.Values[0].ValueToString());
				Row.DB = GetFName();
				//
				for (const auto &VAL : Row.Values) {
					LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
					switch (VAL.Type) {
						case ESQLType::Integer:
							LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %i"),VAL.Integer)); break;
						case ESQLType::Float:
							LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %f"),VAL.Float)); break;
						case ESQLType::Text:
							LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text)); break;
				default: break;}}
			}///
		}///
	DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return Row;
#endif
	//
	return FSQLRow();
}

ESQLResult USQLite::DB_DELETE_Row(const FString &RowID) {
#if WITH_SERVER_CODE
	if (!DB_TABLE.ColumnTypes.IsValidIndex(0)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (DELETE Row Command): Unnable to read IDs from this Database!"),*GetName()));
		return ESQLResult::ERROR;
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("DELETE FROM %s WHERE %s = '%s'"),*GetNameSanitized(),*DB_TABLE.Columns[0],*RowID);}
	else {SQL=FString::Printf(TEXT("DELETE FROM %s WHERE %s = '%s'"),*((GetNameSanitized()+TEXT("_"))+DB_VERSION.CurrentVersion),*DB_TABLE.Columns[0],*RowID);}
	//
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	ESQLResult Result = USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
	//
	return Result;
#endif
	//
	return ESQLResult::ERROR;
}

bool USQLite::DB_SELECT_Boolean(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return false;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Bools)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Boolean Field: %s"),*GetName(),*ColumnName));
		return false;
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=128"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UBool>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Boolean;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Boolean) {
									return (VAL.Text.Equals(TEXT("true"),ESearchCase::IgnoreCase)) ? true : false;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Boolean Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return false;
#endif
	//
	return false;
}

uint8 USQLite::DB_SELECT_Byte(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return INDEX_NONE;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Bytes)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Byte Field: %s"),*GetName(),*ColumnName));
		return INDEX_NONE;
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=128"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_INTEGER:
							{
								Field.Type = ESQLType::Integer;
								Field.Cast = ESQLReadMode::Integer;
								Field.Integer = sqlite3_column_int64(Statement,Column);
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Integer:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %i"),VAL.Integer));
								//
								if (VAL.Cast==ESQLReadMode::Integer) {
									return VAL.Integer;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Byte Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return INDEX_NONE;
#endif
	//
	return INDEX_NONE;
}

int32 USQLite::DB_SELECT_Integer(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return INDEX_NONE;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Ints)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Integer Field: %s"),*GetName(),*ColumnName));
		return INDEX_NONE;
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=128"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_INTEGER:
							{
								Field.Type = ESQLType::Integer;
								Field.Cast = ESQLReadMode::Integer;
								Field.Integer = sqlite3_column_int64(Statement,Column);
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Integer:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %i"),VAL.Integer));
								//
								if (VAL.Cast==ESQLReadMode::Integer) {
									return VAL.Integer;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Integer Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return INDEX_NONE;
#endif
	//
	return INDEX_NONE;
}

float USQLite::DB_SELECT_Float(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return INDEX_NONE;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Floats)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Float Field: %s"),*GetName(),*ColumnName));
		return INDEX_NONE;
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=128"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_FLOAT:
							{
								Field.Type = ESQLType::Float;
								Field.Cast = ESQLReadMode::Float;
								Field.Float = sqlite3_column_double(Statement,Column);
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Float:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %f"),VAL.Float));
								//
								if (VAL.Cast==ESQLReadMode::Float) {
									return VAL.Float;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Float Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return INDEX_NONE;
#endif
	//
	return INDEX_NONE;
}

FName USQLite::DB_SELECT_Name(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return FName();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Names)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Name Field: %s"),*GetName(),*ColumnName));
		return FName();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=128"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UName>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Name;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Name) {
									return *VAL.Text;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Name Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return FName();
#endif
	//
	return FName();
}

FText USQLite::DB_SELECT_Text(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return FText();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Texts)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Text Field: %s"),*GetName(),*ColumnName));
		return FText();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=128"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UText>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Text;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Text) {
									return FText::FromString(VAL.Text);
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Text Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return FText();
#endif
	//
	return FText();
}

FString USQLite::DB_SELECT_String(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return FString();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Strings)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a String Field: %s"),*GetName(),*ColumnName));
		return FString();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=128"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UString>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::String;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::String) {
									return VAL.Text;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a String Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return FString();
#endif
	//
	return FString();
}

FDateTime USQLite::DB_SELECT_Date(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return FDateTime();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.DateTimes)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Date Field: %s"),*GetName(),*ColumnName));
		return FDateTime();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=256"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_INTEGER:
							{
								Field.Type = ESQLType::Integer;
								Field.Cast = ESQLReadMode::Integer;
								Field.Integer = sqlite3_column_int64(Statement,Column);
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Integer:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*FDateTime::FromUnixTimestamp(VAL.Integer).ToString()));
								//
								if (VAL.Cast==ESQLReadMode::Integer) {
									return FDateTime::FromUnixTimestamp(VAL.Integer);
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Date Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return FDateTime();
#endif
	//
	return FDateTime();
}

FColor USQLite::DB_SELECT_Color(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return FColor();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Colors)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Color Field: %s"),*GetName(),*ColumnName));
		return FColor();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=128"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UColor>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Color;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Color) {
									TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
									TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(VAL.Text);
									//
									if (!FJsonSerializer::Deserialize(JReader,JSON)) {
										LOG_DB(Debug,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object for : [%s]"),*VAL.Name.ToString()));
									return FColor();}
									//
									FColor Color;
									Color.R = JSON->GetIntegerField(TEXT("r")); Color.G = JSON->GetIntegerField(TEXT("g"));
									Color.B = JSON->GetIntegerField(TEXT("b")); Color.A = JSON->GetIntegerField(TEXT("a"));
									//
									return Color;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Color Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return FColor();
#endif
	//
	return FColor();
}

FVector2D USQLite::DB_SELECT_Vector2D(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return FVector2D();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Vector2D)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Vector 2D Field: %s"),*GetName(),*ColumnName));
		return FVector2D();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=128"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UVector2D>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Vector2D;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Vector2D) {
									TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
									TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(VAL.Text);
									//
									if (!FJsonSerializer::Deserialize(JReader,JSON)) {
										LOG_DB(Debug,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object for : [%s]"),*VAL.Name.ToString()));
									return FVector2D();}
									//
									FVector2D Vector;
									Vector.X = JSON->GetNumberField(TEXT("x"));
									Vector.Y = JSON->GetNumberField(TEXT("y"));
									//
									return Vector;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Vector 2D Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return FVector2D();
#endif
	//
	return FVector2D();
}

FVector USQLite::DB_SELECT_Vector3D(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return FVector();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Vector3D)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Vector 3D Field: %s"),*GetName(),*ColumnName));
		return FVector();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=128"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UVector3D>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Vector3D;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Vector3D) {
									TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
									TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(VAL.Text);
									//
									if (!FJsonSerializer::Deserialize(JReader,JSON)) {
										LOG_DB(Debug,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object for : [%s]"),*VAL.Name.ToString()));
									return FVector();}
									//
									FVector Vector;
									Vector.X = JSON->GetNumberField(TEXT("x"));
									Vector.Y = JSON->GetNumberField(TEXT("y"));
									Vector.Z = JSON->GetNumberField(TEXT("z"));
									//
									return Vector;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Vector 3D Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return FVector();
#endif
	//
	return FVector();
}

FRotator USQLite::DB_SELECT_Rotator(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return FRotator();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Rotators)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Rotator Field: %s"),*GetName(),*ColumnName));
		return FRotator();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=128"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("URotator>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Rotator;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Rotator) {
									TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
									TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(VAL.Text);
									//
									if (!FJsonSerializer::Deserialize(JReader,JSON)) {
										LOG_DB(Debug,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object for : [%s]"),*VAL.Name.ToString()));
									return FRotator();}
									//
									FRotator Rotator;
									Rotator.Pitch = JSON->GetNumberField(TEXT("pitch"));
									Rotator.Roll = JSON->GetNumberField(TEXT("roll"));
									Rotator.Yaw = JSON->GetNumberField(TEXT("yaw"));
									//
									return Rotator;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Rotator Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return FRotator();
#endif
	//
	return FRotator();
}

UObject* USQLite::DB_SELECT_Object(UObject* Context, const FString &RowID, const FString &ColumnName) {
	if (!Context->IsValidLowLevelFast()||Context->GetWorld()==nullptr) {
		LOG_DB(true,ESQLSeverity::Warning,FString(TEXT("[%s] (SELECT Column Command): Target Column Field is Object, caller must implement and provide a valid 'GetWorld()' override function.")));
		return nullptr;
	}///
	//
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return nullptr;
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.ObjectPtrs)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Object Pointer Field: %s"),*GetName(),*ColumnName));
		return nullptr;
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=128"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UObject>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::ObjectPtr;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::ObjectPtr) {
									FString Class, Instance;
									VAL.Text.Split(TEXT("::"),&Class,&Instance);
									//
									const FSoftObjectPath ClassPath(Class);
									TSoftObjectPtr<UObject>ClassPtr(ClassPath);
									//
									auto CDO = ClassPtr.LoadSynchronous();
									if ((CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
										for (TActorIterator<AActor>Actor(Context->GetWorld()); Actor; ++Actor) {
											if ((*Actor)->GetFullName()==Instance) {return (*Actor); break;}
										}///
									} else if (CDO!=nullptr) {
										for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
											if ((*OBJ)->GetFullName()==Instance) {return (*OBJ); break;}
										}///
									}///
									//
									return nullptr;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Object Pointer Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return nullptr;
#endif
	//
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TArray<bool> USQLite::DB_SELECT_Boolean_Array(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<bool>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Booleans Field: %s"),*GetName(),*ColumnName));
		return TArray<bool>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=512"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UArray>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Array;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Array) {
									TArray<bool>Data;
									//
									TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
									TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(VAL.Text);
									//
									if (!FJsonSerializer::Deserialize(JReader,JSON)) {
										LOG_DB(Debug,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object for : [%s]"),*VAL.Name.ToString()));
									break;}
									//
									auto JArray = JSON->GetArrayField(VAL.Name.ToString());
									for (int32 I=0, N=JArray.Num()-1; I<=N; I++) {
										const auto &JValue = JArray[I];
										//
										if (JValue.IsValid()&&(!JValue->IsNull())) {
											const bool Value = JValue->AsBool();
											Data.Add(Value);
										}///
									}///
									//
									return Data;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Booleans Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return TArray<bool>();
#endif
	//
	return TArray<bool>();
}

TArray<uint8> USQLite::DB_SELECT_Byte_Array(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<uint8>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Bytes Field: %s"),*GetName(),*ColumnName));
		return TArray<uint8>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=512"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UArray>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Array;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Array) {
									TArray<uint8>Data;
									//
									TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
									TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(VAL.Text);
									//
									if (!FJsonSerializer::Deserialize(JReader,JSON)) {
										LOG_DB(Debug,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object for : [%s]"),*VAL.Name.ToString()));
									break;}
									//
									auto JArray = JSON->GetArrayField(VAL.Name.ToString());
									for (int32 I=0, N=JArray.Num()-1; I<=N; I++) {
										const auto &JValue = JArray[I];
										//
										if (JValue.IsValid()&&(!JValue->IsNull())) {
											const uint8 Value = JValue->AsNumber();
											Data.Add(Value);
										}///
									}///
									//
									return Data;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Bytes Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return TArray<uint8>();
#endif
	//
	return TArray<uint8>();
}

TArray<int32> USQLite::DB_SELECT_Integer_Array(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<int32>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Integers Field: %s"),*GetName(),*ColumnName));
		return TArray<int32>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=512"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UArray>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Array;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Array) {
									TArray<int32>Data;
									//
									TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
									TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(VAL.Text);
									//
									if (!FJsonSerializer::Deserialize(JReader,JSON)) {
										LOG_DB(Debug,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object for : [%s]"),*VAL.Name.ToString()));
									break;}
									//
									auto JArray = JSON->GetArrayField(VAL.Name.ToString());
									for (int32 I=0, N=JArray.Num()-1; I<=N; I++) {
										const auto &JValue = JArray[I];
										//
										if (JValue.IsValid()&&(!JValue->IsNull())) {
											const int32 Value = JValue->AsNumber();
											Data.Add(Value);
										}///
									}///
									//
									return Data;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Integers Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return TArray<int32>();
#endif
	//
	return TArray<int32>();
}

TArray<float> USQLite::DB_SELECT_Float_Array(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<float>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Floats Field: %s"),*GetName(),*ColumnName));
		return TArray<float>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=512"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UArray>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Array;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Array) {
									TArray<float>Data;
									//
									TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
									TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(VAL.Text);
									//
									if (!FJsonSerializer::Deserialize(JReader,JSON)) {
										LOG_DB(Debug,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object for : [%s]"),*VAL.Name.ToString()));
									break;}
									//
									auto JArray = JSON->GetArrayField(VAL.Name.ToString());
									for (int32 I=0, N=JArray.Num()-1; I<=N; I++) {
										const auto &JValue = JArray[I];
										//
										if (JValue.IsValid()&&(!JValue->IsNull())) {
											const float Value = JValue->AsNumber();
											Data.Add(Value);
										}///
									}///
									//
									return Data;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Floats Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return TArray<float>();
#endif
	//
	return TArray<float>();
}

TArray<FName> USQLite::DB_SELECT_Name_Array(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<FName>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Names Field: %s"),*GetName(),*ColumnName));
		return TArray<FName>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=512"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UArray>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Array;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Array) {
									TArray<FName>Data;
									//
									TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
									TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(VAL.Text);
									//
									if (!FJsonSerializer::Deserialize(JReader,JSON)) {
										LOG_DB(Debug,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object for : [%s]"),*VAL.Name.ToString()));
									break;}
									//
									auto JArray = JSON->GetArrayField(VAL.Name.ToString());
									for (int32 I=0, N=JArray.Num()-1; I<=N; I++) {
										const auto &JValue = JArray[I];
										//
										if (JValue.IsValid()&&(!JValue->IsNull())) {
											const FName Value = *JValue->AsString();
											Data.Add(Value);
										}///
									}///
									//
									return Data;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Names Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return TArray<FName>();
#endif
	//
	return TArray<FName>();
}

TArray<FText> USQLite::DB_SELECT_Text_Array(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<FText>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Texts Field: %s"),*GetName(),*ColumnName));
		return TArray<FText>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=512"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UArray>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Array;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Array) {
									TArray<FText>Data;
									//
									TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
									TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(VAL.Text);
									//
									if (!FJsonSerializer::Deserialize(JReader,JSON)) {
										LOG_DB(Debug,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object for : [%s]"),*VAL.Name.ToString()));
									break;}
									//
									auto JArray = JSON->GetArrayField(VAL.Name.ToString());
									for (int32 I=0, N=JArray.Num()-1; I<=N; I++) {
										const auto &JValue = JArray[I];
										//
										if (JValue.IsValid()&&(!JValue->IsNull())) {
											const FText Value = FText::FromString(JValue->AsString());
											Data.Add(Value);
										}///
									}///
									//
									return Data;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Texts Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return TArray<FText>();
#endif
	//
	return TArray<FText>();
}

TArray<FString> USQLite::DB_SELECT_String_Array(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<FString>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Strings Field: %s"),*GetName(),*ColumnName));
		return TArray<FString>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=512"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UArray>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Array;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Array) {
									TArray<FString>Data;
									//
									TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
									TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(VAL.Text);
									//
									if (!FJsonSerializer::Deserialize(JReader,JSON)) {
										LOG_DB(Debug,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object for : [%s]"),*VAL.Name.ToString()));
									break;}
									//
									auto JArray = JSON->GetArrayField(VAL.Name.ToString());
									for (int32 I=0, N=JArray.Num()-1; I<=N; I++) {
										const auto &JValue = JArray[I];
										//
										if (JValue.IsValid()&&(!JValue->IsNull())) {
											const FString Value = JValue->AsString();
											Data.Add(Value);
										}///
									}///
									//
									return Data;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Strings Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return TArray<FString>();
#endif
	//
	return TArray<FString>();
}

TArray<FVector2D> USQLite::DB_SELECT_Vector2D_Array(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<FVector2D>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Vector 2D Field: %s"),*GetName(),*ColumnName));
		return TArray<FVector2D>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=512"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UArray>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Array;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Array) {
									TArray<FVector2D>Data;
									//
									TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
									TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(VAL.Text);
									//
									if (!FJsonSerializer::Deserialize(JReader,JSON)) {
										LOG_DB(Debug,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object for : [%s]"),*VAL.Name.ToString()));
									break;}
									//
									auto JArray = JSON->GetArrayField(VAL.Name.ToString());
									for (int32 I=0, N=JArray.Num()-1; I<=N; I++) {
										const auto &JValue = JArray[I];
										//
										if (JValue.IsValid()&&(!JValue->IsNull())) {
											const auto JContainer = JValue->AsObject();
											if (JContainer.IsValid()) {
												FVector2D Vector;
												Vector.X = JContainer->GetNumberField(TEXT("x"));
												Vector.Y = JContainer->GetNumberField(TEXT("y"));
												//
												Data.Add(Vector);
											}///
										}///
									}///
									//
									return Data;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Vector 2D Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return TArray<FVector2D>();
#endif
	//
	return TArray<FVector2D>();
}

TArray<FVector> USQLite::DB_SELECT_Vector3D_Array(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<FVector>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Vector 3D Field: %s"),*GetName(),*ColumnName));
		return TArray<FVector>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=512"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UArray>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Array;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Array) {
									TArray<FVector>Data;
									//
									TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
									TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(VAL.Text);
									//
									if (!FJsonSerializer::Deserialize(JReader,JSON)) {
										LOG_DB(Debug,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object for : [%s]"),*VAL.Name.ToString()));
									break;}
									//
									auto JArray = JSON->GetArrayField(VAL.Name.ToString());
									for (int32 I=0, N=JArray.Num()-1; I<=N; I++) {
										const auto &JValue = JArray[I];
										//
										if (JValue.IsValid()&&(!JValue->IsNull())) {
											const auto JContainer = JValue->AsObject();
											if (JContainer.IsValid()) {
												FVector Vector;
												Vector.X = JContainer->GetNumberField(TEXT("x"));
												Vector.Y = JContainer->GetNumberField(TEXT("y"));
												Vector.Z = JContainer->GetNumberField(TEXT("z"));
												//
												Data.Add(Vector);
											}///
										}///
									}///
									//
									return Data;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Vector 3D Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return TArray<FVector>();
#endif
	//
	return TArray<FVector>();
}

TArray<FRotator> USQLite::DB_SELECT_Rotator_Array(const FString &RowID, const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<FRotator>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Rotators Field: %s"),*GetName(),*ColumnName));
		return TArray<FRotator>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=512"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UArray>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Array;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Array) {
									TArray<FRotator>Data;
									//
									TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
									TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(VAL.Text);
									//
									if (!FJsonSerializer::Deserialize(JReader,JSON)) {
										LOG_DB(Debug,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object for : [%s]"),*VAL.Name.ToString()));
									break;}
									//
									auto JArray = JSON->GetArrayField(VAL.Name.ToString());
									for (int32 I=0, N=JArray.Num()-1; I<=N; I++) {
										const auto &JValue = JArray[I];
										//
										if (JValue.IsValid()&&(!JValue->IsNull())) {
											const auto JContainer = JValue->AsObject();
											if (JContainer.IsValid()) {
												FRotator Rotator;
												Rotator.Yaw = JContainer->GetNumberField(TEXT("yaw"));
												Rotator.Roll = JContainer->GetNumberField(TEXT("roll"));
												Rotator.Pitch = JContainer->GetNumberField(TEXT("pitch"));
												//
												Data.Add(Rotator);
											}///
										}///
									}///
									//
									return Data;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Rotators Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return TArray<FRotator>();
#endif
	//
	return TArray<FRotator>();
}

TArray<UObject*> USQLite::DB_SELECT_Object_Array(UObject* Context, const FString &RowID, const FString &ColumnName) {
	if (!Context->IsValidLowLevelFast()||Context->GetWorld()==nullptr) {
		LOG_DB(true,ESQLSeverity::Warning,FString(TEXT("[%s] (SELECT Column Command): Target Column Field is Array of Objects, caller must implement and provide a valid 'GetWorld()' override function.")));
		return TArray<UObject*>();
	}///
	//
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<UObject*>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Arrays)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Object Pointers Field: %s"),*GetName(),*ColumnName));
		return TArray<UObject*>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	SQL = SQL+FString::Printf(TEXT(" WHERE %s = '%s'"),*DB_TABLE.Columns[0],*RowID);
	//
	//
	DBSTM Statement;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=512"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UArray>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Array;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Array) {
									TArray<UObject*>Data;
									//
									TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
									TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(VAL.Text);
									//
									if (!FJsonSerializer::Deserialize(JReader,JSON)) {
										LOG_DB(Debug,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object for : [%s]"),*VAL.Name.ToString()));
									break;}
									//
									auto JArray = JSON->GetArrayField(VAL.Name.ToString());
									for (int32 I=0, N=JArray.Num()-1; I<=N; I++) {
										const auto &JValue = JArray[I];
										//
										if (JValue.IsValid()&&(!JValue->IsNull())) {
											const FString Value = JValue->AsString();
											//
											FString Class, Instance;
											Value.Split(TEXT("::"),&Class,&Instance);
											//
											const FSoftObjectPath ClassPath(Class);
											TSoftObjectPtr<UObject>ClassPtr(ClassPath);
											//
											auto CDO = ClassPtr.LoadSynchronous();
											if ((CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
												for (TActorIterator<AActor>Actor(Context->GetWorld()); Actor; ++Actor) {
													if ((*Actor)->GetFullName()==Instance) {Data.Add(*Actor); break;}
												}///
											} else if (CDO!=nullptr) {
												for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
													if ((*OBJ)->GetFullName()==Instance) {Data.Add(*OBJ); break;}
												}///
											}///
										}///
									}///
									//
									return Data;
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not an Array of Object Pointers Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return TArray<UObject*>();
#endif
	//
	return TArray<UObject*>();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TArray<FName> USQLite::DB_SELECT_IDs() {
#if WITH_SERVER_CODE
	if (!DB_TABLE.ColumnTypes.IsValidIndex(0)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL IDs Command): Unnable to read IDs from this Database!"),*GetName()));
		return TArray<FName>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*DB_TABLE.Columns[0],*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*DB_TABLE.Columns[0],*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	//
	DBSTM Statement;
	TArray<FName>Data;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=1024"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::Name;
								const auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[%s]:: %s"),*VAL.Name.ToString(),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Name) {
									Data.Add(*VAL.Text);
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT ALL IDs Command): Target Column Type mismatch; Column is not a Name Field: %s"),*GetName(),*DB_TABLE.Columns[0]));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return Data;
#endif
	//
	return TArray<FName>();
}

TArray<bool> USQLite::DB_SELECT_Booleans(const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<bool>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Bools)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Boolean Field: %s"),*GetName(),*ColumnName));
		return TArray<bool>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	//
	DBSTM Statement;
	TArray<bool>Data;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=1024"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UBool>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Boolean;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[%s]:: %s"),*VAL.Name.ToString(),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Boolean) {
									const bool B = (VAL.Text.Equals(TEXT("true"),ESearchCase::IgnoreCase)) ? true : false;
									Data.Add(B);
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Boolean Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return Data;
#endif
	//
	return TArray<bool>();
}

TArray<uint8> USQLite::DB_SELECT_Bytes(const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<uint8>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Bytes)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Byte Field: %s"),*GetName(),*ColumnName));
		return TArray<uint8>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	//
	DBSTM Statement;
	TArray<uint8>Data;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=1024"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_INTEGER:
							{
								Field.Type = ESQLType::Integer;
								Field.Cast = ESQLReadMode::Integer;
								Field.Integer = sqlite3_column_int64(Statement,Column);
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Integer:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[%s]:: %i"),*VAL.Name.ToString(),VAL.Integer));
								//
								if (VAL.Cast==ESQLReadMode::Integer) {
									Data.Add(VAL.Integer);
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Byte Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return Data;
#endif
	//
	return TArray<uint8>();
}

TArray<int32> USQLite::DB_SELECT_Integers(const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<int32>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Ints)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Integer Field: %s"),*GetName(),*ColumnName));
		return TArray<int32>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	//
	DBSTM Statement;
	TArray<int32>Data;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=1024"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_INTEGER:
							{
								Field.Type = ESQLType::Integer;
								Field.Cast = ESQLReadMode::Integer;
								Field.Integer = sqlite3_column_int64(Statement,Column);
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Integer:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[%s]:: %i"),*VAL.Name.ToString(),VAL.Integer));
								//
								if (VAL.Cast==ESQLReadMode::Integer) {
									Data.Add(VAL.Integer);
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Integer Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return Data;
#endif
	//
	return TArray<int32>();
}

TArray<float> USQLite::DB_SELECT_Floats(const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<float>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Floats)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Float Field: %s"),*GetName(),*ColumnName));
		return TArray<float>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	//
	DBSTM Statement;
	TArray<float>Data;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=1024"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_FLOAT:
							{
								Field.Type = ESQLType::Float;
								Field.Cast = ESQLReadMode::Float;
								Field.Float = sqlite3_column_double(Statement,Column);
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Float:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[%s]:: %f"),*VAL.Name.ToString(),VAL.Float));
								//
								if (VAL.Cast==ESQLReadMode::Float) {
									Data.Add(VAL.Float);
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT Column Command): Target Column Type mismatch; Column is not a Float Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return Data;
#endif
	//
	return TArray<float>();
}

TArray<FName> USQLite::DB_SELECT_Names(const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<FName>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Names)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Name Field: %s"),*GetName(),*ColumnName));
		return TArray<FName>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	//
	DBSTM Statement;
	TArray<FName>Data;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=1024"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UName>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Name;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[%s]:: %s"),*VAL.Name.ToString(),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Name) {
									Data.Add(*VAL.Text);
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Name Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return Data;
#endif
	//
	return TArray<FName>();
}

TArray<FText> USQLite::DB_SELECT_Texts(const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<FText>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Texts)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Text Field: %s"),*GetName(),*ColumnName));
		return TArray<FText>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	//
	DBSTM Statement;
	TArray<FText>Data;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=1024"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UText>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Text;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[%s]:: %s"),*VAL.Name.ToString(),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Text) {
									Data.Add(FText::FromString(VAL.Text));
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Text Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return Data;
#endif
	//
	return TArray<FText>();
}

TArray<FString> USQLite::DB_SELECT_Strings(const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<FString>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Strings)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a String Field: %s"),*GetName(),*ColumnName));
		return TArray<FString>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	//
	DBSTM Statement;
	TArray<FString>Data;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=1024"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UName>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Name;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[%s]:: %s"),*VAL.Name.ToString(),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Name) {
									Data.Add(VAL.Text);
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a String Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return Data;
#endif
	//
	return TArray<FString>();
}

TArray<FDateTime> USQLite::DB_SELECT_Dates(const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<FDateTime>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.DateTimes)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Date Field: %s"),*GetName(),*ColumnName));
		return TArray<FDateTime>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	//
	DBSTM Statement;
	TArray<FDateTime>Data;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=256"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_INTEGER:
							{
								Field.Type = ESQLType::Integer;
								Field.Cast = ESQLReadMode::Integer;
								Field.Integer = sqlite3_column_int64(Statement,Column);
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Integer:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[%s]:: %s"),*VAL.Name.ToString(),*FDateTime::FromUnixTimestamp(VAL.Integer).ToString()));
								//
								if (VAL.Cast==ESQLReadMode::Integer) {
									Data.Add(FDateTime::FromUnixTimestamp(VAL.Integer));
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Date Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return Data;
#endif
	//
	return TArray<FDateTime>();
}

TArray<FColor> USQLite::DB_SELECT_Colors(const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<FColor>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Colors)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Color Field: %s"),*GetName(),*ColumnName));
		return TArray<FColor>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	//
	DBSTM Statement;
	TArray<FColor>Data;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=1024"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UColor>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Color;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[%s]:: %s"),*VAL.Name.ToString(),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Color) {
									TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
									TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(VAL.Text);
									//
									if (!FJsonSerializer::Deserialize(JReader,JSON)) {
										LOG_DB(Debug,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object for : [%s]"),*VAL.Name.ToString()));
									break;}
									//
									FColor Color;
									Color.R = JSON->GetIntegerField(TEXT("r")); Color.G = JSON->GetIntegerField(TEXT("g"));
									Color.B = JSON->GetIntegerField(TEXT("b")); Color.A = JSON->GetIntegerField(TEXT("a"));
									//
									Data.Add(Color);
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Color Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return Data;
#endif
	//
	return TArray<FColor>();
}

TArray<FVector2D> USQLite::DB_SELECT_Vector2Ds(const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<FVector2D>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Vector2D)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Vector 2D Field: %s"),*GetName(),*ColumnName));
		return TArray<FVector2D>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	//
	DBSTM Statement;
	TArray<FVector2D>Data;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=1024"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UVector2D>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Vector2D;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[%s]:: %s"),*VAL.Name.ToString(),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Vector2D) {
									TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
									TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(VAL.Text);
									//
									if (!FJsonSerializer::Deserialize(JReader,JSON)) {
										LOG_DB(Debug,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object for : [%s]"),*VAL.Name.ToString()));
									break;}
									//
									FVector2D Vector;
									Vector.X = JSON->GetIntegerField(TEXT("x"));
									Vector.Y = JSON->GetIntegerField(TEXT("y"));
									//
									Data.Add(Vector);
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Vector 2D Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return Data;
#endif
	//
	return TArray<FVector2D>();
}

TArray<FVector> USQLite::DB_SELECT_Vector3Ds(const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<FVector>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Vector3D)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Vector 3D Field: %s"),*GetName(),*ColumnName));
		return TArray<FVector>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	//
	DBSTM Statement;
	TArray<FVector>Data;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=1024"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UVector3D>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Vector3D;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[%s]:: %s"),*VAL.Name.ToString(),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Vector3D) {
									TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
									TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(VAL.Text);
									//
									if (!FJsonSerializer::Deserialize(JReader,JSON)) {
										LOG_DB(Debug,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object for : [%s]"),*VAL.Name.ToString()));
									break;}
									//
									FVector Vector;
									Vector.X = JSON->GetIntegerField(TEXT("x"));
									Vector.Y = JSON->GetIntegerField(TEXT("y"));
									Vector.Z = JSON->GetIntegerField(TEXT("z"));
									//
									Data.Add(Vector);
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Vector 3D Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return Data;
#endif
	//
	return TArray<FVector>();
}

TArray<FRotator> USQLite::DB_SELECT_Rotators(const FString &ColumnName) {
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<FRotator>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.Rotators)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Rotator Field: %s"),*GetName(),*ColumnName));
		return TArray<FRotator>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	//
	DBSTM Statement;
	TArray<FRotator>Data;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=1024"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("URotator>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::Rotator;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::Rotator) {
									TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
									TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(VAL.Text);
									//
									if (!FJsonSerializer::Deserialize(JReader,JSON)) {
										LOG_DB(Debug,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object for : [%s]"),*VAL.Name.ToString()));
									break;}
									//
									FRotator Rotator;
									Rotator.Pitch = JSON->GetIntegerField(TEXT("pitch"));
									Rotator.Roll = JSON->GetIntegerField(TEXT("roll"));
									Rotator.Yaw = JSON->GetIntegerField(TEXT("yaw"));
									//
									Data.Add(Rotator);
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Rotator Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return Data;
#endif
	//
	return TArray<FRotator>();
}

TArray<UObject*> USQLite::DB_SELECT_Objects(UObject* Context, const FString &ColumnName) {
	if (!Context->IsValidLowLevelFast()||Context->GetWorld()==nullptr) {
		LOG_DB(true,ESQLSeverity::Warning,FString(TEXT("[%s] (SELECT ALL Columns Command): Target Column Field is Object, caller must implement and provide a valid 'GetWorld()' override function.")));
		return TArray<UObject*>();
	}///
	//
#if WITH_SERVER_CODE
	const int32 ID = DB_TABLE.Columns.Find(ColumnName);
	if ((ID==INDEX_NONE)||(!DB_TABLE.ColumnTypes.IsValidIndex(ID))) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Field doesn't exist in this Database: %s"),*GetName(),*ColumnName));
		return TArray<UObject*>();
	}///
	//
	if (!DB_TABLE.ColumnTypes[ID].Equals(USQL::USQLTypes.ObjectPtrs)) {
		LOG_DB(true,ESQLSeverity::Error,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Object Pointer Field: %s"),*GetName(),*ColumnName));
		return TArray<UObject*>();
	}///
	//
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {SQL=FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*GetNameSanitized());}
	else {SQL = FString::Printf(TEXT("SELECT %s FROM %s"),*ColumnName,*(GetNameSanitized()+TEXT("_")+DB_VERSION.CurrentVersion));}
	//
	//
	DBSTM Statement;
	TArray<UObject*>Data;
	char* ErrorMSG = nullptr;
	ESQLResult Result = ESQLResult::ABORT;
	//
	LOG_DB(Debug,ESQLSeverity::Info,SQL);
	if (USQL::DB_OpenDatabase(DB_RUNTIME,DB_FILE)) {
		DB_CACHE(DB_RUNTIME,TEXT("PRAGMA cache_size=1024"));
		DBT_BEGIN(DB_RUNTIME,ErrorMSG);
			Result = static_cast<ESQLResult>(sqlite3_prepare(DB_RUNTIME,TCHAR_TO_UTF8(*SQL),-1,&Statement,nullptr));
			if (Result==ESQLResult::OK) {
				FSQLRow Row; int32 Step=0;
				for (Step=sqlite3_step(Statement); Step==SQLITE_ROW && Step!=SQLITE_DONE; Step=sqlite3_step(Statement)) {
					for (int32 Column=0; Column<sqlite3_column_count(Statement); Column++) {
						FString Name = UTF8_TO_TCHAR(sqlite3_column_name(Statement,Column));
						int32 Type = sqlite3_column_type(Statement,Column);
						FSQLField Field; Field.Name = FName(*Name);
						Field.Type = ESQLType::Unknown;
						//
						switch (Type) {
							case SQLITE_TEXT:
							{
								Field.Type = ESQLType::Text;
								Field.Cast = ESQLReadMode::TimeStamp;
								auto Value = FString(UTF8_TO_TCHAR(sqlite3_column_text(Statement,Column)));
								if (Value.Split(TEXT("UObject>>"),nullptr,&Value)) {Field.Cast=ESQLReadMode::ObjectPtr;}
								Field.Text = Value;
							}	break;
						default: break;}
					if (Field.Type!=ESQLType::Unknown) {Row.Values.Add(Field);}
				}} DB_FINAL(Statement);
				//
				if (Row.Values.Num()>0) {
					for (const auto &VAL : Row.Values) {
						switch (VAL.Type) {
							case ESQLType::Text:
							{
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[FIELD]:: %s"),*VAL.Name.ToString()));
								LOG_DB(Debug,ESQLSeverity::Info,FString::Printf(TEXT("[VALUE]:: %s"),*VAL.Text));
								//
								if (VAL.Cast==ESQLReadMode::ObjectPtr) {
									FString Class, Instance;
									VAL.Text.Split(TEXT("::"),&Class,&Instance);
									//
									const FSoftObjectPath ClassPath(Class);
									TSoftObjectPtr<UObject>ClassPtr(ClassPath);
									//
									auto CDO = ClassPtr.LoadSynchronous();
									if ((CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
										for (TActorIterator<AActor>Actor(Context->GetWorld()); Actor; ++Actor) {
											if ((*Actor)->GetFullName()==Instance) {Data.Add(*Actor); continue;}
										}///
									} else if (CDO!=nullptr) {
										for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
											if ((*OBJ)->GetFullName()==Instance) {Data.Add(*OBJ); continue;}
										}///
									}///
								}///
							}	break;
							//
							default:
								LOG_DB(Debug,ESQLSeverity::Warning,FString::Printf(TEXT("[%s] (SELECT ALL Columns Command): Target Column Type mismatch; Column is not a Object Pointer Field: %s"),*GetName(),*ColumnName));
						break;}///
					}///
				}///
			} else {LOG_DB(true,ESQLSeverity::Error,FString(ANSI_TO_TCHAR(ErrorMSG)));}
		DBT_END(DB_RUNTIME,ErrorMSG);
	USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	return Data;
#endif
	//
	return TArray<UObject*>();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void USQLite::DB_EnqueueSAVE(const FString &SQL) {
#if WITH_SERVER_CODE
	if ((USQL::GetSaveProgress()<100.f)&&(USQL::GetSavesDone()>0))
	{LOG_DB(Debug,ESQLSeverity::Warning,TEXT("DB Save action already in progress... Wait until action is complete, Enqueue for Save aborted!")); return;}
	//
	if (!DBS_QUEUE.Contains(SQL)) {DBS_QUEUE.Add(SQL);}
#endif
}

void USQLite::DB_EnqueueLOAD(const FString &SQL) {
#if WITH_SERVER_CODE
	if ((USQL::GetLoadProgress()<100.f)&&(USQL::GetLoadsDone()>0))
	{LOG_DB(Debug,ESQLSeverity::Warning,TEXT("DB Load action already in progress... Wait until action is complete, Enqueue for Load aborted!")); return;}
	//
	if (!DBL_QUEUE.Contains(SQL)) {DBL_QUEUE.Add(SQL);}
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DB Runtime Interface:

void USQLite::DB_PrepareToSave_Implementation(USQLite* Database, const ESQLSaveMode Mode) {
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill())
	{LOG_DB(Debug,ESQLSeverity::Error,TEXT("DB Object is invalid... Cannot prepare a DB Save action with this USQLite Object's Table!")); return;}
	//
	switch (Mode) {
		case ESQLSaveMode::Insert:
		{
			/// C++ Interface API. Do this in your Class' own PrepareToSave() for auto SQL Code Generation:
			//const auto SQL = Database->DB_GenerateSQL_Object_INSERT(Cast<UObject>(this));
			//Database->DB_EnqueueSAVE(SQL);
		}	break;
		//
		case ESQLSaveMode::Update:
		{
			/// C++ Interface API. Do this in your Class' own PrepareToSave() for auto SQL Code Generation:
			//const auto SQL = Database->DB_GenerateSQL_Object_UPDATE(Cast<UObject>(this));
			//Database->DB_EnqueueSAVE(SQL);
		}	break;
	default: break;}
}

void USQLite::DB_PrepareToLoad_Implementation(USQLite* Database) {
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill())
	{LOG_DB(Debug,ESQLSeverity::Error,TEXT("DB Object is invalid... Cannot prepare a DB Load action with this USQLite Object's Table!")); return;}
	//
	/// C++ Interface API. Do this in your Class' own PrepareToLoad() for auto SQL Code Generation:
	//const auto SQL = Database->DB_GenerateSQL_Object_SELECT(Cast<UObject>(this));
	//Database->DB_EnqueueLOAD(SQL);
}

void USQLite::DB_OnBeginSave_Implementation(USQLite* Database) {
	if (Database!=const_cast<USQLite*>(this)) {return;}
	//
	ENetMode NET = NM_Standalone;
	const UWorld* _World = Database->GetWorld();
	if (_World!=nullptr) {NET=_World->GetNetMode();}
	//
	if (NET != NM_Client) {
		for (TObjectIterator<UObject>Object; Object; ++Object) {
			if (!(*Object)||!(*Object)->IsValidLowLevelFast()||(*Object)->IsPendingKill()) {continue;}
			if ((*Object)->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
			if ((*Object)->IsA(USQLite::StaticClass())) {continue;}
			//
			if ((*Object)->IsA(AActor::StaticClass())) {
				const auto &Actor = CastChecked<AActor>(*Object);
				if (Actor->Role != ROLE_Authority) {continue;}
			} else {
				const auto &Owner = (*Object)->GetTypedOuter<AActor>();
				if (Owner) {
					if (Owner->Role != ROLE_Authority) {continue;}
				}///
			}///
			//
			const auto &Interface = Cast<ISQLiteData>(*Object);
			if (Interface) {
				Interface->Execute_DB_OnBeginSave(*Object,const_cast<USQLite*>(this));
			} else if (Object->GetClass()->ImplementsInterface(USQLiteData::StaticClass())) {
				ISQLiteData::Execute_DB_OnBeginSave(*Object,const_cast<USQLite*>(this));
			}///
		}///
	}///
	//
	EVENT_OnBeginDataSAVE.Broadcast(Database);
}

void USQLite::DB_OnProgressSave_Implementation(USQLite* Database, const float Progress) {
	if (Database!=const_cast<USQLite*>(this)) {return;}
	//
	ENetMode NET = NM_Standalone;
	const UWorld* _World = Database->GetWorld();
	if (_World!=nullptr) {NET=_World->GetNetMode();}
	//
	if (NET != NM_Client) {
		for (TObjectIterator<UObject>Object; Object; ++Object) {
			if (!(*Object)||!(*Object)->IsValidLowLevelFast()||(*Object)->IsPendingKill()) {continue;}
			if ((*Object)->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
			if ((*Object)->IsA(USQLite::StaticClass())) {continue;}
			//
			if ((*Object)->IsA(AActor::StaticClass())) {
				const auto &Actor = CastChecked<AActor>(*Object);
				if (Actor->Role != ROLE_Authority) {continue;}
			} else {
				const auto &Owner = (*Object)->GetTypedOuter<AActor>();
				if (Owner) {
					if (Owner->Role != ROLE_Authority) {continue;}
				}///
			}///
			//
			const auto &Interface = Cast<ISQLiteData>(*Object);
			if (Interface) {
				Interface->Execute_DB_OnProgressSave(*Object,const_cast<USQLite*>(this),Progress);
			} else if (Object->GetClass()->ImplementsInterface(USQLiteData::StaticClass())) {
				ISQLiteData::Execute_DB_OnProgressSave(*Object,const_cast<USQLite*>(this),Progress);
			}///
		}///
	}///
	//
	EVENT_OnProgressDataSAVE.Broadcast(Database,Progress);
}

void USQLite::DB_OnFinishSave_Implementation(USQLite* Database, const bool Success) {
	if (Database!=const_cast<USQLite*>(this)) {return;}
	//
	ENetMode NET = NM_Standalone;
	const UWorld* _World = Database->GetWorld();
	if (_World!=nullptr) {NET=_World->GetNetMode();}
	//
	if (NET != NM_Client) {
		for (TObjectIterator<UObject>Object; Object; ++Object) {
			if (!(*Object)||!(*Object)->IsValidLowLevelFast()||(*Object)->IsPendingKill()) {continue;}
			if ((*Object)->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
			if ((*Object)->IsA(USQLite::StaticClass())) {continue;}
			//
			if ((*Object)->IsA(AActor::StaticClass())) {
				const auto &Actor = CastChecked<AActor>(*Object);
				if (Actor->Role != ROLE_Authority) {continue;}
			} else {
				const auto &Owner = (*Object)->GetTypedOuter<AActor>();
				if (Owner) {
					if (Owner->Role != ROLE_Authority) {continue;}
				}///
			}///
			//
			const auto &Interface = Cast<ISQLiteData>(*Object);
			if (Interface) {
				Interface->Execute_DB_OnFinishSave(*Object,const_cast<USQLite*>(this),Success);
			} else if (Object->GetClass()->ImplementsInterface(USQLiteData::StaticClass())) {
				ISQLiteData::Execute_DB_OnFinishSave(*Object,const_cast<USQLite*>(this),Success);
			}///
		}///
	}///
	//
	if (DB_RUNTIME!=nullptr) {USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	DB_DestroyLoadScreen();
	EVENT_OnFinishDataSAVE.Broadcast(Database,Success);
}

void USQLite::DB_OnBeginLoad_Implementation(USQLite* Database) {
	if (Database!=const_cast<USQLite*>(this)) {return;}
	//
	for (TObjectIterator<UObject>Object; Object; ++Object) {
		if (!(*Object)||!(*Object)->IsValidLowLevelFast()||(*Object)->IsPendingKill()) {continue;}
		if ((*Object)->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
		if ((*Object)->IsA(USQLite::StaticClass())) {continue;}
		//
		const auto &Interface = Cast<ISQLiteData>(*Object);
		if (Interface) {
			Interface->Execute_DB_OnBeginLoad(*Object,const_cast<USQLite*>(this));
		} else if (Object->GetClass()->ImplementsInterface(USQLiteData::StaticClass())) {
			ISQLiteData::Execute_DB_OnBeginLoad(*Object,const_cast<USQLite*>(this));
		}///
	}///
	//
	EVENT_OnBeginDataLOAD.Broadcast(Database);
}

void USQLite::DB_OnProgressLoad_Implementation(USQLite* Database, const float Progress, const FSQLRow Data) {
	if (Database!=const_cast<USQLite*>(this)) {return;}
	//
	for (TObjectIterator<UObject>Object; Object; ++Object) {
		if (!(*Object)||!(*Object)->IsValidLowLevelFast()||(*Object)->IsPendingKill()) {continue;}
		if ((*Object)->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
		if ((*Object)->IsA(USQLite::StaticClass())) {continue;}
		//
		FString ID;
		if ((*Object)->IsA(UActorComponent::StaticClass())) {ID = USQL::MakeComponentDBID(CastChecked<UActorComponent>(*Object));} else
		if ((*Object)->IsA(AActor::StaticClass())) {ID = USQL::MakeActorDBID(CastChecked<AActor>(*Object));}
		else {ID = USQL::MakeObjectDBID(*Object);} if (ID.IsEmpty()||Data.ID!=(*ID)) {continue;}
		//
		const auto &Interface = Cast<ISQLiteData>(*Object);
		if (Interface) {
			Interface->Execute_DB_OnProgressLoad(*Object,const_cast<USQLite*>(this),Progress,Data);
		} else if (Object->GetClass()->ImplementsInterface(USQLiteData::StaticClass())) {
			ISQLiteData::Execute_DB_OnProgressLoad(*Object,const_cast<USQLite*>(this),Progress,Data);
		}///
	}///
	//
	EVENT_OnProgressDataLOAD.Broadcast(Database,Progress,Data);
}

void USQLite::DB_OnFinishLoad_Implementation(USQLite* Database, bool Success) {
	if (Database!=const_cast<USQLite*>(this)) {return;}
	//
	for (TObjectIterator<UObject>Object; Object; ++Object) {
		if (!(*Object)||!(*Object)->IsValidLowLevelFast()||(*Object)->IsPendingKill()) {continue;}
		if ((*Object)->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
		if ((*Object)->IsA(USQLite::StaticClass())) {continue;}
		//
		const auto &Interface = Cast<ISQLiteData>(*Object);
		if (Interface) {
			Interface->Execute_DB_OnFinishLoad(*Object,const_cast<USQLite*>(this),Success);
		} else if (Object->GetClass()->ImplementsInterface(USQLiteData::StaticClass())) {
			ISQLiteData::Execute_DB_OnFinishLoad(*Object,const_cast<USQLite*>(this),Success);
	}} if (DB_RUNTIME!=nullptr) {USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	DB_DestroyLoadScreen();
	EVENT_OnFinishDataLOAD.Broadcast(Database,Success);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DB Threading Interface:

void USQLite::DB_GetThreadSafety(ESQLThreadSafety &ThreadSafety, const FSQLite_ThreadChanged &ThreadChangedCallback) {
	ThreadSafety = USQL::ThreadSafety;
	ThreadChangedDelegate = ThreadChangedCallback;
	if (ThreadSafety==USQL::LastThreadState) {return;}
	ThreadChangedDelegate.ExecuteIfBound(ThreadSafety);
}

void USQLite::DB_CheckThreadState() {
	if (USQL::ThreadSafety!=ESQLThreadSafety::IsCurrentlyThreadSafe) {return;}
	//
	switch (USQL::LastThreadState) {
		case ESQLThreadSafety::AsynchronousSaving:
		{
			DB_OnFinishSave(const_cast<USQLite*>(this),true);
			USQL::LastThreadState = ESQLThreadSafety::IsCurrentlyThreadSafe;
		}	break;
		//
		case ESQLThreadSafety::AsynchronousLoading:
		{
			DB_OnFinishLoad(const_cast<USQLite*>(this),true);
			USQL::LastThreadState = ESQLThreadSafety::IsCurrentlyThreadSafe;
		}	break;
	default: break;}
}

void USQLite::DB_OnProgressSave__Threaded_Implementation(USQLite* Database, const float Progress) {
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	if (Database!=const_cast<USQLite*>(this)) {return;}
	//
	ENetMode NET = NM_Standalone;
	const UWorld* _World = Database->GetWorld();
	if (_World!=nullptr) {NET=_World->GetNetMode();}
	//
	if (NET != NM_Client) {
		for (TObjectIterator<UObject>Object; Object; ++Object) {
			if (!(*Object)||!(*Object)->IsValidLowLevelFast()||(*Object)->IsPendingKill()) {continue;}
			if ((*Object)->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
			if ((*Object)->IsA(USQLite::StaticClass())) {continue;}
			//
			if ((*Object)->IsA(AActor::StaticClass())) {
				const auto &Actor = CastChecked<AActor>(*Object);
				if (Actor->Role != ROLE_Authority) {continue;}
			} else {
				const auto &Owner = (*Object)->GetTypedOuter<AActor>();
				if (Owner) {
					if (Owner->Role != ROLE_Authority) {continue;}
				}///
			}///
			//
			const auto &Interface = Cast<ISQLiteData>(*Object);
			if (Interface) {
				Interface->Execute_DB_OnProgressSave__Threaded(*Object,const_cast<USQLite*>(this),Progress);
			} else if (Object->GetClass()->ImplementsInterface(USQLiteData::StaticClass())) {
				ISQLiteData::Execute_DB_OnProgressSave__Threaded(*Object,const_cast<USQLite*>(this),Progress);
			}///
		}///
	}///
}

void USQLite::DB_OnFinishSave__Threaded_Implementation(USQLite* Database, const bool Success) {
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	if (Database!=const_cast<USQLite*>(this)) {return;}
	//
	ENetMode NET = NM_Standalone;
	const UWorld* _World = Database->GetWorld();
	if (_World!=nullptr) {NET=_World->GetNetMode();}
	//
	if (NET != NM_Client) {
		for (TObjectIterator<UObject>Object; Object; ++Object) {
			if (!(*Object)||!(*Object)->IsValidLowLevelFast()||(*Object)->IsPendingKill()) {continue;}
			if ((*Object)->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
			if ((*Object)->IsA(USQLite::StaticClass())) {continue;}
			//
			if ((*Object)->IsA(AActor::StaticClass())) {
				const auto &Actor = CastChecked<AActor>(*Object);
				if (Actor->Role != ROLE_Authority) {continue;}
			} else {
				const auto &Owner = (*Object)->GetTypedOuter<AActor>();
				if (Owner) {
					if (Owner->Role != ROLE_Authority) {continue;}
				}///
			}///
			//
			const auto &Interface = Cast<ISQLiteData>(*Object);
			if (Interface) {
				Interface->Execute_DB_OnFinishSave__Threaded(*Object,const_cast<USQLite*>(this),Success);
			} else if (Object->GetClass()->ImplementsInterface(USQLiteData::StaticClass())) {
				ISQLiteData::Execute_DB_OnFinishSave__Threaded(*Object,const_cast<USQLite*>(this),Success);
	}}} if (DB_RUNTIME!=nullptr) {USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.DB_OnFinishSave"),STAT_FSimpleDelegateGraphTask_DB_OnFinishSave,STATGROUP_TaskGraphTasks);
	FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
		FSimpleDelegateGraphTask::FDelegate::CreateStatic(&CheckThreadState,Database),
		GET_STATID(STAT_FSimpleDelegateGraphTask_DB_OnFinishSave),
		nullptr, ENamedThreads::GameThread
	);//
}

void USQLite::DB_OnProgressLoad__Threaded_Implementation(USQLite* Database, const FSQLRow Data, const float Progress) {
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	if (Database!=const_cast<USQLite*>(this)) {return;}
	//
	for (TObjectIterator<UObject>Object; Object; ++Object) {
		if (!(*Object)||!(*Object)->IsValidLowLevelFast()||(*Object)->IsPendingKill()) {continue;}
		if ((*Object)->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
		if ((*Object)->IsA(USQLite::StaticClass())) {continue;}
		//
		FString ID;
		if ((*Object)->IsA(UActorComponent::StaticClass())) {ID = USQL::MakeComponentDBID(CastChecked<UActorComponent>(*Object));} else
		if ((*Object)->IsA(AActor::StaticClass())) {ID = USQL::MakeActorDBID(CastChecked<AActor>(*Object));}
		else {ID = USQL::MakeObjectDBID(*Object);} if (ID.IsEmpty()||Data.ID!=(*ID)) {continue;}
		//
		const auto &Interface = Cast<ISQLiteData>(*Object);
		if (Interface) {
			Interface->Execute_DB_OnProgressLoad__Threaded(*Object,const_cast<USQLite*>(this),Data,Progress);
		} else if (Object->GetClass()->ImplementsInterface(USQLiteData::StaticClass())) {
			ISQLiteData::Execute_DB_OnProgressLoad__Threaded(*Object,const_cast<USQLite*>(this),Data,Progress);
		}///
	}///
}

void USQLite::DB_OnFinishLoad__Threaded_Implementation(USQLite* Database, bool Success) {
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	if (Database!=const_cast<USQLite*>(this)) {return;}
	//
	for (TObjectIterator<UObject>Object; Object; ++Object) {
		if (!(*Object)||!(*Object)->IsValidLowLevelFast()||(*Object)->IsPendingKill()) {continue;}
		if ((*Object)->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
		if ((*Object)->IsA(USQLite::StaticClass())) {continue;}
		//
		const auto &Interface = Cast<ISQLiteData>(*Object);
		if (Interface) {
			Interface->Execute_DB_OnFinishLoad__Threaded(*Object,const_cast<USQLite*>(this),Success);
		} else if (Object->GetClass()->ImplementsInterface(USQLiteData::StaticClass())) {
			ISQLiteData::Execute_DB_OnFinishLoad__Threaded(*Object,const_cast<USQLite*>(this),Success);
	}} if (DB_RUNTIME!=nullptr) {USQL::DB_CloseDatabase(DB_RUNTIME);}
	//
	DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.DB_OnFinishLoad"),STAT_FSimpleDelegateGraphTask_DB_OnFinishLoad,STATGROUP_TaskGraphTasks);
	FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
		FSimpleDelegateGraphTask::FDelegate::CreateStatic(&CheckThreadState,Database),
		GET_STATID(STAT_FSimpleDelegateGraphTask_DB_OnFinishLoad),
		nullptr, ENamedThreads::GameThread
	);//
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// USQLite Base Serializable Object Interface:

void USQLSerializable_OBJ::DB_PrepareToSave_Implementation(USQLite* Database, const ESQLSaveMode Mode) {
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	switch (Mode) {
		case ESQLSaveMode::Insert:
		{
			const auto SQL = Database->DB_GenerateSQL_Object_INSERT(Cast<UObject>(this));
			Database->DB_EnqueueSAVE(SQL);
		}	break;
		//
		case ESQLSaveMode::Update:
		{
			const auto SQL = Database->DB_GenerateSQL_Object_UPDATE(Cast<UObject>(this));
			Database->DB_EnqueueSAVE(SQL);
		}	break;
	default: break;}
}

void USQLSerializable_OBJ::DB_PrepareToLoad_Implementation(USQLite* Database) {
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	//
	const auto SQL = Database->DB_GenerateSQL_Object_SELECT(Cast<UObject>(this));
	Database->DB_EnqueueLOAD(SQL);
}

void USQLSerializable_OBJ::DB_OnProgressLoad_Implementation(USQLite* Database, const float Progress, const FSQLRow Data) {
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	if (Data.ID==*USQL::MakeObjectDBID(Cast<UObject>(this))) {Database->DB_UnpackObjectDATA(Cast<UObject>(this),Data,Debug);}
}

void USQLSerializable_OBJ::DB_OnProgressLoad__Threaded_Implementation(USQLite* Database, const FSQLRow Data, const float Progress) {
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	if (Data.ID==*USQL::MakeObjectDBID(Cast<UObject>(this))) {Database->DB_UnpackObjectDATA(Cast<UObject>(this),Data,Debug);}
}

void USQLSerializable_OBJ::DB_OnBeginLoad_Implementation(USQLite* Database){}
void USQLSerializable_OBJ::DB_OnBeginSave_Implementation(USQLite* Database){}
void USQLSerializable_OBJ::DB_OnFinishLoad_Implementation(USQLite* Database, bool Success){}
void USQLSerializable_OBJ::DB_OnFinishSave_Implementation(USQLite* Database, const bool Success){}
void USQLSerializable_OBJ::DB_OnProgressSave_Implementation(USQLite* Database, const float Progress){}
void USQLSerializable_OBJ::DB_OnFinishLoad__Threaded_Implementation(USQLite* Database, bool Success){}
void USQLSerializable_OBJ::DB_OnFinishSave__Threaded_Implementation(USQLite* Database, const bool Success){}
void USQLSerializable_OBJ::DB_OnProgressSave__Threaded_Implementation(USQLite* Database, const float Progress){}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// USQLite Base Serializable Component Interface:

void USQLSerializable_CMP::DB_PrepareToSave_Implementation(USQLite* Database, const ESQLSaveMode Mode) {
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	switch (Mode) {
		case ESQLSaveMode::Insert:
		{
			const auto SQL = Database->DB_GenerateSQL_Component_INSERT(Cast<UActorComponent>(this));
			Database->DB_EnqueueSAVE(SQL);
		}	break;
		//
		case ESQLSaveMode::Update:
		{
			const auto SQL = Database->DB_GenerateSQL_Component_UPDATE(Cast<UActorComponent>(this));
			Database->DB_EnqueueSAVE(SQL);
		}	break;
	default: break;}
}

void USQLSerializable_CMP::DB_PrepareToLoad_Implementation(USQLite* Database) {
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	//
	const auto SQL = Database->DB_GenerateSQL_Component_SELECT(Cast<UActorComponent>(this));
	Database->DB_EnqueueLOAD(SQL);
}

void USQLSerializable_CMP::DB_OnProgressLoad_Implementation(USQLite* Database, const float Progress, const FSQLRow Data) {
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	if (Data.ID==*USQL::MakeComponentDBID(Cast<UActorComponent>(this))) {Database->DB_UnpackComponentDATA(Cast<UActorComponent>(this),Data,Debug);}
}

void USQLSerializable_CMP::DB_OnProgressLoad__Threaded_Implementation(USQLite* Database, const FSQLRow Data, const float Progress) {
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	if (Data.ID==*USQL::MakeComponentDBID(Cast<UActorComponent>(this))) {Database->DB_UnpackComponentDATA(Cast<UActorComponent>(this),Data,Debug);}
}

void USQLSerializable_CMP::DB_OnBeginLoad_Implementation(USQLite* Database){}
void USQLSerializable_CMP::DB_OnBeginSave_Implementation(USQLite* Database){}
void USQLSerializable_CMP::DB_OnFinishLoad_Implementation(USQLite* Database, bool Success){}
void USQLSerializable_CMP::DB_OnFinishSave_Implementation(USQLite* Database, const bool Success){}
void USQLSerializable_CMP::DB_OnProgressSave_Implementation(USQLite* Database, const float Progress){}
void USQLSerializable_CMP::DB_OnFinishLoad__Threaded_Implementation(USQLite* Database, bool Success){}
void USQLSerializable_CMP::DB_OnFinishSave__Threaded_Implementation(USQLite* Database, const bool Success){}
void USQLSerializable_CMP::DB_OnProgressSave__Threaded_Implementation(USQLite* Database, const float Progress){}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// USQLite Base Serializable Actor Interface:

void ASQLSerializable::DB_PrepareToSave_Implementation(USQLite* Database, const ESQLSaveMode Mode) {
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	switch (Mode) {
		case ESQLSaveMode::Insert:
		{
			const auto SQL = Database->DB_GenerateSQL_Actor_INSERT(Cast<AActor>(this));
			Database->DB_EnqueueSAVE(SQL);
		}	break;
		//
		case ESQLSaveMode::Update:
		{
			const auto SQL = Database->DB_GenerateSQL_Actor_UPDATE(Cast<AActor>(this));
			Database->DB_EnqueueSAVE(SQL);
		}	break;
	default: break;}
}

void ASQLSerializable::DB_PrepareToLoad_Implementation(USQLite* Database) {
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	//
	const auto SQL = Database->DB_GenerateSQL_Actor_SELECT(Cast<AActor>(this));
	Database->DB_EnqueueLOAD(SQL);
}

void ASQLSerializable::DB_OnProgressLoad_Implementation(USQLite* Database, const float Progress, const FSQLRow Data) {
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	if (Data.ID==*USQL::MakeActorDBID(Cast<AActor>(this))) {Database->DB_UnpackActorDATA(Cast<AActor>(this),Data,Debug);}
}

void ASQLSerializable::DB_OnProgressLoad__Threaded_Implementation(USQLite* Database, const FSQLRow Data, const float Progress) {
	if (!Database||!Database->IsValidLowLevelFast()||Database->IsPendingKill()) {return;}
	if (Data.ID==*USQL::MakeActorDBID(Cast<AActor>(this))) {Database->DB_UnpackActorDATA(Cast<AActor>(this),Data,Debug);}
}

void ASQLSerializable::DB_OnBeginLoad_Implementation(USQLite* Database){}
void ASQLSerializable::DB_OnBeginSave_Implementation(USQLite* Database){}
void ASQLSerializable::DB_OnFinishLoad_Implementation(USQLite* Database, bool Success){}
void ASQLSerializable::DB_OnFinishSave_Implementation(USQLite* Database, const bool Success){}
void ASQLSerializable::DB_OnProgressSave_Implementation(USQLite* Database, const float Progress){}
void ASQLSerializable::DB_OnFinishLoad__Threaded_Implementation(USQLite* Database, bool Success){}
void ASQLSerializable::DB_OnFinishSave__Threaded_Implementation(USQLite* Database, const bool Success){}
void ASQLSerializable::DB_OnProgressSave__Threaded_Implementation(USQLite* Database, const float Progress){}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Loading-Screen Interface:

void USQLite::DB_LaunchLoadScreen(const ESQLThreadSafety Mode, const FText Info) {
	if (GEngine==nullptr||World==nullptr) {return;}
	//
	const auto &PC = UGameplayStatics::GetPlayerController(World,0);
	if (PC!=nullptr) {
		const auto &HUD = Cast<ASQLoadScreenHUD>(PC->GetHUD());
		if (HUD!=nullptr) {
			switch (LoadScreenMode) {
				case ESQLoadScreenMode::BackgroundBlur:
				{
					if (PauseGameOnLoad) {PC->ServerPause();}
					HUD->DisplayBlurLoadScreenHUD(Mode,Info,FeedbackFont,ProgressBarTint,BackBlurPower);
				}	break;
				//
				case ESQLoadScreenMode::SplashScreen:
				{
					if (PauseGameOnLoad) {PC->ServerPause();}
					HUD->DisplaySplashLoadScreenHUD(Mode,Info,FeedbackFont,ProgressBarTint,SplashImage,SplashStretch);
				}	break;
				//
				case ESQLoadScreenMode::MoviePlayer:
				{
					if (PauseGameOnLoad) {PC->ServerPause();}
					HUD->DisplayMovieLoadScreenHUD(Mode,Info,FeedbackFont,ProgressBarTint,SplashMovie,ProgressBarOnMovie);
				}	break;
			default: break;}
		}///
	}///
}

void USQLite::DB_DestroyLoadScreen() {
	if (GEngine==nullptr||World==nullptr||!World->IsValidLowLevel()) {return;}
	//
	const auto &PC = UGameplayStatics::GetPlayerController(World,0);
	if (PC!=nullptr) {
		const auto &HUD = Cast<ASQLoadScreenHUD>(PC->GetHUD());
		if (HUD!=nullptr) {
			switch (LoadScreenMode) {
				case ESQLoadScreenMode::BackgroundBlur:
				{
					if (PauseGameOnLoad) {PC->SetPause(false);}
					HUD->RemoveBlurLoadScreenHUD();
				}	break;
				//
				case ESQLoadScreenMode::SplashScreen:
				{
					if (PauseGameOnLoad) {PC->SetPause(false);}
					HUD->RemoveSplashLoadScreenHUD();
				}	break;
				//
				case ESQLoadScreenMode::MoviePlayer:
				{
					if (PauseGameOnLoad) {PC->SetPause(false);}
					HUD->RemoveMovieLoadScreenHUD();
				}	break;
			default: break;}
		}///
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Editor Only:

#if WITH_EDITOR
//
void USQLite::OnTableDirty() {
	if (DB_VERSION.CurrentVersion.IsEmpty()) {
		USQL::DB_Execute(DB_RUNTIME,DB_FILE,FString::Printf(TEXT("DROP TABLE IF EXISTS %s"),*GetNameSanitized()));
	} else {USQL::DB_Execute(DB_RUNTIME,DB_FILE,FString::Printf(TEXT("DROP TABLE IF EXISTS %s"),*((GetNameSanitized()+TEXT("_"))+DB_VERSION.CurrentVersion)));}
	USQL::DB_Execute(DB_RUNTIME,DB_FILE,DB_CreateTableSQL());
	//
	DBS_QUEUE.Empty();
	DBL_QUEUE.Empty();
	DB_DataPreview.Rows.Empty();
	//
	const auto &Settings = GetMutableDefault<USQLite_Settings>();
	//
	Settings->SaveConfig();
	Settings->UpdateDefaultConfigFile();
}///
//
void USQLite::OnPreviewDelete(const FString RowID) {
	DB_TABLE.Lock = true;
	//
	FString SQL;
	if (DB_VERSION.CurrentVersion.IsEmpty()) {
		SQL = FString::Printf(TEXT("DELETE FROM %s WHERE %s = '%s'"),*GetNameSanitized(),*DB_TABLE.Columns[0],*RowID);
	} else {SQL = FString::Printf(TEXT("DELETE FROM %s WHERE %s = '%s'"),*((GetNameSanitized()+TEXT("_"))+DB_VERSION.CurrentVersion),*DB_TABLE.Columns[0],*RowID);}
	//
	auto Result = USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
	if (Result==ESQLResult::OK) {DB_RefreshPreview();}
}///
//
void USQLite::OnPreviewPaste(const FString FromID, const FString ToID) {
	DB_TABLE.Lock = true;
	//
	FString SQL;
	for (int32 I=1; I<DB_TABLE.Columns.Num(); ++I) {
		if (DB_VERSION.CurrentVersion.IsEmpty()) {
			SQL += FString::Printf(TEXT("UPDATE %s SET %s = (SELECT %s FROM %s WHERE %s = '%s') WHERE %s = '%s'"),*GetNameSanitized(),*DB_TABLE.Columns[I],*DB_TABLE.Columns[I],*GetNameSanitized(),*DB_TABLE.Columns[0],*FromID,*DB_TABLE.Columns[0],*ToID)+TEXT("; ");
		} else {
			SQL += FString::Printf(TEXT("UPDATE %s SET %s = (SELECT %s FROM %s WHERE %s = '%s') WHERE %s = '%s'"),*((GetNameSanitized()+TEXT("_"))+DB_VERSION.CurrentVersion),*DB_TABLE.Columns[I],*DB_TABLE.Columns[I],*((GetNameSanitized()+TEXT("_"))+DB_VERSION.CurrentVersion),*DB_TABLE.Columns[0],*FromID,*DB_TABLE.Columns[0],*ToID)+TEXT("; ");
		}///
	}///
	//
	auto Result = USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
	if (Result==ESQLResult::OK) {DB_RefreshPreview();}
}///
//
void USQLite::OnPreviewInsert(const FString SQL) {
	DB_TABLE.Lock = true;
	//
	auto Result = USQL::DB_Execute(DB_RUNTIME,DB_FILE,SQL);
	if (Result==ESQLResult::OK) {DB_RefreshPreview();}
}
//
void USQLite::OnVersioningDelete() {
	if (!DB_VERSION.CurrentVersion.IsEmpty()) {
		USQL::DB_Execute(DB_RUNTIME,DB_FILE,FString::Printf(TEXT("DROP TABLE IF EXISTS %s"),*((GetNameSanitized()+TEXT("_"))+DB_VERSION.CurrentVersion)));
		//
		DBS_QUEUE.Empty();
		DBL_QUEUE.Empty();
		DB_DataPreview.Rows.Empty();
	}///
}///
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
//
void USQLite_Settings::PostEditChangeProperty(FPropertyChangedEvent &PECP) {
	FName Property = (PECP.Property != nullptr) ? PECP.Property->GetFName() : NAME_None;
	if (Property==GET_MEMBER_NAME_CHECKED(USQLite_Settings,DeepLogs)) {
		USQL::Logs = DeepLogs;
	}///
}///
//
void USQLite::PostEditChangeProperty(struct FPropertyChangedEvent &PECP) {
	FName Property = (PECP.Property != nullptr) ? PECP.Property->GetFName() : NAME_None;
	//
	if (Property==GET_MEMBER_NAME_CHECKED(USQLite,DB_VERSIONS)) {
		DB_VERSION.TargetVersions = DB_VERSIONS.Array();
		DB_VERSION.CurrentVersion.Empty();
	}///
	//
	for (auto VS : DB_VERSIONS) {
		if (VS.Equals(TEXT("DEFAULT"),ESearchCase::IgnoreCase)) {DB_VERSIONS.Remove(VS); DB_VERSION.TargetVersions=DB_VERSIONS.Array();}
	}	if (!DB_VERSIONS.Contains(DB_VERSION.CurrentVersion)) {DB_VERSION.CurrentVersion.Empty();}
	//
	if (!DB_FILE.Contains(GetNameSanitized(),ESearchCase::CaseSensitive)) {DB_Setup();}
	//
	//
	Super::PostEditChangeProperty(PECP);
}///
//
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////