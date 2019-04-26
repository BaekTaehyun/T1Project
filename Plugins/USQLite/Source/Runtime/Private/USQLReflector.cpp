//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "USQLReflector.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Reflector Routines:

bool USQLReflector::Logs;

bool USQLReflector::IsSupportedProperty(const UProperty* Property) {
	return (
		Property->IsA(UStrProperty::StaticClass()) ||
		Property->IsA(UIntProperty::StaticClass()) ||
		Property->IsA(USetProperty::StaticClass()) ||
		Property->IsA(UMapProperty::StaticClass()) ||
		Property->IsA(UBoolProperty::StaticClass()) ||
		Property->IsA(UByteProperty::StaticClass()) ||
		Property->IsA(UEnumProperty::StaticClass()) ||
		Property->IsA(UNameProperty::StaticClass()) ||
		Property->IsA(UTextProperty::StaticClass()) ||
		Property->IsA(UFloatProperty::StaticClass()) ||
		Property->IsA(UArrayProperty::StaticClass()) ||
		Property->IsA(UStructProperty::StaticClass()) ||
		Property->IsA(UObjectProperty::StaticClass())
	);//
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Property Reflector:

FString USQLReflector::UPropertyToSQL(UBoolProperty* Property, const UObject* Container) {
	check(Property); check(Container);
	//
	const auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	const bool Value = Property->GetPropertyValue(ValuePtr);
	//
	return (Value==true) ? TEXT("'UBool>>true'") : TEXT("'UBool>>false'");
}

FString USQLReflector::UPropertyToSQL(UByteProperty* Property, const UObject* Container) {
	check(Property); check(Container);
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	return FString::Printf(TEXT("%i"),Property->GetUnsignedIntPropertyValue(ValuePtr));
}

FString USQLReflector::UPropertyToSQL(UIntProperty* Property, const UObject* Container) {
	check(Property); check(Container);
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	return FString::Printf(TEXT("%i"),Property->GetSignedIntPropertyValue(ValuePtr));
}

FString USQLReflector::UPropertyToSQL(UFloatProperty* Property, const UObject* Container) {
	check(Property); check(Container);
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	return FString::Printf(TEXT("%f"),Property->GetFloatingPointPropertyValue(ValuePtr));
}

FString USQLReflector::UPropertyToSQL(UEnumProperty* Property, const UObject* Container) {
	check(Property); check(Container);
	//
	FString Output = TEXT("None");
	UEnum* Enum = Property->GetEnum();
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	Output = Enum->GetNameStringByValue(Property->GetUnderlyingProperty()->GetUnsignedIntPropertyValue(ValuePtr));
	//
	return FString::Printf(TEXT("'UEnum>>%s'"),*Output);
}

FString USQLReflector::UPropertyToSQL(UNameProperty* Property, const UObject* Container) {
	check(Property); check(Container);
	//
	FString Output = TEXT("None");
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	Output = Property->GetPropertyValue(ValuePtr).ToString();
	Output = Output.Replace(TEXT("'"),TEXT("`"));
	//
	return FString::Printf(TEXT("'UName>>%s'"),*Output);
}

FString USQLReflector::UPropertyToSQL(UTextProperty* Property, const UObject* Container) {
	check(Property); check(Container);
	//
	FString Output = TEXT("None");
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	Output = Property->GetPropertyValue(ValuePtr).ToString();
	Output = Output.Replace(TEXT("'"),TEXT("`"));
	//
	return FString::Printf(TEXT("'UText>>%s'"),*Output);
}

FString USQLReflector::UPropertyToSQL(UStrProperty* Property, const UObject* Container, const ESQLWriteMode Mode) {
	check(Property); check(Container);
	FString Output = TEXT("None");
	//
	switch (Mode) {
		case ESQLWriteMode::String:
		{
			auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
			Output = Property->GetPropertyValue(ValuePtr);
			Output = Output.Replace(TEXT("'"),TEXT("`"));
			return FString::Printf(TEXT("'UString>>%s'"),*Output);
		}	break;
		//
		case ESQLWriteMode::TimeStamp:
			Output = FString(TEXT("DATETIME('now','localtime')")); return Output;
		break;
	default: break;}
	//
	return TEXT("'UString>>NULL'");
}

FString USQLReflector::UPropertyToSQL(UArrayProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	auto ArrayPtr = Property->ContainerPtrToValuePtr<void>(Container);
	FScriptArrayHelper Helper(Property,ArrayPtr);
	//
	FString Output; TArray<TSharedPtr<FJsonValue>>Out;
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	//
	if (Property->Inner->IsA(UObjectProperty::StaticClass())) {
		auto Objects = *Property->ContainerPtrToValuePtr<TArray<UObject*>>(Container);
		for (auto OBJ : Objects) {
			if ((OBJ!=nullptr)&&OBJ->IsValidLowLevelFast()) {
				auto ObjectPath = OBJ->GetFullName();
				auto ClassPath = OBJ->GetClass()->GetDefaultObject()->GetPathName();
				TSharedPtr<FJsonValue>JValue = MakeShareable(new FJsonValueString(ClassPath+TEXT("::")+ObjectPath));
		Out.Add(JValue);}}
	} else {
		for (int32 I=0, N=Helper.Num()-1; I<=N; I++) {
			auto Value = FJsonObjectConverter::UPropertyToJsonValue(Property->Inner,Helper.GetRawPtr(I),0,CPF_Transient);
			if (Value.IsValid()&&(!Value->IsNull())) {Out.Push(Value);}
	}} if (Out.Num()>0) {JSON->SetArrayField(Property->GetName(),Out);}
	//
	FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);
	return FString::Printf(TEXT("'UArray>>%s'"),*Output);
}

FString USQLReflector::UPropertyToSQL(USetProperty* Property, const UObject* Container) {
	check(Property); check(Container);
	//
	auto SetPtr = Property->ContainerPtrToValuePtr<void>(Container);
	FScriptSetHelper Helper(Property,SetPtr);
	//
	FString Output; TArray<TSharedPtr<FJsonValue>>Out;
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	//
	if (Property->ElementProp->IsA(UObjectProperty::StaticClass())) {
		auto Objects = *Property->ContainerPtrToValuePtr<TSet<UObject*>>(Container);
		for (auto OBJ : Objects) {
			if ((OBJ!=nullptr)&&OBJ->IsValidLowLevelFast()) {
				auto ObjectPath = OBJ->GetFullName();
				auto ClassPath = OBJ->GetClass()->GetDefaultObject()->GetPathName();
				TSharedPtr<FJsonValue>JValue = MakeShareable(new FJsonValueString(ClassPath+TEXT("::")+ObjectPath));
		Out.Add(JValue);}}
	} else {
		for (int32 I=0, N=Helper.Num()-1; I<=N; I++) {
			auto Value = FJsonObjectConverter::UPropertyToJsonValue(Property->ElementProp,Helper.GetElementPtr(I),0,CPF_Transient);
			if (Value.IsValid()&&(!Value->IsNull())) {Out.Push(Value);}
	}} if (Out.Num()>0) {JSON->SetArrayField(Property->GetName(),Out);}
	//
	FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);
	return FString::Printf(TEXT("'USet>>%s'"),*Output);
}

