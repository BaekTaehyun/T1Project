//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "USQL.h"
#include "ISQLiteDriver.h"
#include "LoadScreen/USQLoadScreenHUD.h"

#include "Runtime/Engine/Classes/Curves/CurveFloat.h"
#include "Runtime/SlateCore/Public/Fonts/SlateFontInfo.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Core/Public/Async/TaskGraphInterfaces.h"
#include "Runtime/Engine/Classes/Curves/CurveLinearColor.h"
#include "USQLite_Shared.h"

#include "USQLite.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DECLARE_DYNAMIC_DELEGATE_OneParam(FSQLite_ThreadChanged,ESQLThreadSafety,ThreadState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSQLite_BeginDataSAVE,USQLite*,Database);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSQLite_FinishDataSAVE,USQLite*,Database,const bool,Success);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSQLite_ProgressDataSAVE,USQLite*,Database,const float,Progress);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSQLite_BeginDataLOAD,USQLite*,Database);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSQLite_FinishDataLOAD,USQLite*,Database,const bool,Success);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSQLite_ProgressDataLOAD,USQLite*,Database,const float,Progress,FSQLRow,Data);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UENUM(BlueprintType,meta=(DisplayName="SQL Load-Screen Mode",RestrictedToClasses=SQLite,BlueprintInternalUseOnly=true))
enum class ESQLoadScreenMode : uint8 {
	BackgroundBlur,
	SplashScreen,
	MoviePlayer,
	NoLoadScreen
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// USQLite Default Settings:

UCLASS(classGroup=Synaptech, Category="Synaptech", config = USQL, meta=(DisplayName="USQLite Settings"))
class USQLITE_API USQLite_Settings : public UObject {
	GENERATED_BODY()
	//
	USQLite_Settings();
public:
	/** If enabled, SQLite operations on Fields and Values will be logged. */
	UPROPERTY(Category="USQL", EditDefaultsOnly, config, meta=(DisplayName="[DB] Deep Logs"))
	bool DeepLogs;
	//
	//
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent &PECP) override;
#endif
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// USQLite Main Class:

UCLASS(classGroup=Synaptech, Category="Synaptech", BlueprintType, Blueprintable, hideCategories = ("Events"))
class USQLITE_API USQLite : public UObject, public ISQLiteDriver {
	GENERATED_BODY()
	//
	USQLite();
	friend class USQL;
	friend class DBS_ExecuteQueue_Task;
	friend class DBL_ExecuteQueue_Task;
#if WITH_EDITORONLY_DATA
	friend class FUSQLDetails;
	friend class SQLPreviewWidget;
#endif
private:
	UWorld* World;
	DBSQL DB_RUNTIME;
protected:
	UPROPERTY(Category="_TABLE", EditDefaultsOnly, meta=(DisplayName="[DATABASE] Table"))
	FSQLTable DB_TABLE;
	//
	//
	/** Dictates which thread SQLite Commands are executed within.
	If you're saving and loading many Object Properties, you should consider run the Database in Threaded Mode.
	If you need absolute control of when saving and loading starts and finishes, run the Database in Synchronous Mode (locks Game Thread when loading). */
	UPROPERTY(Category="USQL", EditDefaultsOnly, BlueprintReadOnly, meta=(DisplayName="[DATABASE] Runtime Mode"))
	ESQLRuntimeMode DB_MODE;
	//
	/** DB File's Absolute Path. */
	UPROPERTY(Category="USQL", EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay, meta=(DisplayName="[DATABASE] File Path"))
	FString DB_FILE;
	//
	/** SQL Command Condition Statement: UPDATE. */
	UPROPERTY(Category="USQL", EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay, meta=(DisplayName="[DATABASE] UPDATE Condition"))
	FString DBS_UpdateCondition;
	//
	/** SQL Command Condition Statement: SELECT. */
	UPROPERTY(Category="USQL", EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay, meta=(DisplayName="[DATABASE] SELECT Condition"))
	FString DBL_SelectCondition;
	//
	/** Enables Debug Logs. */
	UPROPERTY(Category="USQL", EditDefaultsOnly, AdvancedDisplay)
	bool Debug;
	//
	/** SQL Command Queue : SAVE (Raw SQL Command Set of Strings). */
	UPROPERTY(Category="USQL", BlueprintReadOnly, meta=(DisplayName="[DATABASE] SQL Command QUEUE : SAVE"))
	TSet<FString> DBS_QUEUE;
	//
	/** SQL Command Queue : LOAD (Raw SQL Command Set of Strings). */
	UPROPERTY(Category="USQL", BlueprintReadOnly, meta=(DisplayName="[DATABASE] SQL Command QUEUE : LOAD"))
	TSet<FString> DBL_QUEUE;
	//
	//
	/** SQL Table Versioning. Used as Table Name's Postfix for Identity. */
	UPROPERTY(Category="VERSIONING", EditDefaultsOnly, meta=(DisplayName="[DATABASE] Target Active Version"))
	FSQLVersion DB_VERSION;
	//
	/** SQL Table Versioning. List of Table Versions; Each Version equates to a different Table generated in the Database. */
	UPROPERTY(Category="VERSIONING", EditDefaultsOnly, meta=(DisplayName="[DATABASE] TABLE Versioning"))
	TSet<FString> DB_VERSIONS;
	//
	/** SQL Property Versioning. Used as 'Property Name Redirector' for Identity;
	If a Property's name have changed in a Class, Property cannot be loaded from a Database Record using old ID;
	Using a Property Redirector of 'Old Name' mapped to 'New Name' provides a way to load records from outdated Databases.
	Only Name Redirectors are supported, type mismatching will generate errors when loading a Property from the Database. */
	UPROPERTY(Category="VERSIONING", EditDefaultsOnly, meta=(DisplayName="[DATABASE] CLASS Versioning"))
	TMap<TSubclassOf<UObject>,FSQLRedirect> DB_REDIRECTORS;
	//
	//
	/** Automatically setup a Load-Screen of selected type when loading. */
	UPROPERTY(Category="LOAD-SCREEN", EditDefaultsOnly, BlueprintReadWrite, meta=(DisplayName="[LOAD-SCREEN] Screen Mode"))
	ESQLoadScreenMode LoadScreenMode;
	//
	/** Text to display with Progress Bar when 'Saving'. */
	UPROPERTY(Category="LOAD-SCREEN", EditDefaultsOnly, BlueprintReadWrite, meta=(DisplayName="[LOAD-SCREEN] Feedback Text: SAVE"))
	FText FeedbackSAVE;
	//
	/** Text to display with Progress Bar when 'Loading'. */
	UPROPERTY(Category="LOAD-SCREEN", EditDefaultsOnly, BlueprintReadWrite, meta=(DisplayName="[LOAD-SCREEN] Feedback Text: LOAD"))
	FText FeedbackLOAD;
	//
	/** If Load-Screen Mode is a 'Background Blur', dictates how strong the blur frame will be while loading. */
	UPROPERTY(Category="LOAD-SCREEN", EditDefaultsOnly, BlueprintReadWrite, meta=(DisplayName="[LOAD-SCREEN] Background-Blur Power", ClampMin="1.0", UIMin="1.0", ClampMax="100.0", UIMax="100.0"))
	float BackBlurPower;
	//
	/** Image used to display as background when 'Loading' or 'Saving' if Mode is 'Splash Screen'. */
	UPROPERTY(Category="LOAD-SCREEN", EditDefaultsOnly, BlueprintReadWrite, meta=(DisplayName="[LOAD-SCREEN] Splash-Screen", AllowedClasses="Texture2D"))
	FStringAssetReference SplashImage;
	//
	/** Image used to display as background when 'Loading' or 'Saving' if Mode is 'Splash Screen'. */
	UPROPERTY(Category="LOAD-SCREEN", EditDefaultsOnly, BlueprintReadWrite, meta=(DisplayName="[LOAD-SCREEN] Splash-Movie", AllowedClasses="FileMediaSource"))
	FStringAssetReference SplashMovie;
	//
	/** If enabled, when Mode is 'Movie Player', Progress Bar will still be displayed on top of video. */
	UPROPERTY(Category="LOAD-SCREEN", EditDefaultsOnly, BlueprintReadWrite, meta=(DisplayName="[LOAD-SCREEN] Progress-Bar on Movies"))
	bool ProgressBarOnMovie;
	//
	/** If enabled, Threaded Save and Load calls will pause the game while loading. */
	UPROPERTY(Category="LOAD-SCREEN", EditDefaultsOnly, BlueprintReadWrite, meta=(DisplayName="[LOAD-SCREEN] Pause Game"))
	bool PauseGameOnLoad;
	//
	/** Color tint applied on top of Progress Bar's filler image. */
	UPROPERTY(Category="LOAD-SCREEN", EditDefaultsOnly, BlueprintReadWrite, AdvancedDisplay, meta=(DisplayName="[LOAD-SCREEN] Progress-Bar Tint"))
	FLinearColor ProgressBarTint;
	//
	/* Stretching mode of Splash Image, if applicable. */
	UPROPERTY(Category="LOAD-SCREEN", EditDefaultsOnly, BlueprintReadWrite, AdvancedDisplay, meta=(DisplayName="[LOAD-SCREEN] Splash Stretch-Mode"))
	TEnumAsByte<EStretch::Type> SplashStretch;
	//
	/** Font used to display 'Loading' or 'Saving' feedback text. */
	UPROPERTY(Category="LOAD-SCREEN", EditDefaultsOnly, BlueprintReadWrite, AdvancedDisplay, meta=(DisplayName="[LOAD-SCREEN] Feedback Font"))
	FSlateFontInfo FeedbackFont;
	//
	//
	/** Re-Asserts Default Property Values.
	Do NOT call this before this Database Object's Properties finish being initialized! */
	virtual void DB_Setup();
	//
	/** Gets Object Name, stripping out illegal characters. */
	virtual FString GetNameSanitized() const;
	//
	/** Generates a SQL Execution String from this Database's Table Fields that can insert a SQL Table into DB File. */
	virtual FString DB_CreateTableSQL();
	//
	/** Generates a SQL Execution String from this Database's Table Fields that can insert a SQL Table into DB File. */
	virtual FString DB_CreateTableSQL(const FString Header);
	//
	/** Returns Column ID Name mapped to a Class Version Redirector, if any. */
	virtual FString DB_GetMappedColumnID(TSubclassOf<UObject> Class, const FString ColumnID) const;
	//
	virtual void DB_LaunchLoadScreen(const ESQLThreadSafety Mode, const FText Info);
	virtual void DB_DestroyLoadScreen();
	//
	//
#if WITH_EDITORONLY_DATA
	UPROPERTY(Category="DATABASE", VisibleDefaultsOnly, meta=(DisplayName="[DATABASE] Data Preview"))
	FSQLPreview DB_DataPreview;
#endif
public:
	virtual void PostLoad() override;
	virtual void BeginDestroy() override;
	virtual UWorld* GetWorld() const override;
	//
	virtual void DB_CheckThreadState();

