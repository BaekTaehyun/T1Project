//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "USQL.h"
#include "USQLite_Shared.h"
#include "ISQLiteData.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UINTERFACE(Category="USQL", BlueprintType, meta = (DisplayName="SQL Interface : Database"))
class USQLITE_API USQLiteData : public UInterface {
	GENERATED_BODY()
};

class USQLITE_API ISQLiteData {
	GENERATED_BODY()
public:
	/// DB: Prepare Save Interface.
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta = (DisplayName="[DATABASE] On Prepare To Save:", Keywords = "DB Save Prepare"))
	void DB_PrepareToSave(USQLite* Database, const ESQLSaveMode Mode);
	//
	/// DB: Prepare Load Interface.
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta = (DisplayName="[DATABASE] On Prepare To Load:", Keywords = "DB Load Prepare"))
	void DB_PrepareToLoad(USQLite* Database);
	//
	//
	/// DB: Begin Save Listener.
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta = (DisplayName="[DATABASE] On Save Data Begin:", Keywords = "DB Save Begin"))
	void DB_OnBeginSave(USQLite* Database);
	//
	/// DB: Progress Save Listener.
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta = (DisplayName="[DATABASE] On Save Data Progress:", Keywords = "DB Save Progress"))
	void DB_OnProgressSave(USQLite* Database, const float Progress);
	//
	/// DB: Finish Save Listener.
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta = (DisplayName="[DATABASE] On Save Data Finish:", Keywords = "DB Save Finish"))
	void DB_OnFinishSave(USQLite* Database, const bool Success);
	//
	//
	/// DB: Begin Load Listener.
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta = (DisplayName="[DATABASE] On Load Data Begin:", Keywords = "DB Load Begin"))
	void DB_OnBeginLoad(USQLite* Database);
	//
	/// DB: Progress Load Listener.
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta = (DisplayName="[DATABASE] On Load Data Progress:", Keywords = "DB Load Progress"))
	void DB_OnProgressLoad(USQLite* Database, const float Progress, FSQLRow Data);
	//
	/// DB: Finish Load Listener.
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta = (DisplayName="[DATABASE] On Load Data Finish:", Keywords = "DB Load Finish"))
	void DB_OnFinishLoad(USQLite* Database, const bool Success);
	//
	//
	/// DB: Progress Save Threaded Listener.
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta = (DisplayName="[DATABASE] On Save Data Progress [Threaded]:", Keywords = "DB Save Progress"))
	void DB_OnProgressSave__Threaded(USQLite* Database, const float Progress);
	//
	/// DB: Finish Save Threaded Listener.
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta = (DisplayName="[DATABASE] On Save Data Finish [Threaded]:", Keywords = "DB Save Finish"))
	void DB_OnFinishSave__Threaded(USQLite* Database, const bool Success);
	//
	/// DB: Progress Load Threaded Listener.
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta = (DisplayName="[DATABASE] On Load Data Progress [Threaded]:", Keywords = "DB Load Progress"))
	void DB_OnProgressLoad__Threaded(USQLite* Database, FSQLRow Data, const float Progress);
	//
	/// DB: Finish Load Threaded Listener.
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta = (DisplayName="[DATABASE] On Load Data Finish [Threaded]:", Keywords = "DB Load Finish"))
	void DB_OnFinishLoad__Threaded(USQLite* Database, const bool Success);
	//
	//
	/// DB: Global Save Progress Report Listener.
	UFUNCTION(Category="USQL", BlueprintImplementableEvent, meta = (DisplayName="[DATABASE] On Global Save Progress [Threaded]:", Keywords = "DB Save Progress"))
	void DB_OnGlobalSaveProgressReport__Threaded(const float Progress);
	//
	/// DB: Global Load Progress Report Listener.
	UFUNCTION(Category="USQL", BlueprintImplementableEvent, meta = (DisplayName="[DATABASE] On Global Load Progress [Threaded]:", Keywords = "DB Load Progress"))
	void DB_OnGlobalLoadProgressReport__Threaded(const float Progress);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////