FString USQLReflector::UPropertyToSQL(UMapProperty* Property, const UObject* Container) {
	check(Property); check(Container);
	//
	auto MapPtr = Property->ContainerPtrToValuePtr<void>(Container);
	FScriptMapHelper Helper(Property,MapPtr);
	//
	FString Output;
	TArray<TSharedPtr<FJsonValue>>Keys;
	TArray<TSharedPtr<FJsonValue>>Values;
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	//
	if (Property->KeyProp->IsA(UObjectProperty::StaticClass())) {
		auto Objects = *Property->ContainerPtrToValuePtr<TMap<UObject*,void*>>(Container);
		for (auto OBJ : Objects) {
			if ((OBJ.Key!=nullptr)&&OBJ.Key->IsValidLowLevelFast()) {
				auto ObjectPath = OBJ.Key->GetFullName();
				auto ClassPath = OBJ.Key->GetClass()->GetDefaultObject()->GetPathName();
				TSharedPtr<FJsonValue>JKey = MakeShareable(new FJsonValueString(ClassPath+TEXT("::")+ObjectPath));
		Keys.Add(JKey);}}
	} else {
		for (int32 I=0, N=Helper.Num()-1; I<=N; I++) {
			auto Key = FJsonObjectConverter::UPropertyToJsonValue(Property->KeyProp,Helper.GetKeyPtr(I),0,CPF_Transient);
			if (Key.IsValid()&&(!Key->IsNull())) {Keys.Push(Key);}
		}///
	}///
	//
	if ((Property->KeyProp->IsA(UNameProperty::StaticClass()))&&(Property->ValueProp->IsA(UObjectProperty::StaticClass()))) {
		auto Objects = *Property->ContainerPtrToValuePtr<TMap<FName,UObject*>>(Container);
		for (auto OBJ : Objects) {
			if ((OBJ.Value!=nullptr)&&OBJ.Value->IsValidLowLevelFast()) {
				auto ObjectPath = OBJ.Value->GetFullName();
				auto ClassPath = OBJ.Value->GetClass()->GetDefaultObject()->GetPathName();
				TSharedPtr<FJsonValue>JValue = MakeShareable(new FJsonValueString(ClassPath+TEXT("::")+ObjectPath));
		Values.Add(JValue);}}
	} else if ((Property->KeyProp->IsA(UObjectProperty::StaticClass()))&&(Property->ValueProp->IsA(UObjectProperty::StaticClass()))) {
		auto Objects = *Property->ContainerPtrToValuePtr<TMap<UObject*,UObject*>>(Container);
		for (auto OBJ : Objects) {
			if ((OBJ.Value!=nullptr)&&OBJ.Value->IsValidLowLevelFast()) {
				auto ObjectPath = OBJ.Value->GetFullName();
				auto ClassPath = OBJ.Value->GetClass()->GetDefaultObject()->GetPathName();
				TSharedPtr<FJsonValue>JValue = MakeShareable(new FJsonValueString(ClassPath+TEXT("::")+ObjectPath));
		Values.Add(JValue);}}
	} else if (Property->ValueProp->IsA(UObjectProperty::StaticClass())) {
		LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("Property Map Value Type <-> [Data Map Value Type] mismatch:  %s"),*Property->GetName()));
		LOG_DB(Logs,ESQLSeverity::Error,TEXT("Only  TMap<UObject*,UObject*>  OR  TMap<FName,UObject*>  are supported for Maps of Object References!"));
	} else {
		for (int32 I=0, N=Helper.Num()-1; I<=N; I++) {
			auto Value = FJsonObjectConverter::UPropertyToJsonValue(Property->ValueProp,Helper.GetValuePtr(I),0,CPF_Transient);
			if (Value.IsValid()&&(!Value->IsNull())) {Values.Push(Value);}
		}///
	}///
	//
	if ((Keys.Num()>0)&&(Values.Num()>0)) {
		JSON->SetArrayField(Property->GetName()+TEXT("_KEY"),Keys);
		JSON->SetArrayField(Property->GetName()+TEXT("_VALUE"),Values);
	}///
	//
	FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);
	return FString::Printf(TEXT("'UMap>>%s'"),*Output);
}

FString USQLReflector::UPropertyToSQL(UStructProperty* Property, const UObject* Container, const ESQLWriteMode Mode) {
	check(Property); check(Container); FString Output;
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	//
	switch (Mode) {
		case ESQLWriteMode::DateTime:
		{
			auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
			TSharedPtr<FJsonValue>_JSON = FJsonObjectConverter::UPropertyToJsonValue(CastChecked<UProperty>(Property),ValuePtr,CPF_SaveGame,CPF_Transient);
			_JSON.Get()->TryGetString(Output); FDateTime Value; FDateTime::Parse(Output,Value);
			Output = FString::Printf(TEXT("%i"),Value.ToUnixTimestamp()); return Output;
		}	break;
		//
		case ESQLWriteMode::Color:
		{
			auto StructPtr = Property->ContainerPtrToValuePtr<void>(Container);
			auto Value = FJsonObjectConverter::UPropertyToJsonValue(CastChecked<UProperty>(Property),StructPtr,CPF_SaveGame,CPF_Transient);
			if (Value.IsValid()&&(!Value->IsNull())) {JSON->SetField(Property->GetName(),Value); FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);}
			return FString::Printf(TEXT("'UColor>>%s'"),*Output);
		}	break;
		//
		case ESQLWriteMode::Vector2D:
		{
			if (Property->Struct!=TBaseStructure<FVector2D>::Get()) {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Property Type <-> Column Type] mismatch:  %s"),*Property->GetName())); break;}
			auto StructPtr = Property->ContainerPtrToValuePtr<void>(Container);
			auto Value = FJsonObjectConverter::UPropertyToJsonValue(CastChecked<UProperty>(Property),StructPtr,CPF_SaveGame,CPF_Transient);
			if (Value.IsValid()&&(!Value->IsNull())) {JSON->SetField(Property->GetName(),Value); FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);}
			return FString::Printf(TEXT("'UVector2D>>%s'"),*Output);
		}	break;
		//
		case ESQLWriteMode::Vector3D:
		{
			if (Property->Struct!=TBaseStructure<FVector>::Get()) {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Property Type <-> Column Type] mismatch:  %s"),*Property->GetName())); break;}
			auto StructPtr = Property->ContainerPtrToValuePtr<void>(Container);
			auto Value = FJsonObjectConverter::UPropertyToJsonValue(CastChecked<UProperty>(Property),StructPtr,CPF_SaveGame,CPF_Transient);
			if (Value.IsValid()&&(!Value->IsNull())) {JSON->SetField(Property->GetName(),Value); FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);}
			return FString::Printf(TEXT("'UVector3D>>%s'"),*Output);
		}	break;
		//
		case ESQLWriteMode::Rotator:
		{
			if (Property->Struct!=TBaseStructure<FRotator>::Get()) {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Property Type <-> Column Type] mismatch:  %s"),*Property->GetName())); break;}
			auto StructPtr = Property->ContainerPtrToValuePtr<void>(Container);
			auto Value = FJsonObjectConverter::UPropertyToJsonValue(CastChecked<UProperty>(Property),StructPtr,CPF_SaveGame,CPF_Transient);
			if (Value.IsValid()&&(!Value->IsNull())) {JSON->SetField(Property->GetName(),Value); FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);}
			return FString::Printf(TEXT("'URotator>>%s'"),*Output);
		}	break;
		//
		case ESQLWriteMode::Struct:
		{
			if (Property->Struct==FRuntimeFloatCurve::StaticStruct()) {
				auto StructPtr = Property->ContainerPtrToValuePtr<FRuntimeFloatCurve>(Container);
				for (TFieldIterator<UProperty>IT(Property->Struct); IT; ++IT) {
					UProperty* Field = *IT;
					for (int32 I=0; I<Field->ArrayDim; ++I) {
						auto ValuePtr = Field->ContainerPtrToValuePtr<FRichCurve>(StructPtr,I);
						if (UStructProperty* CurveData = Cast<UStructProperty>(Field)) {ParseStructToJSON(JSON,CurveData,ValuePtr);}
				}} FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer);
				return FString::Printf(TEXT("'UFloatCurve>>%s'"),*Output);
			break;}
			//
			auto StructPtr = Property->ContainerPtrToValuePtr<void>(Container);
			ParseStructToJSON(Output,Property,StructPtr);
			return FString::Printf(TEXT("'UStruct>>%s'"),*Output);
		}	break;
	default: break;}
	//
	return TEXT("'UStruct>>NULL'");
}