	/// :: PROPERTIES ::
	
	/** SQL Blueprint Implementable Event: On Data Save. */
	UPROPERTY(Category="USQL", BlueprintAssignable, meta=(DisplayName="[DATABASE] On Begin Data Save:"))
	FSQLite_BeginDataSAVE EVENT_OnBeginDataSAVE;
	//
	/** SQL Blueprint Implementable Event: On Progress Data Save. */
	UPROPERTY(Category="USQL", BlueprintAssignable, meta=(DisplayName="[DATABASE] On Progress Data Save:"))
	FSQLite_ProgressDataSAVE EVENT_OnProgressDataSAVE;
	//
	/** SQL Blueprint Implementable Event: On Finish Data Save. */
	UPROPERTY(Category="USQL", BlueprintAssignable, meta=(DisplayName="[DATABASE] On Finish Data Save:"))
	FSQLite_FinishDataSAVE EVENT_OnFinishDataSAVE;
	//
	/** SQL Blueprint Implementable Event: On Data Load. */
	UPROPERTY(Category="USQL", BlueprintAssignable, meta=(DisplayName="[DATABASE] On Begin Data Load:"))
	FSQLite_BeginDataLOAD EVENT_OnBeginDataLOAD;
	//
	/** SQL Blueprint Implementable Event: On Progress Data Load. */
	UPROPERTY(Category="USQL", BlueprintAssignable, meta=(DisplayName="[DATABASE] On Progress Data Load:"))
	FSQLite_ProgressDataLOAD EVENT_OnProgressDataLOAD;
	//
	/** SQL Blueprint Implementable Event: On Finish Data Load. */
	UPROPERTY(Category="USQL", BlueprintAssignable, meta=(DisplayName="[DATABASE] On Finish Data Load:"))
	FSQLite_FinishDataLOAD EVENT_OnFinishDataLOAD;
	//
	UPROPERTY() FSQLite_ThreadChanged ThreadChangedDelegate;
	