FString USQLReflector::UPropertyToSQL(UObjectProperty* Property, const UObject* Container, const ESQLWriteMode Mode) {
	check(Property); check(Container);
	//
	FString Output;
	switch (Mode) {
		case ESQLWriteMode::ObjectPtr:
		{
			auto OBJ = *Property->ContainerPtrToValuePtr<UObject*>(Container);
			if ((OBJ!=nullptr)&&OBJ->IsValidLowLevelFast()) {
				auto ObjectPath = OBJ->GetFullName();
				auto ClassPath = OBJ->GetClass()->GetDefaultObject()->GetPathName();
				//
				Output = (ClassPath+TEXT("::")+ObjectPath);
				return FString::Printf(TEXT("'UObject>>%s'"),*Output);
			}///
		}	break;
	default: break;}
	//
	return TEXT("'UObject>>NULL'");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Property Parser:

void USQLReflector::SQLToUProperty(const FSQLField &Field, UBoolProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	if (Field.Text.ToLower()==TEXT("true")) {
		Property->SetPropertyValue(ValuePtr,true);
	return;}
	//
	Property->SetPropertyValue(ValuePtr,false);
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, UByteProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	Property->SetIntPropertyValue(ValuePtr,Field.Integer);
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, UIntProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	Property->SetIntPropertyValue(ValuePtr,Field.Integer);
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, UFloatProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	Property->SetFloatingPointPropertyValue(ValuePtr,Field.Float);
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, UNameProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	Property->SetPropertyValue(ValuePtr,FName(*Field.Text));
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, UTextProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	Property->SetPropertyValue(ValuePtr,FText::FromString(Field.Text));
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, UStrProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	Property->SetPropertyValue(ValuePtr,Field.Text);
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, UEnumProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	UEnum* Enum = Property->GetEnum();
	auto Value = Enum->GetValueByName(*Field.Text);
	if (Value==INDEX_NONE) {Value=0;}
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
	Property->GetUnderlyingProperty()->SetIntPropertyValue(ValuePtr,Value);
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, UArrayProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(Field.Text);
	if (!FJsonSerializer::Deserialize(JReader,JSON)||!JSON.IsValid()) {
		LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object String for : [%s]"),*Property->GetName()));
	return;}
	//
	auto ArrayPtr = Property->ContainerPtrToValuePtr<void>(Container);
	FScriptArrayHelper Helper(Property,ArrayPtr);
	//
	if (JSON->HasField(Field.Name.ToString())) {
		auto JArray = JSON->GetArrayField(Field.Name.ToString());
		Helper.Resize(JArray.Num());
		//
		if (Property->Inner->IsA(UObjectProperty::StaticClass())) {
			TArray<UObject*>&_REF = *Property->ContainerPtrToValuePtr<TArray<UObject*>>(Container);
			for (int32 I=0, N=JArray.Num()-1; I<=N; I++) {
				const auto &JValue = JArray[I];
				if (JValue.IsValid()&&(!JValue->IsNull())) {
					FString Class, Instance;
					JValue->AsString().Split(TEXT("::"),&Class,&Instance);
					//
					const FSoftObjectPath ClassPath(Class);
					TSoftObjectPtr<UObject>ClassPtr(ClassPath);
					//
					auto CDO = ClassPtr.LoadSynchronous();
					if (IsInGameThread()&&(CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
						for (TActorIterator<AActor>Actor(Container->GetWorld()); Actor; ++Actor) {
							if ((*Actor)->GetFullName()==Instance) {_REF[I]=(*Actor); break;}
						}///
					} else if ((CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
						DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUArrayProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUArrayProperty,STATGROUP_TaskGraphTasks);
						FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
							FSimpleDelegateGraphTask::FDelegate::CreateStatic(&RestoreActorReferenceIntoArrayInGameThread,Property,Container,Instance,I),
							GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUArrayProperty),
							nullptr, ENamedThreads::GameThread
						);//
					} else if (IsInGameThread()&&(CDO!=nullptr)) {
						for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
							if ((*OBJ)->GetFullName()==Instance) {_REF[I]=(*OBJ); break;}
						} if (_REF[I]==nullptr) {_REF[I]=NewObject<UObject>(Container,CDO->GetClass(),NAME_None,RF_NoFlags,CDO);}
					} else if (CDO!=nullptr) {
						DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUArrayProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUArrayProperty,STATGROUP_TaskGraphTasks);
						FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
							FSimpleDelegateGraphTask::FDelegate::CreateStatic(&NewObjectIntoArrayInGameThread,Property,Container,CDO,Instance,I),
							GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUArrayProperty),
							nullptr, ENamedThreads::GameThread
						);//
					}///
			}}//////
		} else {
			for (int32 I=0, N=JArray.Num()-1; I<=N; I++) {
				const auto &JValue = JArray[I];
				if (JValue.IsValid()&&(!JValue->IsNull())) {
					if (!FJsonObjectConverter::JsonValueToUProperty(JValue,Property->Inner,Helper.GetRawPtr(I),0,CPF_Transient)) {
						LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Array Element Type <-> Property Array Element Type] mismatch:  %s"),*Property->GetName()));
	}}}}} else {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Array <-> Property Array] mismatch:  %s"),*Property->GetName()));}
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, USetProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(Field.Text);
	if (!FJsonSerializer::Deserialize(JReader,JSON)||!JSON.IsValid()) {
			LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object String for : [%s]"),*Property->GetName()));
	return;}
	//
	auto SetPtr = Property->ContainerPtrToValuePtr<void>(Container);
	FScriptSetHelper Helper(Property,SetPtr);
	//
	if (JSON->HasField(Field.Name.ToString())) {
		auto JSet = JSON->GetArrayField(Field.Name.ToString());
		Helper.EmptyElements();
		//
		if (Property->ElementProp->IsA(UObjectProperty::StaticClass())) {
			TSet<UObject*>&_REF = *Property->ContainerPtrToValuePtr<TSet<UObject*>>(Container);
			for (int32 I=0, N=JSet.Num()-1; I<=N; I++) {
				const auto &JValue = JSet[I];
				if (JValue.IsValid()&&(!JValue->IsNull())) {
					FString Class, Instance;
					JValue->AsString().Split(TEXT("::"),&Class,&Instance);
					//
					const FSoftObjectPath ClassPath(Class);
					TSoftObjectPtr<UObject>ClassPtr(ClassPath);
					//
					auto CDO = ClassPtr.LoadSynchronous();
					if (IsInGameThread()&&(CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
						for (TActorIterator<AActor>Actor(Container->GetWorld()); Actor; ++Actor) {
							if ((*Actor)->GetFullName()==Instance) {_REF.Add(*Actor); break;}
						}///
					} else if ((CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
						DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUSetProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUSetProperty,STATGROUP_TaskGraphTasks);
						FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
							FSimpleDelegateGraphTask::FDelegate::CreateStatic(&RestoreActorReferenceIntoSetInGameThread,Property,Container,Instance),
							GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUSetProperty),
							nullptr, ENamedThreads::GameThread
						);//
					} else if (IsInGameThread()&&(CDO!=nullptr)) {
						bool IsSet = false;
						for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
							if ((*OBJ)->GetFullName()==Instance) {_REF.Add(*OBJ); IsSet=true; break;}
						} if (!IsSet) {_REF.Add(NewObject<UObject>(Container,CDO->GetClass(),NAME_None,RF_NoFlags,CDO));}
					} else if (CDO!=nullptr) {
						DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUSetProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUSetProperty,STATGROUP_TaskGraphTasks);
						FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
							FSimpleDelegateGraphTask::FDelegate::CreateStatic(&NewObjectIntoSetInGameThread,Property,Container,CDO,Instance),
							GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUSetProperty),
							nullptr, ENamedThreads::GameThread
						);//
			}}}/////////
		} else {
			for (int32 I=0, N=JSet.Num()-1; I<=N; I++) {
				const auto &JValue = JSet[I];
				auto Index = Helper.AddDefaultValue_Invalid_NeedsRehash();
				if (JValue.IsValid()&&(!JValue->IsNull())) {
					if (!FJsonObjectConverter::JsonValueToUProperty(JValue,Property->ElementProp,Helper.GetElementPtr(Index),0,CPF_Transient)) {
						LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Set Element Type <-> Property Set Element Type] mismatch:  %s"),*Property->GetName()));
		}}}} Helper.Rehash();
	} else {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Set Member <-> Property Set Member] mismatch:  %s"),*Property->GetName()));}
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, UMapProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(Field.Text);
	if (!FJsonSerializer::Deserialize(JReader,JSON)||!JSON.IsValid()) {
			LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object String for : [%s]"),*Property->GetName()));
	return;}
	//
	auto MapPtr = Property->ContainerPtrToValuePtr<void>(Container);
	FScriptMapHelper Helper(Property,MapPtr);
	//
	TArray<TSharedPtr<FJsonValue>>JKeys;
	if (JSON->HasField(Field.Name.ToString()+TEXT("_KEY"))) {
		JKeys = JSON->GetArrayField(Field.Name.ToString()+TEXT("_KEY"));
		Helper.EmptyValues();
		//
		if (Property->KeyProp->IsA(UObjectProperty::StaticClass())) {
			TMap<UObject*,void*>&_REF = *Property->ContainerPtrToValuePtr<TMap<UObject*,void*>>(Container);
			for (int32 I=0, N=JKeys.Num()-1; I<=N; I++) {
				const auto &JKey = JKeys[I];
				if (JKey.IsValid()&&(!JKey->IsNull())) {
					FString Class, Instance;
					JKey->AsString().Split(TEXT("::"),&Class,&Instance);
					//
					const FSoftObjectPath ClassPath(Class);
					TSoftObjectPtr<UObject>ClassPtr(ClassPath);
					//
					auto CDO = ClassPtr.LoadSynchronous();
					if (IsInGameThread()&&(CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
						for (TActorIterator<AActor>Actor(Container->GetWorld()); Actor; ++Actor) {
							if ((*Actor)->GetFullName()==Instance) {_REF.Add(*Actor); break;}
						}///
					} else if ((CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
						DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUMapProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty,STATGROUP_TaskGraphTasks);
						FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
							FSimpleDelegateGraphTask::FDelegate::CreateStatic(&RestoreActorReferenceIntoMapKeyInGameThread,Property,Container,Instance),
							GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty),
							nullptr, ENamedThreads::GameThread
						);//
					} else if (IsInGameThread()&&(CDO!=nullptr)) {
						bool IsSet = false;
						for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
							if ((*OBJ)->GetFullName()==Instance) {_REF.Add(*OBJ); IsSet=true; break;}
						} if (!IsSet) {_REF.Add(NewObject<UObject>(Container,CDO->GetClass(),NAME_None,RF_NoFlags,CDO));}
					} else if (CDO!=nullptr) {
						DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUMapProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty,STATGROUP_TaskGraphTasks);
						FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
							FSimpleDelegateGraphTask::FDelegate::CreateStatic(&NewObjectIntoMapKeyInGameThread,Property,Container,CDO,Instance),
							GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty),
							nullptr, ENamedThreads::GameThread
						);//
			}}}/////////
		} else {
			for (int32 I=0, N=JKeys.Num()-1; I<=N; I++) {
				const auto &JKey = JKeys[I];
				auto IKey = Helper.AddDefaultValue_Invalid_NeedsRehash();
				if (JKey.IsValid()&&(!JKey->IsNull())) {
					if (!FJsonObjectConverter::JsonValueToUProperty(JKey,Property->KeyProp,Helper.GetKeyPtr(IKey),0,CPF_Transient)) {
						LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Map Key Type <-> Property Map Key Type] mismatch:  %s"),*Property->GetName()));
		}}}} Helper.Rehash();
	} else {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Map Member <-> Property Map Member] mismatch:  %s"),*Property->GetName()));}
	//
	if (JSON->HasField(Field.Name.ToString()+TEXT("_VALUE"))) {
		auto JValues = JSON->GetArrayField(Field.Name.ToString()+TEXT("_VALUE"));
		if ((Property->KeyProp->IsA(UObjectProperty::StaticClass()))&&(Property->ValueProp->IsA(UObjectProperty::StaticClass()))) {
			TMap<UObject*,UObject*>&_REF = *Property->ContainerPtrToValuePtr<TMap<UObject*,UObject*>>(Container);
			for (int32 I=0, N=JValues.Num()-1; I<=N; I++) {
				const auto &JValue = JValues[I];
				const auto &JKey = JKeys[I];
				if (JValue.IsValid()&&(!JValue->IsNull())) {
					FString Class, Instance, Key;
					JKey->AsString().Split(TEXT("::"),nullptr,&Key);
					JValue->AsString().Split(TEXT("::"),&Class,&Instance);
					//
					const FSoftObjectPath ClassPath(Class);
					TSoftObjectPtr<UObject>ClassPtr(ClassPath);
					//
					TArray<UObject*>Keys;
					UObject* KeyOBJ = nullptr;
					_REF.GenerateKeyArray(Keys);
					//
					for (auto OBJ : Keys) {
						if (OBJ->GetFullName()==Key) {KeyOBJ=OBJ; break;}
					} if (KeyOBJ==nullptr) {continue;}
					//
					auto CDO = ClassPtr.LoadSynchronous();
					if (IsInGameThread()&&(CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
						for (TActorIterator<AActor>Actor(Container->GetWorld()); Actor; ++Actor) {
							if ((*Actor)->GetFullName()==Instance) {_REF.Emplace(KeyOBJ,*Actor); break;}
						}///
					} else if ((CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
						DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUMapProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty,STATGROUP_TaskGraphTasks);
						FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
							FSimpleDelegateGraphTask::FDelegate::CreateStatic(&RestoreActorReferenceIntoMapValueInGameThread,Property,Container,KeyOBJ,Instance),
							GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty),
							nullptr, ENamedThreads::GameThread
						);//
					} else if (IsInGameThread()&&(CDO!=nullptr)) {
						bool IsSet = false;
						for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
							if ((*OBJ)->GetFullName()==Instance) {_REF.Emplace(KeyOBJ,*OBJ); IsSet=true; break;}
						} if (!IsSet) {_REF.Emplace(KeyOBJ,NewObject<UObject>(Container,CDO->GetClass(),NAME_None,RF_NoFlags,CDO));}
					} else if (CDO!=nullptr) {
						DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUMapProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty,STATGROUP_TaskGraphTasks);
						FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
							FSimpleDelegateGraphTask::FDelegate::CreateStatic(&NewObjectIntoMapValueInGameThread,Property,Container,KeyOBJ,CDO,Instance),
							GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty),
							nullptr, ENamedThreads::GameThread
						);//
			}}}/////////
		} else if ((Property->KeyProp->IsA(UNameProperty::StaticClass()))&&(Property->ValueProp->IsA(UObjectProperty::StaticClass()))) {
			TMap<FName,UObject*>&_REF = *Property->ContainerPtrToValuePtr<TMap<FName,UObject*>>(Container);
			for (int32 I=0, N=JValues.Num()-1; I<=N; I++) {
				const auto &JValue = JValues[I];
				const auto &JKey = JKeys[I];
				if (JValue.IsValid()&&(!JValue->IsNull())) {
					FString Class, Instance;
					FName Key = *JKey->AsString();
					JValue->AsString().Split(TEXT("::"),&Class,&Instance);
					//
					const FSoftObjectPath ClassPath(Class);
					TSoftObjectPtr<UObject>ClassPtr(ClassPath);
					//
					auto CDO = ClassPtr.LoadSynchronous();
					if (IsInGameThread()&&(CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
						for (TActorIterator<AActor>Actor(Container->GetWorld()); Actor; ++Actor) {
							if ((*Actor)->GetFullName()==Instance) {_REF.Emplace(Key,*Actor); break;}
						}///
					} else if ((CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
						DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUMapProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty,STATGROUP_TaskGraphTasks);
						FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
							FSimpleDelegateGraphTask::FDelegate::CreateStatic(&RestoreActorReferenceIntoNamedMapValueInGameThread,Property,Container,Key,Instance),
							GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty),
							nullptr, ENamedThreads::GameThread
						);//
					} else if (IsInGameThread()&&(CDO!=nullptr)) {
						bool IsSet = false;
						for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
							if ((*OBJ)->GetFullName()==Instance) {_REF.Emplace(Key,*OBJ); IsSet=true; break;}
						} if (!IsSet) {_REF.Emplace(Key,NewObject<UObject>(Container,CDO->GetClass(),NAME_None,RF_NoFlags,CDO));}
					} else if (CDO!=nullptr) {
						DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUMapProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty,STATGROUP_TaskGraphTasks);
						FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
							FSimpleDelegateGraphTask::FDelegate::CreateStatic(&NewObjectIntoNamedMapValueInGameThread,Property,Container,Key,CDO,Instance),
							GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUMapProperty),
							nullptr, ENamedThreads::GameThread
						);//
			}}}/////////
		} else if (Property->ValueProp->IsA(UObjectProperty::StaticClass())) {
			LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Map Value Type <-> Property Map Value Type] mismatch:  %s"),*Property->GetName()));
			LOG_DB(Logs,ESQLSeverity::Error,TEXT("Only  TMap<UObject*,UObject*>  OR  TMap<FName,UObject*>  are supported for Maps of Object References!"));
		} else {
			for (int32 I=0, N=JValues.Num()-1; I<=N; I++) {
				const auto &JValue = JValues[I];
				if (JValue.IsValid()&&(!JValue->IsNull())) {
					if (!FJsonObjectConverter::JsonValueToUProperty(JValue,Property->ValueProp,Helper.GetValuePtr(I),0,CPF_Transient)) {
						LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Map Value Type <-> Property Map Value Type] mismatch:  %s"),*Property->GetName()));
	}}}}} else {LOG_DB(Logs,ESQLSeverity::Warning,FString::Printf(TEXT("[Data Map Member <-> Property Map Member] mismatch:  %s"),*Property->GetName()));}
}

void USQLReflector::SQLToUProperty(const FSQLField &Field, UStructProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonReader<>>JReader = TJsonReaderFactory<>::Create(Field.Text);
	if (Field.Cast!=ESQLReadMode::Integer) {
		if (!FJsonSerializer::Deserialize(JReader,JSON)||!JSON.IsValid()) {
			LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("Unable to parse JSON Object String for : [%s]"),*Property->GetName()));
	return;}}
	//
	switch (Field.Cast) {
		case ESQLReadMode::Integer:
		{
			auto StructPtr = Property->ContainerPtrToValuePtr<FDateTime>(Container);
			if (StructPtr) {(*StructPtr)=FDateTime::FromUnixTimestamp(Field.Integer);}
		}	break;
		//
		case ESQLReadMode::Color:
		{
			if (Property->Struct==TBaseStructure<FColor>::Get()) {
				auto JColor = JSON->GetObjectField(Field.Name.ToString());
				FColor Color; if (JColor.IsValid()) {
					Color.R = JColor->GetIntegerField(TEXT("r")); Color.G = JColor->GetIntegerField(TEXT("g"));
					Color.B = JColor->GetIntegerField(TEXT("b")); Color.A = JColor->GetIntegerField(TEXT("a"));
				}///
				auto StructPtr = Property->ContainerPtrToValuePtr<FColor>(Container);
				if (StructPtr) {(*StructPtr)=Color;}
			} else if (Property->Struct==TBaseStructure<FLinearColor>::Get()) {
				auto JColor = JSON->GetObjectField(Field.Name.ToString());
				FLinearColor Color; if (JColor.IsValid()) {
					Color.R = JColor->GetNumberField(TEXT("r")); Color.G = JColor->GetNumberField(TEXT("g"));
					Color.B = JColor->GetNumberField(TEXT("b")); Color.A = JColor->GetNumberField(TEXT("a"));
				}///
				auto StructPtr = Property->ContainerPtrToValuePtr<FLinearColor>(Container);
				if (StructPtr) {(*StructPtr)=Color;}
			} else {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Column Type <-> Property Type] mismatch:  %s"),*Property->GetName()));}
		}	break;
		//
		case ESQLReadMode::Vector2D:
		{
			if (Property->Struct!=TBaseStructure<FVector2D>::Get()) {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Column Type <-> Property Type] mismatch:  %s"),*Property->GetName())); break;}
			auto JVector = JSON->GetObjectField(Field.Name.ToString());
			FVector2D Vector; if (JVector.IsValid()) {
				Vector.X = JVector->GetNumberField(TEXT("x"));
				Vector.Y = JVector->GetNumberField(TEXT("y"));
			}///
			auto StructPtr = Property->ContainerPtrToValuePtr<FVector2D>(Container);
			if (StructPtr) {(*StructPtr)=Vector;}
		}	break;
		//
		case ESQLReadMode::Vector3D:
		{
			if (Property->Struct!=TBaseStructure<FVector>::Get()) {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Column Type <-> Property Type] mismatch:  %s"),*Property->GetName())); break;}
			auto JVector = JSON->GetObjectField(Field.Name.ToString());
			FVector Vector; if (JVector.IsValid()) {
				Vector.X = JVector->GetNumberField(TEXT("x"));
				Vector.Y = JVector->GetNumberField(TEXT("y"));
				Vector.Z = JVector->GetNumberField(TEXT("z"));
			}///
			auto StructPtr = Property->ContainerPtrToValuePtr<FVector>(Container);
			if (StructPtr) {(*StructPtr)=Vector;}
		}	break;
		//
		case ESQLReadMode::Rotator:
		{
			if (Property->Struct!=TBaseStructure<FRotator>::Get()) {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Column Type <-> Property Type] mismatch:  %s"),*Property->GetName())); break;}
			auto JRotator = JSON->GetObjectField(Field.Name.ToString());
			FRotator Rotator; if (JRotator.IsValid()) {
				Rotator.Pitch = JRotator->GetNumberField(TEXT("pitch"));
				Rotator.Roll = JRotator->GetNumberField(TEXT("roll"));
				Rotator.Yaw = JRotator->GetNumberField(TEXT("yaw"));
			}///
			auto StructPtr = Property->ContainerPtrToValuePtr<FRotator>(Container);
			if (StructPtr) {(*StructPtr)=Rotator;}
		}	break;
		//
		case ESQLReadMode::FloatCurve:
		{
			if (Property->Struct!=FRuntimeFloatCurve::StaticStruct()) {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Column Type <-> Property Type] mismatch:  %s"),*Property->GetName())); break;}
			FRuntimeFloatCurve FloatCurve; FRichCurve CurveData;
			FJsonObjectConverter::JsonObjectToUStruct<FRichCurve>(JSON.ToSharedRef(),&CurveData);
			auto StructPtr = Property->ContainerPtrToValuePtr<FRuntimeFloatCurve>(Container);
			if (StructPtr) {FloatCurve.ExternalCurve = nullptr; FloatCurve.EditorCurveData = CurveData; (*StructPtr)=FloatCurve;}
		}	break;
		//
		case ESQLReadMode::Struct:
		{
			void* StructPtr = Property->ContainerPtrToValuePtr<void>(Container);
			if (StructPtr) {ParseJSONtoStruct(JSON,Property,StructPtr,Container);}
			else {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Column Type <-> Property Type] mismatch:  %s"),*Property->GetName()));}
		}	break;
	default: break;}
}