	/// :: FUNCTIONS ::
	
	/** This will not generate any Progress Reports or Interface Events. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Execute Command: SQL"))
	ESQLResult DB_ExecuteCommand(const FString &SQL);
	//
	//
	/** Adds a SQL Execution String based on this Database's Table Columns that can Serialize Target Object's Properties into SQL DB File. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Add Command to QUEUE : SAVE"))
	void DB_EnqueueSAVE(const FString &SQL);
	//
	/** Adds a SQL Execution String based on this Blueprint's Table Columns that can Serialize Target Object's Properties from SQL DB File. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Add Command to QUEUE : LOAD"))
	void DB_EnqueueLOAD(const FString &SQL);
	//
	//
	/** Executes a SQL Command based on this Database's Table Columns that can Serialize Target Object's Properties into SQL DB File.
	This SQL Command will bypass Save Queue, executing to the DB immediately, progress won't be tracked.
	Excessive use of Immediate Save may cause concurrency issues and Database locks, so use this wisely! */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Immediate Command: SAVE (Object)"))
	void DB_OBJ_ImmediateSAVE(UObject* Object);
	//
	/** This will not generate any Progress Reports or Interface Events. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Immediate Command: SAVE (Component)"))
	void DB_CMP_ImmediateSAVE(UActorComponent* Component);
	//
	/** This will not generate any Progress Reports or Interface Events. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Immediate Command: SAVE (Actor)"))
	void DB_ACT_ImmediateSAVE(AActor* Actor);
	//
	/** Executes a SQL Command based on this Database's Table Columns that can Serialize Target Object's Properties into SQL DB File.
	This SQL Command will bypass Save Queue, executing to the DB immediately, progress won't be tracked.
	Excessive use of Immediate Save may cause concurrency issues and Database locks, so use this wisely!
	This will not generate any Progress Reports or Interface Events. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Immediate Command: LOAD (Object)"))
	void DB_OBJ_ImmediateLOAD(UObject* Object);
	//
	/** This will not generate any Progress Reports or Interface Events. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Immediate Command: LOAD (Component)"))
	void DB_CMP_ImmediateLOAD(UActorComponent* Component);
	//
	/** This will not generate any Progress Reports or Interface Events. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Immediate Command: LOAD (Actor)"))
	void DB_ACT_ImmediateLOAD(AActor* Actor);
	//
	//
	/** Updates a single 'Column Value' (Boolean), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Boolean)"))
	ESQLResult DB_UPDATE_Boolean(const FString &RowID, const FString &ColumnName, const bool Value);
	//
	/** Updates a single 'Column Value' (Byte), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Byte)"))
	ESQLResult DB_UPDATE_Byte(const FString &RowID, const FString &ColumnName, const uint8 Value);
	//
	/** Updates a single 'Column Value' (Integer), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Integer)"))
	ESQLResult DB_UPDATE_Integer(const FString &RowID, const FString &ColumnName, const int32 Value);
	//
	/** Updates a single 'Column Value' (Float), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Float)"))
	ESQLResult DB_UPDATE_Float(const FString &RowID, const FString &ColumnName, const float Value);
	//
	/** Updates a single 'Column Value' (Name), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Name)"))
	ESQLResult DB_UPDATE_Name(const FString &RowID, const FString &ColumnName, const FName Value);
	//
	/** Updates a single 'Column Value' (Text), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Text)"))
	ESQLResult DB_UPDATE_Text(const FString &RowID, const FString &ColumnName, const FText Value);
	//
	/** Updates a single 'Column Value' (String), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (String)"))
	ESQLResult DB_UPDATE_String(const FString &RowID, const FString &ColumnName, const FString Value);
	//
	/** Updates a single 'Column Value' (Date), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Date)"))
	ESQLResult DB_UPDATE_Date(const FString &RowID, const FString &ColumnName, const FDateTime Value);
	//
	/** Updates a single 'Column Value' (Color), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Color)"))
	ESQLResult DB_UPDATE_Color(const FString &RowID, const FString &ColumnName, const FColor Value);
	//
	/** Updates a single 'Column Value' (Vector 2D), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Vector 2D)"))
	ESQLResult DB_UPDATE_Vector2D(const FString &RowID, const FString &ColumnName, const FVector2D Value);
	//
	/** Updates a single 'Column Value' (Vector 3D), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Vector 3D)"))
	ESQLResult DB_UPDATE_Vector3D(const FString &RowID, const FString &ColumnName, const FVector Value);
	//
	/** Updates a single 'Column Value' (Rotator), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Rotator)"))
	ESQLResult DB_UPDATE_Rotator(const FString &RowID, const FString &ColumnName, const FRotator Value);
	//
	/** Updates a single 'Column Value' (Object Pointer), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Object)"))
	ESQLResult DB_UPDATE_Object(const FString &RowID, const FString &ColumnName, const UObject* OBJ);
	//
	//
	/** Updates an array of 'Column Values' (Boolean), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Boolean Array)"))
	ESQLResult DB_UPDATE_Boolean_Array(const FString &RowID, const FString &ColumnName, const TArray<bool>&Values);
	//
	/** Updates an array of 'Column Values' (Byte), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Byte Array)"))
	ESQLResult DB_UPDATE_Byte_Array(const FString &RowID, const FString &ColumnName, const TArray<uint8>&Values);
	//
	/** Updates an array of 'Column Values' (Integer), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Integer Array)"))
	ESQLResult DB_UPDATE_Integer_Array(const FString &RowID, const FString &ColumnName, const TArray<int32>&Values);
	//
	/** Updates an array of 'Column Values' (Float), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Float Array)"))
	ESQLResult DB_UPDATE_Float_Array(const FString &RowID, const FString &ColumnName, const TArray<float>&Values);
	//
	/** Updates an array of 'Column Values' (Name), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Name Array)"))
	ESQLResult DB_UPDATE_Name_Array(const FString &RowID, const FString &ColumnName, const TArray<FName>&Values);
	//
	/** Updates an array of 'Column Values' (Text), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Text Array)"))
	ESQLResult DB_UPDATE_Text_Array(const FString &RowID, const FString &ColumnName, const TArray<FText>&Values);
	//
	/** Updates an array of 'Column Values' (String), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (String Array)"))
	ESQLResult DB_UPDATE_String_Array(const FString &RowID, const FString &ColumnName, const TArray<FString>&Values);
	//
	/** Updates an array of 'Column Values' (Vector 2D), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Vector 2D Array)"))
	ESQLResult DB_UPDATE_Vector2D_Array(const FString &RowID, const FString &ColumnName, const TArray<FVector2D>&Values);
	//
	/** Updates an array of 'Column Values' (Vector 3D), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Vector 3D Array)"))
	ESQLResult DB_UPDATE_Vector3D_Array(const FString &RowID, const FString &ColumnName, const TArray<FVector>&Values);
	//
	/** Updates an array of 'Column Values' (Rotator), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Rotator Array)"))
	ESQLResult DB_UPDATE_Rotator_Array(const FString &RowID, const FString &ColumnName, const TArray<FRotator>&Values);
	//
	/** Updates an array of 'Column Values' (Object Pointer), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] UPDATE Column (Object Array)"))
	ESQLResult DB_UPDATE_Object_Array(const FString &RowID, const FString &ColumnName, const TArray<UObject*>&Values);
	//
	//
	/** Reads all 'Column Values' from a given Row, where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Row"))
	FSQLRow DB_SELECT_Row(const FString &RowID);
	//
	/** Deletes all 'Column Values' from a given Row, where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] DELETE Row"))
	ESQLResult DB_DELETE_Row(const FString &RowID);
	//
	//
	/** Reads a single 'Column Value' (Boolean), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (Boolean)"))
	bool DB_SELECT_Boolean(const FString &RowID, const FString &ColumnName);
	//
	/** Reads a single 'Column Value' (Byte), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (Byte)"))
	uint8 DB_SELECT_Byte(const FString &RowID, const FString &ColumnName);
	//
	/** Reads a single 'Column Value' (Integer), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (Integer)"))
	int32 DB_SELECT_Integer(const FString &RowID, const FString &ColumnName);
	//
	/** Reads a single 'Column Value' (Float), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (Float)"))
	float DB_SELECT_Float(const FString &RowID, const FString &ColumnName);
	//
	/** Reads a single 'Column Value' (Name), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (Name)"))
	FName DB_SELECT_Name(const FString &RowID, const FString &ColumnName);
	//
	/** Reads a single 'Column Value' (Text), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (Text)"))
	FText DB_SELECT_Text(const FString &RowID, const FString &ColumnName);
	//
	/** Reads a single 'Column Value' (String), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (String)"))
	FString DB_SELECT_String(const FString &RowID, const FString &ColumnName);
	//
	/** Reads a single 'Column Value' (Date), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (Date)"))
	FDateTime DB_SELECT_Date(const FString &RowID, const FString &ColumnName);
	//
	/** Reads a single 'Column Value' (Color), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (Color)"))
	FColor DB_SELECT_Color(const FString &RowID, const FString &ColumnName);
	//
	/** Reads a single 'Column Value' (Vector 2D), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (Vector 2D)"))
	FVector2D DB_SELECT_Vector2D(const FString &RowID, const FString &ColumnName);
	//
	/** Reads a single 'Column Value' (Vector 3D), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (Vector 3D)"))
	FVector DB_SELECT_Vector3D(const FString &RowID, const FString &ColumnName);
	//
	/** Reads a single 'Column Value' (Rotator), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (Rotator)"))
	FRotator DB_SELECT_Rotator(const FString &RowID, const FString &ColumnName);
	//
	/** Reads a single 'Column Value' (Object Pointer), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(WorldContext="Context", DisplayName="[DATABASE] SELECT Column (Object)"))
	UObject* DB_SELECT_Object(UObject* Context, const FString &RowID, const FString &ColumnName);
	//
	//
	/** Reads an array of 'Column Value' (Boolean), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (Boolean Array)"))
	TArray<bool> DB_SELECT_Boolean_Array(const FString &RowID, const FString &ColumnName);
	//
	/** Reads an array of 'Column Value' (Byte), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (Byte Array)"))
	TArray<uint8> DB_SELECT_Byte_Array(const FString &RowID, const FString &ColumnName);
	//
	/** Reads an array of 'Column Value' (Integer), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (Integer Array)"))
	TArray<int32> DB_SELECT_Integer_Array(const FString &RowID, const FString &ColumnName);
	//
	/** Reads an array of 'Column Value' (Float), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (Float Array)"))
	TArray<float> DB_SELECT_Float_Array(const FString &RowID, const FString &ColumnName);
	//
	/** Reads an array of 'Column Value' (Name), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (Name Array)"))
	TArray<FName> DB_SELECT_Name_Array(const FString &RowID, const FString &ColumnName);
	//
	/** Reads an array of 'Column Value' (Text), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (Text Array)"))
	TArray<FText> DB_SELECT_Text_Array(const FString &RowID, const FString &ColumnName);
	//
	/** Reads an array of 'Column Value' (String), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (String Array)"))
	TArray<FString> DB_SELECT_String_Array(const FString &RowID, const FString &ColumnName);
	//
	/** Reads an array of 'Column Value' (Vector 2D), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (Vector 2D Array)"))
	TArray<FVector2D> DB_SELECT_Vector2D_Array(const FString &RowID, const FString &ColumnName);
	//
	/** Reads an array of 'Column Value' (Vector 3D), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (Vector 3D Array)"))
	TArray<FVector> DB_SELECT_Vector3D_Array(const FString &RowID, const FString &ColumnName);
	//
	/** Reads an array of 'Column Value' (Rotator), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT Column (Rotator Array)"))
	TArray<FRotator> DB_SELECT_Rotator_Array(const FString &RowID, const FString &ColumnName);
	//
	/** Reads an array of 'Column Value' (Object Pointer), where Row equals specified ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(WorldContext="Context", DisplayName="[DATABASE] SELECT Column (Object Array)"))
	TArray<UObject*> DB_SELECT_Object_Array(UObject* Context, const FString &RowID, const FString &ColumnName);
	//
	//
	/** Reads all 'ID Values' (Name), where Field equals a Valid ID. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT ALL IDs (Name)"))
	TArray<FName> DB_SELECT_IDs();
	//
	/** Reads all 'Column Values' (Boolean), where Field equals a Valid Column. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT ALL from Column (Boolean)"))
	TArray<bool> DB_SELECT_Booleans(const FString &ColumnName);
	//
	/** Reads all 'Column Values' (Byte), where Field equals a Valid Column. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT ALL from Column (Byte)"))
	TArray<uint8> DB_SELECT_Bytes(const FString &ColumnName);
	//
	/** Reads all 'Column Values' (Integer), where Field equals a Valid Column. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT ALL from Column (Integer)"))
	TArray<int32> DB_SELECT_Integers(const FString &ColumnName);
	//
	/** Reads all 'Column Values' (Float), where Field equals a Valid Column. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT ALL from Column (Float)"))
	TArray<float> DB_SELECT_Floats(const FString &ColumnName);
	//
	/** Reads all 'Column Values' (Name), where Field equals a Valid Column. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT ALL from Column (Name)"))
	TArray<FName> DB_SELECT_Names(const FString &ColumnName);
	//
	/** Reads all 'Column Values' (Text), where Field equals a Valid Column. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT ALL from Column (Text)"))
	TArray<FText> DB_SELECT_Texts(const FString &ColumnName);
	//
	/** Reads all 'Column Values' (String), where Field equals a Valid Column. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT ALL from Column (String)"))
	TArray<FString> DB_SELECT_Strings(const FString &ColumnName);
	//
	/** Reads all 'Column Values' (Date), where Field equals a Valid Column. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT ALL from Column (Date)"))
	TArray<FDateTime> DB_SELECT_Dates(const FString &ColumnName);
	//
	/** Reads all 'Column Values' (Color), where Field equals a Valid Column. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT ALL from Column (Color)"))
	TArray<FColor> DB_SELECT_Colors(const FString &ColumnName);
	//
	/** Reads all 'Column Values' (Vector 2D), where Field equals a Valid Column. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT ALL from Column (Vector 2D)"))
	TArray<FVector2D> DB_SELECT_Vector2Ds(const FString &ColumnName);
	//
	/** Reads all 'Column Values' (Vector 3D), where Field equals a Valid Column. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT ALL from Column (Vector 3D)"))
	TArray<FVector> DB_SELECT_Vector3Ds(const FString &ColumnName);
	//
	/** Reads all 'Column Values' (Rotator), where Field equals a Valid Column. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] SELECT ALL from Column (Rotator)"))
	TArray<FRotator> DB_SELECT_Rotators(const FString &ColumnName);
	//
	/** Reads all 'Column Values' (Object Pointers), where Field equals a Valid Column. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(WorldContext="Context", DisplayName="[DATABASE] SELECT ALL from Column (Object)"))
	TArray<UObject*> DB_SELECT_Objects(UObject* Context, const FString &ColumnName);
	//
	//
	/** Generates a SQL Execution String for Target Object based on this Blueprint's Table Fields that can be used to serialize Target Object's Properties into this SQL DB File. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Generate SQL from OBJECT : INSERT"))
	FString DB_GenerateSQL_Object_INSERT(UObject* Object);
	//
	/** Generates a SQL Execution String for Target Object based on this Blueprint's Table Fields that can be used to update Target Object's Properties into this SQL DB File. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Generate SQL from OBJECT : UPDATE"))
	FString DB_GenerateSQL_Object_UPDATE(UObject* Object);
	//
	/** Generates a SQL Execution String for Target Object based on this Blueprint's Table Fields that can be used to read Target Object's Properties from SQL DB File. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Generate SQL from OBJECT : SELECT"))
	FString DB_GenerateSQL_Object_SELECT(UObject* Object);
	//
	/** Generates a SQL Execution String for Target Object based on this Blueprint's Table Fields that can be used to find and delete Target Object's Properties from SQL DB File. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Generate SQL from OBJECT : DELETE"))
	FString DB_GenerateSQL_Object_DELETE(UObject* Object);
	//
	/** Generates a SQL Execution String for Target Actor based on this Blueprint's Table Fields that can be used to serialize Target Actor's Properties into this SQL DB File. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Generate SQL from ACTOR : INSERT"))
	FString DB_GenerateSQL_Actor_INSERT(AActor* Actor);
	//
	/** Generates a SQL Execution String for Target Actor based on this Blueprint's Table Fields that can be used to serialize Target Actor's Properties into this SQL DB File. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Generate SQL from ACTOR : UPDATE"))
	FString DB_GenerateSQL_Actor_UPDATE(AActor* Actor);
	//
	/** Generates a SQL Execution String for Target Actor based on this Blueprint's Table Fields that can be used to read Target Actor's Properties from SQL DB File. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Generate SQL from ACTOR : SELECT"))
	FString DB_GenerateSQL_Actor_SELECT(AActor* Actor);
	//
	/** Generates a SQL Execution String for Target Actor based on this Blueprint's Table Fields that can be used to find and delete Target Actor's Properties from SQL DB File. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Generate SQL from ACTOR : DELETE"))
	FString DB_GenerateSQL_Actor_DELETE(AActor* Actor);
	//
	/** Generates a SQL Execution String for Target Component based on this Blueprint's Table Fields that can be used to serialize Target Component's Properties into this SQL DB File. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Generate SQL from COMPONENT : INSERT"))
	FString DB_GenerateSQL_Component_INSERT(UActorComponent* Component);
	//
	/** Generates a SQL Execution String for Target Component based on this Blueprint's Table Fields that can be used to serialize Target Component's Properties into this SQL DB File. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Generate SQL from COMPONENT : UPDATE"))
	FString DB_GenerateSQL_Component_UPDATE(UActorComponent* Component);
	//
	/** Generates a SQL Execution String for Target Component based on this Blueprint's Table Fields that can be used to read Target Component's Properties from SQL DB File. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Generate SQL from COMPONENT : SELECT"))
	FString DB_GenerateSQL_Component_SELECT(UActorComponent* Component);
	//
	/** Generates a SQL Execution String for Target Component based on this Blueprint's Table Fields that can be used to find and delete Target Component's Properties from SQL DB File. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Generate SQL from COMPONENT : DELETE"))
	FString DB_GenerateSQL_Component_DELETE(UActorComponent* Component);
	//
	//
	/** After SQL Data is loaded, from DB File, call this method to extract Properties and apply Values to any Target Object loaded. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Unpack Data from QUEUE : OBJECT"))
	void DB_UnpackObjectDATA(UObject* Object, const FSQLRow Data, const bool Log);
	//
	/** After SQL Data is loaded, from DB File, call this method to extract Properties and apply Values to any Target Actor loaded. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Unpack Data from QUEUE : ACTOR"))
	void DB_UnpackActorDATA(AActor* Actor, const FSQLRow Data, const bool Log);
	//
	/** After SQL Data is loaded, from DB File, call this method to extract Properties and apply Values to any Target Component loaded. */
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Unpack Data from QUEUE : COMPONENT"))
	void DB_UnpackComponentDATA(UActorComponent* Component, const FSQLRow Data, const bool Log);
	//
	//
	/** DB: Save Progress Report Interface. */
	UFUNCTION()
	void DB_DoSaveProgressReport(USQLite* Database, const float Progress);
	//
	/** DB: Load Progress Report Interface. */
	UFUNCTION()
	void DB_DoLoadProgressReport(USQLite* Database, const float Progress);
	//
	/** DB: Save Interface. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, BlueprintCallable, meta=(WorldContext="Context", DisplayName="[DATABASE] Save All Data", Keywords="DB Save Interface"))
	void DB_Save(UObject* Context, const ESQLSaveMode Mode);
	//
	/** DB: Load Interface. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, BlueprintCallable, meta=(WorldContext="Context", DisplayName="[DATABASE] Load All Data", Keywords="DB Load Interface"))
	void DB_Load(UObject* Context);
	//
	//
	/** DB: Save Interface. */
	UFUNCTION(Category="DNA", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Prepare To Save:", Keywords="DB Save Prepare"))
	void DB_PrepareToSave(USQLite* Database, const ESQLSaveMode Mode);
	//
	/** DB: Load Interface. */
	UFUNCTION(Category="DNA", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Prepare To Load:", Keywords="DB Load Prepare"))
	void DB_PrepareToLoad(USQLite* Database);
	//
	/** DB: Begin Save. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Save Data Begin:", Keywords="DB Save Begin"))
	void DB_OnBeginSave(USQLite* Database);
	//
	/** DB: Progress Save. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Save Data Progress:", Keywords="DB Save Progress"))
	void DB_OnProgressSave(USQLite* Database, const float Progress);
	//
	/** DB: Finish Save. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Save Data Finish:", Keywords="DB Save Finish"))
	void DB_OnFinishSave(USQLite* Database, const bool Success);
	//
	/** DB: Begin Load. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Load Data Begin:", Keywords="DB Load Begin"))
	void DB_OnBeginLoad(USQLite* Database);
	//
	/** DB: Progress Load. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Load Data Progress:", Keywords="DB Load Progress"))
	void DB_OnProgressLoad(USQLite* Database, const float Progress, FSQLRow Data);
	//
	/** DB: Finish Load. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Load Data Finish:", Keywords="DB Load Finish"))
	void DB_OnFinishLoad(USQLite* Database, const bool Success);
	//
	//
	/** DB: Progress Save [Threaded].
	WARNING!! Called outside Game's Thread.
	Do NOT call Game Functions from this Event. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Save Data Progress [Threaded]:", Keywords="DB Save Progress"))
	void DB_OnProgressSave__Threaded(USQLite* Database, const float Progress);
	//
	/** DB: Finish Save [Threaded].
	WARNING!! Called outside Game's Thread.
	Do NOT call Game Functions from this Event. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Save Data Finish [Threaded]:", Keywords="DB Save Finish"))
	void DB_OnFinishSave__Threaded(USQLite* Database, const bool Success);
	//
	/** DB: Progress Load [Threaded].
	WARNING!! Called outside Game's Thread.
	Do NOT call Game Functions from this Event. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Load Data Progress [Threaded]:", Keywords="DB Load Progress"))
	void DB_OnProgressLoad__Threaded(USQLite* Database, FSQLRow Data, const float Progress);
	//
	/** DB: Finish Load [Threaded].
	WARNING!! Called outside Game's Thread.
	Do NOT call Game Functions from this Event. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Load Data Finish [Threaded]:", Keywords="DB Load Finish"))
	void DB_OnFinishLoad__Threaded(USQLite* Database, const bool Success);
	//
	//
	/** DB: Global Save Progress Report [Threaded].
	WARNING!! Called outside Game's Thread.
	Do NOT call Game Functions from this Event. */
	UFUNCTION(Category="USQL", BlueprintImplementableEvent, meta=(DisplayName="[DATABASE] On Global Save Progress [Threaded]:", Keywords="DB Save Progress"))
	void DB_OnGlobalSaveProgressReport__Threaded(const float Progress);
	//
	/** DB: Global Load Progress Report [Threaded].
	WARNING!! Called outside Game's Thread.
	Do NOT call Game Functions from this Event. */
	UFUNCTION(Category="USQL", BlueprintImplementableEvent, meta=(DisplayName="[DATABASE] On Global Load Progress [Threaded]:", Keywords="DB Load Progress"))
	void DB_OnGlobalLoadProgressReport__Threaded(const float Progress);
	//
	//
	/** Gets Current Version of Table as a String. */
	UFUNCTION(Category="USQL", BlueprintPure, meta=(DisplayName="[DATABASE] Get Version", Keywords="DB Get Version"))
	FString DB_GetVersion() const;
	//
	//
	UFUNCTION(Category="USQL", BlueprintCallable, meta=(DisplayName="[DATABASE] Get Thread Safety", Keywords="DB Thread Safety", ExpandEnumAsExecs="ThreadSafety"))
	void DB_GetThreadSafety(ESQLThreadSafety &ThreadSafety, const FSQLite_ThreadChanged &ThreadChangedCallback);
	//
	//
#if WITH_EDITORONLY_DATA
	void OnTableDirty();
	void OnVersioningDelete();
	void OnPreviewInsert(const FString SQL);
	void OnPreviewDelete(const FString RowID);
	void OnPreviewPaste(const FString FromID, const FString ToID);
	//
	void DB_RefreshPreview();
	const bool DB_IsPreviewDirty();
	void DB_UpsertPreview(const FString SQL);
#endif
	//
	//
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent &PECP) override;
#endif
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SQLite Threading Interface:

class DBS_ExecuteQueue_Task : public FNonAbandonableTask {
private:
	USQLite* Database;
public:
	FORCEINLINE TStatId GetStatId() const {RETURN_QUICK_DECLARE_CYCLE_STAT(DBS_ExecuteQueue_Task,STATGROUP_ThreadPoolAsyncTasks);}
	DBS_ExecuteQueue_Task(USQLite* Database) {check(Database); this->Database = Database;}
	//
	void DoWork() {USQL::DBS_ExecuteQueue__Threaded(Database);}
};

class DBL_ExecuteQueue_Task : public FNonAbandonableTask {
private:
	USQLite* Database;
public:
	FORCEINLINE TStatId GetStatId() const {RETURN_QUICK_DECLARE_CYCLE_STAT(DBL_ExecuteQueue_Task,STATGROUP_ThreadPoolAsyncTasks);}
	DBL_ExecuteQueue_Task(USQLite* Database) {check(Database); this->Database = Database;}
	//
	void DoWork() {USQL::DBL_ExecuteQueue__Threaded(Database);}
};

static void CheckThreadState(USQLite* Database) {
	if (!IsInGameThread()) {return;}
	Database->DB_CheckThreadState();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// USQLite Base Serializable Object:

UCLASS(classGroup=Synaptech, Category="Synaptech", BlueprintType, Blueprintable, EditInlineNew, meta=(DisplayName="SQL Auto-Serializable Object"))
class USQLITE_API USQLSerializable_OBJ : public UObject, public ISQLiteData {
	GENERATED_BODY()
protected:
	/** Enables Debug Logs. */
	UPROPERTY(Category="USQL", EditDefaultsOnly, AdvancedDisplay)
	bool Debug;
public:
	/** DB: Save Interface. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Prepare To Save:", Keywords="DB Save Prepare"))
	void DB_PrepareToSave(USQLite* Database, const ESQLSaveMode Mode);
	//
	/** DB: Load Interface. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Prepare To Load:", Keywords="DB Load Prepare"))
	void DB_PrepareToLoad(USQLite* Database);
	//
	/** DB: Begin Save. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Save Data Begin:", Keywords="DB Save Begin"))
	void DB_OnBeginSave(USQLite* Database);
	//
	/** DB: Progress Save. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Save Data Progress:", Keywords="DB Save Progress"))
	void DB_OnProgressSave(USQLite* Database, const float Progress);
	//
	/** DB: Finish Save. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Save Data Finish:", Keywords="DB Save Finish"))
	void DB_OnFinishSave(USQLite* Database, const bool Success);
	//
	/** DB: Begin Load. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Load Data Begin:", Keywords="DB Load Begin"))
	void DB_OnBeginLoad(USQLite* Database);
	//
	/** DB: Progress Load. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Load Data Progress:", Keywords="DB Load Progress"))
	void DB_OnProgressLoad(USQLite* Database, const float Progress, FSQLRow Data);
	//
	/** DB: Finish Load. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Load Data Finish:", Keywords="DB Load Finish"))
	void DB_OnFinishLoad(USQLite* Database, const bool Success);
	//
	//
	/** DB: Progress Save [Threaded]. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Save Data Progress [Threaded]:", Keywords="DB Save Progress"))
	void DB_OnProgressSave__Threaded(USQLite* Database, const float Progress);
	//
	/** DB: Finish Save [Threaded]. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Save Data Finish [Threaded]:", Keywords="DB Save Finish"))
	void DB_OnFinishSave__Threaded(USQLite* Database, const bool Success);
	//
	/** DB: Progress Load [Threaded]. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Load Data Progress [Threaded]:", Keywords="DB Load Progress"))
	void DB_OnProgressLoad__Threaded(USQLite* Database, FSQLRow Data, const float Progress);
	//
	/** DB: Finish Load [Threaded]. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Load Data Finish [Threaded]:", Keywords="DB Load Finish"))
	void DB_OnFinishLoad__Threaded(USQLite* Database, const bool Success);
	//
	//
	/** DB: Global Load Progress Report [Threaded]. */
	UFUNCTION(Category="USQL", BlueprintImplementableEvent, meta=(DisplayName="[DATABASE] On Global Load Progress [Threaded]:", Keywords="DB Load Progress"))
	void DB_OnGlobalLoadProgressReport__Threaded(const float Progress);
	//
	/** DB: Global Save Progress Report [Threaded]. */
	UFUNCTION(Category="USQL", BlueprintImplementableEvent, meta=(DisplayName="[DATABASE] On Global Save Progress [Threaded]:", Keywords="DB Save Progress"))
	void DB_OnGlobalSaveProgressReport__Threaded(const float Progress);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// USQLite Base Serializable Component:

UCLASS(classGroup=Synaptech, Category="Synaptech", BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent, DisplayName="SQL Auto-Serializable Component"))
class USQLITE_API USQLSerializable_CMP : public UActorComponent, public ISQLiteData {
	GENERATED_BODY()
protected:
	/** Enables Debug Logs. */
	UPROPERTY(Category="USQL", EditDefaultsOnly, AdvancedDisplay)
	bool Debug;
public:
	/** DB: Save Interface. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Prepare To Save:", Keywords="DB Save Prepare"))
	void DB_PrepareToSave(USQLite* Database, const ESQLSaveMode Mode);
	//
	/** DB: Load Interface. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Prepare To Load:", Keywords="DB Load Prepare"))
	void DB_PrepareToLoad(USQLite* Database);
	//
	/** DB: Begin Save. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Save Data Begin:", Keywords="DB Save Begin"))
	void DB_OnBeginSave(USQLite* Database);
	//
	/** DB: Progress Save. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Save Data Progress:", Keywords="DB Save Progress"))
	void DB_OnProgressSave(USQLite* Database, const float Progress);
	//
	/** DB: Finish Save. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Save Data Finish:", Keywords="DB Save Finish"))
	void DB_OnFinishSave(USQLite* Database, const bool Success);
	//
	/** DB: Begin Load. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Load Data Begin:", Keywords="DB Load Begin"))
	void DB_OnBeginLoad(USQLite* Database);
	//
	/** DB: Progress Load. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Load Data Progress:", Keywords="DB Load Progress"))
	void DB_OnProgressLoad(USQLite* Database, const float Progress, FSQLRow Data);
	//
	/** DB: Finish Load. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Load Data Finish:", Keywords="DB Load Finish"))
	void DB_OnFinishLoad(USQLite* Database, const bool Success);
	//
	//
	/** DB: Progress Save [Threaded]. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Save Data Progress [Threaded]:", Keywords="DB Save Progress"))
	void DB_OnProgressSave__Threaded(USQLite* Database, const float Progress);
	//
	/** DB: Finish Save [Threaded]. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Save Data Finish [Threaded]:", Keywords="DB Save Finish"))
	void DB_OnFinishSave__Threaded(USQLite* Database, const bool Success);
	//
	/** DB: Progress Load [Threaded]. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Load Data Progress [Threaded]:", Keywords="DB Load Progress"))
	void DB_OnProgressLoad__Threaded(USQLite* Database, FSQLRow Data, const float Progress);
	//
	/** DB: Finish Load [Threaded]. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Load Data Finish [Threaded]:", Keywords="DB Load Finish"))
	void DB_OnFinishLoad__Threaded(USQLite* Database, const bool Success);
	//
	//
	/** DB: Global Load Progress Report [Threaded]. */
	UFUNCTION(Category="USQL", BlueprintImplementableEvent, meta=(DisplayName="[DATABASE] On Global Load Progress [Threaded]:", Keywords="DB Load Progress"))
	void DB_OnGlobalLoadProgressReport__Threaded(const float Progress);
	//
	/** DB: Global Save Progress Report [Threaded]. */
	UFUNCTION(Category="USQL", BlueprintImplementableEvent, meta=(DisplayName="[DATABASE] On Global Save Progress [Threaded]:", Keywords="DB Save Progress"))
	void DB_OnGlobalSaveProgressReport__Threaded(const float Progress);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// USQLite Base Serializable Actor:

UCLASS(classGroup=Synaptech, Category="Synaptech", BlueprintType, Blueprintable, meta=(DisplayName="SQL Auto-Serializable Actor Class"))
class USQLITE_API ASQLSerializable : public AActor, public ISQLiteData {
	GENERATED_BODY()
protected:
	/** Enables Debug Logs. */
	UPROPERTY(Category="USQL", EditDefaultsOnly, AdvancedDisplay)
	bool Debug;
public:
	/** DB: Save Interface. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Prepare To Save:", Keywords="DB Save Prepare"))
	void DB_PrepareToSave(USQLite* Database, const ESQLSaveMode Mode);
	//
	/** DB: Load Interface. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Prepare To Load:", Keywords="DB Load Prepare"))
	void DB_PrepareToLoad(USQLite* Database);
	//
	/** DB: Begin Save. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Save Data Begin:", Keywords="DB Save Begin"))
	void DB_OnBeginSave(USQLite* Database);
	//
	/** DB: Progress Save. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Save Data Progress:", Keywords="DB Save Progress"))
	void DB_OnProgressSave(USQLite* Database, const float Progress);
	//
	/** DB: Finish Save. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Save Data Finish:", Keywords="DB Save Finish"))
	void DB_OnFinishSave(USQLite* Database, const bool Success);
	//
	/** DB: Begin Load. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Load Data Begin:", Keywords="DB Load Begin"))
	void DB_OnBeginLoad(USQLite* Database);
	//
	/** DB: Progress Load. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Load Data Progress:", Keywords="DB Load Progress"))
	void DB_OnProgressLoad(USQLite* Database, const float Progress, FSQLRow Data);
	//
	/** DB: Finish Load. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Load Data Finish:", Keywords="DB Load Finish"))
	void DB_OnFinishLoad(USQLite* Database, const bool Success);
	//
	//
	/** DB: Progress Save [Threaded]. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Save Data Progress [Threaded]:", Keywords="DB Save Progress"))
	void DB_OnProgressSave__Threaded(USQLite* Database, const float Progress);
	//
	/** DB: Finish Save [Threaded]. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Save Data Finish [Threaded]:", Keywords="DB Save Finish"))
	void DB_OnFinishSave__Threaded(USQLite* Database, const bool Success);
	//
	/** DB: Progress Load [Threaded]. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Load Data Progress [Threaded]:", Keywords="DB Load Progress"))
	void DB_OnProgressLoad__Threaded(USQLite* Database, FSQLRow Data, const float Progress);
	//
	/** DB: Finish Load [Threaded]. */
	UFUNCTION(Category="USQL", BlueprintNativeEvent, meta=(DisplayName="[DATABASE] On Load Data Finish [Threaded]:", Keywords="DB Load Finish"))
	void DB_OnFinishLoad__Threaded(USQLite* Database, const bool Success);
	//
	//
	/** DB: Global Load Progress Report [Threaded]. */
	UFUNCTION(Category="USQL", BlueprintImplementableEvent, meta=(DisplayName="[DATABASE] On Global Load Progress [Threaded]:", Keywords="DB Load Progress"))
	void DB_OnGlobalLoadProgressReport__Threaded(const float Progress);
	//
	/** DB: Global Save Progress Report [Threaded]. */
	UFUNCTION(Category="USQL", BlueprintImplementableEvent, meta=(DisplayName="[DATABASE] On Global Save Progress [Threaded]:", Keywords="DB Save Progress"))
	void DB_OnGlobalSaveProgressReport__Threaded(const float Progress);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////