void USQLReflector::SQLToUProperty(FSQLField &Field, UObjectProperty* Property, UObject* Container) {
	check(Property); check(Container);
	//
	switch (Field.Cast) {
		case ESQLReadMode::ObjectPtr:
		{
			auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Container);
			//
			FString Class, Instance;
			Field.Text.Split(TEXT("::"),&Class,&Instance);
			//
			const FSoftObjectPath ClassPath(Class);
			TSoftObjectPtr<UObject>ClassPtr(ClassPath);
			//
			auto CDO = ClassPtr.LoadSynchronous();
			if (IsInGameThread()&&(CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
				for (TActorIterator<AActor>Actor(Container->GetWorld()); Actor; ++Actor) {
					if ((*Actor)->GetFullName()==Instance) {Property->SetPropertyValue(ValuePtr,(*Actor)); break;}
				}///
			} else if ((CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
				DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUProperty,STATGROUP_TaskGraphTasks);
				FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
					FSimpleDelegateGraphTask::FDelegate::CreateStatic(&RestoreActorReferenceInGameThread,Property,Container,Instance),
					GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUProperty),
					nullptr, ENamedThreads::GameThread
				);//
			} else if (IsInGameThread()&&(CDO!=nullptr)) {
				bool IsSet = false;
				for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
					if ((*OBJ)->GetFullName()==Instance) {Property->SetPropertyValue(ValuePtr,(*OBJ)); IsSet=true; break;}
				} if (!IsSet) {Property->SetPropertyValue(ValuePtr,NewObject<UObject>(Container,CDO->GetClass(),NAME_None,RF_NoFlags,CDO));}
			} else if (CDO!=nullptr) {
				DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUProperty,STATGROUP_TaskGraphTasks);
				FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
					FSimpleDelegateGraphTask::FDelegate::CreateStatic(&NewObjectReferenceInGameThread,Property,Container,CDO,Instance),
					GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUProperty),
					nullptr, ENamedThreads::GameThread
				);//
			}///
		}	break;
	default: break;}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Struct Parser:

TSharedPtr<FJsonObject> USQLReflector::ParseStructToJSON(FString &Output, UStructProperty* Property, const void* StructPtr) {
	TSharedRef<TJsonWriter<>>Buffer = TJsonWriterFactory<>::Create(&Output);
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	check(Property); check(StructPtr);
	//
	UScriptStruct* Struct = Property->Struct;
	for (TFieldIterator<UProperty>IT(Struct); IT; ++IT) {
		UProperty* Field = *IT; 
		for (int32 I=0; I<Field->ArrayDim; ++I) {
			auto ValuePtr = Field->ContainerPtrToValuePtr<void>(StructPtr,I);
			if (!ValuePtr) {continue;} ParsePropertyToJSON(JSON,Field,ValuePtr,0,CPF_Transient);
		}///
	}///
	//
	FJsonSerializer::Serialize(JSON.ToSharedRef(),Buffer); return JSON;
}

void USQLReflector::ParseStructToJSON(TSharedPtr<FJsonObject> &JSON, UStructProperty* Property, const void* StructPtr) {
	UScriptStruct* Struct = Property->Struct;
	check(Property); check(StructPtr);
	//
	for (TFieldIterator<UProperty>IT(Struct); IT; ++IT) {
		UProperty* Field = *IT; 
		for (int32 I=0; I<Field->ArrayDim; ++I) {
			auto ValuePtr = Field->ContainerPtrToValuePtr<void>(StructPtr,I);
			if (!ValuePtr) {continue;} ParsePropertyToJSON(JSON,Field,ValuePtr,0,CPF_Transient);
		}///
	}///
}

TSharedPtr<FJsonObject> USQLReflector::ParseStructToJSON(UStructProperty* Property, const void* StructPtr) {
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	UScriptStruct* Struct = Property->Struct;
	check(Property); check(StructPtr);
	//
	for (TFieldIterator<UProperty>IT(Struct); IT; ++IT) {
		UProperty* Field = *IT; 
		for (int32 I=0; I<Field->ArrayDim; ++I) {
			auto ValuePtr = Field->ContainerPtrToValuePtr<void>(StructPtr,I);
			if (!ValuePtr) {continue;} ParsePropertyToJSON(JSON,Field,ValuePtr,0,CPF_Transient);
		}///
	}///
	//
	return JSON;
}

void USQLReflector::ParsePropertyToJSON(TSharedPtr<FJsonObject> &JSON, UProperty* Property, const void* ValuePtr, int64 CheckFlags, int64 SkipFlags) {
	check(Property); check(ValuePtr);
	TSharedPtr<FJsonValue>JValue;
	//
	if (UBoolProperty* _Bool = Cast<UBoolProperty>(Property)) {
		JValue = FJsonObjectConverter::UPropertyToJsonValue(_Bool,ValuePtr,CheckFlags,SkipFlags);
		if (JValue.IsValid()&&(!JValue->IsNull())) {JSON->SetField(_Bool->GetName(),JValue);}
	}///
	//
	if (UEnumProperty* _Enum = Cast<UEnumProperty>(Property)) {
		UEnum* EnumPtr = _Enum->GetEnum();
		FString Value = EnumPtr->GetNameStringByValue(_Enum->GetUnderlyingProperty()->GetUnsignedIntPropertyValue(ValuePtr));
		JSON->SetStringField(_Enum->GetName(),Value);
	}///
	//
	if (UNumericProperty* _Numeric = Cast<UNumericProperty>(Property)) {
		JValue = FJsonObjectConverter::UPropertyToJsonValue(_Numeric,ValuePtr,CheckFlags,SkipFlags);
		if (JValue.IsValid()&&(!JValue->IsNull())) {JSON->SetField(_Numeric->GetName(),JValue);}
	}///
	//
	if (UNameProperty* _Name = Cast<UNameProperty>(Property)) {
		JValue = FJsonObjectConverter::UPropertyToJsonValue(_Name,ValuePtr,CheckFlags,SkipFlags);
		if (JValue.IsValid()&&(!JValue->IsNull())) {JSON->SetField(_Name->GetName(),JValue);}
	}///
	//
	if (UStrProperty* _String = Cast<UStrProperty>(Property)) {
		JValue = FJsonObjectConverter::UPropertyToJsonValue(_String,ValuePtr,CheckFlags,SkipFlags);
		if (JValue.IsValid()&&(!JValue->IsNull())) {JSON->SetField(_String->GetName(),JValue);}
	}///
	//
	if (UTextProperty* _Text = Cast<UTextProperty>(Property)) {
		JValue = FJsonObjectConverter::UPropertyToJsonValue(_Text,ValuePtr,CheckFlags,SkipFlags);
		if (JValue.IsValid()&&(!JValue->IsNull())) {JSON->SetField(_Text->GetName(),JValue);}
	}///
	//
	if (UArrayProperty* _Array = Cast<UArrayProperty>(Property)) {
		TArray<TSharedPtr<FJsonValue>>Out;
		FScriptArrayHelper Helper(_Array,ValuePtr);
		if (_Array->Inner->IsA(UObjectProperty::StaticClass())) {
			LOG_DB(Logs,ESQLSeverity::Error,TEXT("Array of Object References, when member of a Struct Property, is not supported!"));
		} else {
			for (int32 I=0, N=Helper.Num()-1; I<=N; I++) {
				auto Value = FJsonObjectConverter::UPropertyToJsonValue(_Array->Inner,Helper.GetRawPtr(I),CheckFlags,SkipFlags);
				if (Value.IsValid()&&(!Value->IsNull())) {Out.Push(Value);}
	}} if (Out.Num()>0) {JSON->SetArrayField(_Array->GetName(),Out);}}
	//
	if (USetProperty* _Set = Cast<USetProperty>(Property)) {
		TArray<TSharedPtr<FJsonValue>>Out;
		FScriptSetHelper Helper(_Set,ValuePtr);
		if (_Set->ElementProp->IsA(UObjectProperty::StaticClass())) {
			LOG_DB(Logs,ESQLSeverity::Error,TEXT("Set of Object References, when member of a Struct Property, is not supported!"));
		} else {
			for (int32 I=0, N=Helper.Num()-1; I<=N; I++) {
				auto Value = FJsonObjectConverter::UPropertyToJsonValue(_Set->ElementProp,Helper.GetElementPtr(I),CheckFlags,SkipFlags);
				if (Value.IsValid()&&(!Value->IsNull())) {Out.Push(Value);}
	}} if (Out.Num()>0) {JSON->SetArrayField(_Set->GetName(),Out);}}
	//
	if (UMapProperty* _Map = Cast<UMapProperty>(Property)) {
		TArray<TSharedPtr<FJsonValue>>Keys;
		TArray<TSharedPtr<FJsonValue>>Values;
		FScriptMapHelper Helper(_Map,ValuePtr);
		//
		if (_Map->KeyProp->IsA(UObjectProperty::StaticClass())) {
			LOG_DB(Logs,ESQLSeverity::Error,TEXT("Map Keys of Object References, when member of a Struct Property, are not supported!"));
		} else {
			for (int32 I=0, N=Helper.Num()-1; I<=N; I++) {
				auto Key = FJsonObjectConverter::UPropertyToJsonValue(_Map->KeyProp,Helper.GetKeyPtr(I),CheckFlags,SkipFlags);
				if (Key.IsValid()&&(!Key->IsNull())) {Keys.Push(Key);}
			}///
		}///
		if (_Map->ValueProp->IsA(UObjectProperty::StaticClass())) {
			LOG_DB(Logs,ESQLSeverity::Error,TEXT("Map Values of Object References, when member of a Struct Property, are not supported!"));
		} else {
			for (int32 I=0, N=Helper.Num()-1; I<=N; I++) {
				auto Value = FJsonObjectConverter::UPropertyToJsonValue(_Map->ValueProp,Helper.GetValuePtr(I),CheckFlags,SkipFlags);
				if (Value.IsValid()&&(!Value->IsNull())) {Values.Push(Value);}
			}///
		}///
		if ((Keys.Num()>0)&&(Values.Num()>0)) {
			JSON->SetArrayField(_Map->GetName()+TEXT("_KEY"),Keys);
			JSON->SetArrayField(_Map->GetName()+TEXT("_VALUE"),Values);
		}///
	}///
	//
	if (UStructProperty* _Struct = Cast<UStructProperty>(Property)) {
		JSON->SetObjectField(_Struct->GetName(),ParseStructToJSON(_Struct,ValuePtr));
	}///
	//
	if (UObjectProperty* _Object = Cast<UObjectProperty>(Property)) {
		if (_Object->GetPropertyValue(ValuePtr)->IsValidLowLevelFast()) {
			auto ObjectPath = _Object->GetPropertyValue(ValuePtr)->GetFullName();
			auto ClassPath = _Object->GetPropertyValue(ValuePtr)->GetClass()->GetDefaultObject()->GetPathName();
			JSON->SetStringField(_Object->GetName(),ClassPath+TEXT("::")+ObjectPath);
		}///
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Struct Member Parser:

void USQLReflector::ParseJSONtoStruct(TSharedPtr<FJsonObject> &JSON, UStructProperty* Property, void* StructPtr, UObject* Container) {
	UScriptStruct* Struct = Property->Struct;
	check(Property); check(StructPtr);
	//
	for (TFieldIterator<UProperty>IT(Struct); IT; ++IT) {
		UProperty* Field = *IT; 
		for (int32 I=0; I<Field->ArrayDim; ++I) {
			void* ValuePtr = Field->ContainerPtrToValuePtr<void>(StructPtr,I);
			if (ValuePtr) {ParseJSONtoProperty(JSON,Field,ValuePtr,Container,0,CPF_Transient);}
		}///
	}///
}

void USQLReflector::ParseJSONtoProperty(TSharedPtr<FJsonObject> &JSON, UProperty* Property, void* ValuePtr, UObject* Container, int64 CheckFlags, int64 SkipFlags) {
	check(Property); check(ValuePtr);
	//
	if (UBoolProperty* _Bool = Cast<UBoolProperty>(Property)) {
		const auto Field = _Bool->GetName();
		if (JSON->HasField(Field)) {
			_Bool->SetPropertyValue(ValuePtr,JSON->GetBoolField(Field));
		}///
	}///
	//
	if (UEnumProperty* _Enum = Cast<UEnumProperty>(Property)) {
		UEnum* EnumPtr = _Enum->GetEnum();
		const auto Field = _Enum->GetName();
		if ((EnumPtr)&&JSON->HasField(Field)) {
			auto Value = EnumPtr->GetValueByName(*JSON->GetStringField(Field));
			_Enum->GetUnderlyingProperty()->SetIntPropertyValue(ValuePtr,Value);
		}///
	}///
	//
	if (UByteProperty* _Byte = Cast<UByteProperty>(Property)) {
		const auto Field = _Byte->GetName();
		if (JSON->HasField(Field)) {
			_Byte->SetIntPropertyValue(ValuePtr,(uint64)JSON->GetIntegerField(Field));
		}///
	}///
	//
	if (UIntProperty* _Int = Cast<UIntProperty>(Property)) {
		const auto Field = _Int->GetName();
		if (JSON->HasField(Field)) {
			_Int->SetIntPropertyValue(ValuePtr,(int64)JSON->GetIntegerField(Field));
		}///
	}///
	//
	if (UFloatProperty* _Float = Cast<UFloatProperty>(Property)) {
		const auto Field = _Float->GetName();
		if (JSON->HasField(Field)) {
			_Float->SetFloatingPointPropertyValue(ValuePtr,JSON->GetNumberField(Field));
		}///
	}///
	//
	if (UNameProperty* _Name = Cast<UNameProperty>(Property)) {
		const auto Field = _Name->GetName();
		if (JSON->HasField(Field)) {
			_Name->SetPropertyValue(ValuePtr,*JSON->GetStringField(Field));
		}///
	}///
	//
	if (UStrProperty* _String = Cast<UStrProperty>(Property)) {
		const auto Field = _String->GetName();
		if (JSON->HasField(Field)) {
			_String->SetPropertyValue(ValuePtr,JSON->GetStringField(Field));
		}///
	}///
	//
	if (UTextProperty* _Text = Cast<UTextProperty>(Property)) {
		const auto Field = _Text->GetName();
		if (JSON->HasField(Field)) {
			_Text->SetPropertyValue(ValuePtr,FText::FromString(JSON->GetStringField(Field)));
		}///
	}///
	//
	if (UArrayProperty* _Array = Cast<UArrayProperty>(Property)) {
		const auto Field = _Array->GetName();
		if (JSON->HasField(Field)) {
			if (_Array->Inner->IsA(UObjectProperty::StaticClass())) {
				LOG_DB(Logs,ESQLSeverity::Error,TEXT("Array of Object References, when member of a Struct Property, is not supported!"));
			} else {
				auto JArray = JSON->GetArrayField(Field);
				FScriptArrayHelper Helper(_Array,ValuePtr);
				Helper.Resize(JArray.Num());
				for (int32 I=0, N=JArray.Num()-1; I<=N; I++) {
					const auto &JValue = JArray[I];
					if (JValue.IsValid()&&(!JValue->IsNull())) {
						if (!FJsonObjectConverter::JsonValueToUProperty(JValue,_Array->Inner,Helper.GetRawPtr(I),0,SkipFlags)) {
							LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Array Element Type <-> Property Array Element Type] mismatch:  %s"),*Property->GetName()));
		}}}}}///////////////
	}///
	//
	if (USetProperty* _Set = Cast<USetProperty>(Property)) {
		const auto Field = _Set->GetName();
		if (JSON->HasField(Field)) {
			if (_Set->ElementProp->IsA(UObjectProperty::StaticClass())) {
				LOG_DB(Logs,ESQLSeverity::Error,TEXT("Set of Object References, when member of a Struct Property, is not supported!"));
			} else {
				auto JSet = JSON->GetArrayField(Field);
				FScriptSetHelper Helper(_Set,ValuePtr);
				Helper.EmptyElements();
				for (int32 I=0, N=JSet.Num()-1; I<=N; I++) {
					const auto &JValue = JSet[I];
					auto Index = Helper.AddDefaultValue_Invalid_NeedsRehash();
					if (JValue.IsValid()&&(!JValue->IsNull())) {
						if (!FJsonObjectConverter::JsonValueToUProperty(JValue,_Set->ElementProp,Helper.GetElementPtr(Index),0,SkipFlags)) {
							LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Set Element Type <-> Property Set Element Type] mismatch:  %s"),*Property->GetName()));
					}}//////
	} Helper.Rehash();}}}
	//
	if (UMapProperty* _Map = Cast<UMapProperty>(Property)) {
		int32 Num=0;
		const auto Field = _Map->GetName();
		if (JSON->HasField(Field+TEXT("_KEY"))) {
			if (_Map->KeyProp->IsA(UObjectProperty::StaticClass())) {
				LOG_DB(Logs,ESQLSeverity::Error,TEXT("Map Keys of Object References, when member of a Struct Property, are not supported!"));
			} else {
				auto JKeys = JSON->GetArrayField(Field+TEXT("_KEY"));
				FScriptMapHelper Helper(_Map,ValuePtr);
				Helper.EmptyValues(); Num = JKeys.Num();
				for (int32 I=0, N=JKeys.Num()-1; I<=N; I++) {
					const auto &JKey = JKeys[I];
					auto IKey = Helper.AddDefaultValue_Invalid_NeedsRehash();
					if (JKey.IsValid()&&(!JKey->IsNull())) {
						if (!FJsonObjectConverter::JsonValueToUProperty(JKey,_Map->KeyProp,Helper.GetKeyPtr(IKey),0,SkipFlags)) {
							LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Map Key Type <-> Property Map Key Type] mismatch:  %s"),*Property->GetName()));
		}}} Helper.Rehash();}}
		//
		if (JSON->HasField(Field+TEXT("_VALUE"))) {
			if (_Map->ValueProp->IsA(UObjectProperty::StaticClass())) {
				LOG_DB(Logs,ESQLSeverity::Error,TEXT("Map Values of Object References, when member of a Struct Property, are not supported!"));
			} else {
				auto JValues = JSON->GetArrayField(Field+TEXT("_VALUE"));
				FScriptMapHelper Helper(_Map,ValuePtr);
				check(JValues.Num()==Num);
				for (int32 I=0, N=JValues.Num()-1; I<=N; I++) {
					const auto &JValue = JValues[I];
					if (JValue.IsValid()&&(!JValue->IsNull())) {
						if (!FJsonObjectConverter::JsonValueToUProperty(JValue,_Map->ValueProp,Helper.GetValuePtr(I),0,SkipFlags)) {
							LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Map Value Type <-> Property Map Value Type] mismatch:  %s"),*Property->GetName()));
		}}}}}///////////////
	}///
	//
	if (UStructProperty* _Struct = Cast<UStructProperty>(Property)) {
		const auto Field = _Struct->GetName();
		if (JSON->HasField(Field)) {
			auto JStruct = JSON->GetObjectField(Field);
			if (JStruct.IsValid()) {ParseJSONtoStruct(JStruct,_Struct,ValuePtr,Container);}
		}///
	}///
	//
	if (UObjectProperty* _Object = Cast<UObjectProperty>(Property)) {
		const auto Field = _Object->GetName();
		if (JSON->HasField(Field)) {
			FString Class, Instance;
			JSON->GetStringField(Field).Split(TEXT("::"),&Class,&Instance);
			//
			const FSoftObjectPath ClassPath(Class);
			TSoftObjectPtr<UObject>ClassPtr(ClassPath);
			//
			auto CDO = ClassPtr.LoadSynchronous();
			if (IsInGameThread()&&(CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
				for (TActorIterator<AActor>Actor(Container->GetWorld()); Actor; ++Actor) {
					if ((*Actor)->GetFullName()==Instance) {_Object->SetPropertyValue(ValuePtr,(*Actor)); break;}
				}///
			} else if ((CDO!=nullptr)&&CDO->IsA(AActor::StaticClass())) {
				DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUProperty,STATGROUP_TaskGraphTasks);
				FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
					FSimpleDelegateGraphTask::FDelegate::CreateStatic(&RestoreActorReferenceInGameThread,_Object,Container,Instance),
					GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUProperty),
					nullptr, ENamedThreads::GameThread
				);//
			} else if (IsInGameThread()&&(CDO!=nullptr)) {
				bool IsSet = false;
				for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
					if ((*OBJ)->GetFullName()==Instance) {_Object->SetPropertyValue(ValuePtr,(*OBJ)); IsSet=true; break;}
				} if (!IsSet) {_Object->SetPropertyValue(ValuePtr,NewObject<UObject>(Container,CDO->GetClass(),NAME_None,RF_NoFlags,CDO));}
			} else if (CDO!=nullptr) {
				DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SQLObjectToUProperty"),STAT_FSimpleDelegateGraphTask_SQLObjectToUProperty,STATGROUP_TaskGraphTasks);
				FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
					FSimpleDelegateGraphTask::FDelegate::CreateStatic(&NewObjectReferenceInGameThread,_Object,Container,CDO,Instance),
					GET_STATID(STAT_FSimpleDelegateGraphTask_SQLObjectToUProperty),
					nullptr, ENamedThreads::GameThread
				);//
			}///
		} else {LOG_DB(Logs,ESQLSeverity::Error,FString::Printf(TEXT("[Data Object Struct Member <-> Property Object Struct Member] mismatch:  %s"),*Property->GetName()));}
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UObject* USQLReflector::NewObjectInstance(UObject* Context, UClass* Class) {
	if (!Context||!Context->IsValidLowLevelFast()||Context->IsPendingKill()) {return nullptr;}
	if (Class==nullptr) {return nullptr;}
	//
	auto World = GEngine->GetWorldFromContextObject(Context,EGetWorldErrorMode::LogAndReturnNull);
	if (World!=nullptr) {return NewObject<UObject>(Context,Class);}
	//
	return nullptr;
}

UObject* USQLReflector::NewONamedbjectInstance(UObject* Context, UClass* Class, FName Name) {
	if (!Context||!Context->IsValidLowLevelFast()||Context->IsPendingKill()) {return nullptr;}
	if (Class==nullptr) {return nullptr;}
	//
	auto World = GEngine->GetWorldFromContextObject(Context,EGetWorldErrorMode::LogAndReturnNull);
	if (World!=nullptr) {return NewObject<UObject>(Context,Class,Name);}
	//
	return nullptr;
}

UObject* USQLReflector::GetClassDefaultObject(UClass* Class) {
	return Class->GetDefaultObject(true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////