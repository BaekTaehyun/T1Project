// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.


#include "T1ContentBrowserSingleton.h"
#include "Textures/SlateIcon.h"
#include "Misc/ConfigCacheIni.h"
#include "Widgets/SWindow.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBox.h"
#include "Framework/Docking/WorkspaceItem.h"
#include "Framework/Docking/TabManager.h"
#include "EditorStyleSet.h"
#include "Editor.h"
#include "T1ContentBrowserLog.h"
#include "T1ContentBrowserUtils.h"
#include "T1SAssetPicker.h"
#include "T1SPathPicker.h"
#include "T1SCollectionPicker.h"
#include "T1SContentBrowser.h"
#include "T1ContentBrowserEditor/Public/T1ContentBrowserEditor.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructure.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"
#include "IDocumentation.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"
#include "T1SAssetDialog.h"
#include "TutorialMetaData.h"
#include "Widgets/Docking/SDockTab.h"
#include "T1NativeClassHierarchy.h"
#include "T1EmptyFolderVisibilityManager.h"
#include "T1CollectionAssetRegistryBridge.h"
#include "T1ContentBrowserCommands.h"

#define LOCTEXT_NAMESPACE "T1ContentBrowserEditor"

FT1ContentBrowserSingleton::FT1ContentBrowserSingleton()
	: EmptyFolderVisibilityManager(MakeShared<FT1EmptyFolderVisibilityManager>())
	, CollectionAssetRegistryBridge(MakeShared<FT1CollectionAssetRegistryBridge>())
	, SettingsStringID(0)
{
	// Register the tab spawners for all content browsers
	const FSlateIcon ContentBrowserIcon(FEditorStyle::GetStyleSetName(), "T1ContentBrowser.TabIcon");
	const IWorkspaceMenuStructure& MenuStructure = WorkspaceMenu::GetMenuStructure();
	TSharedRef<FWorkspaceItem> ContentBrowserGroup = MenuStructure.GetToolsCategory()->AddGroup(
		LOCTEXT( "WorkspaceMenu_ContentBrowserCategory", "Content Browser" ),
		LOCTEXT( "ContentBrowserMenuTooltipText", "Open a Content Browser tab." ),
		ContentBrowserIcon,
		true);

	for ( int32 BrowserIdx = 0; BrowserIdx < ARRAY_COUNT(ContentBrowserTabIDs); BrowserIdx++ )
	{
		const FName TabID = FName(*FString::Printf(TEXT("T1ContentBrowserTab%d"), BrowserIdx + 1));
		ContentBrowserTabIDs[BrowserIdx] = TabID;

		const FText DefaultDisplayName = GetContentBrowserLabelWithIndex( BrowserIdx );

		FGlobalTabmanager::Get()->RegisterNomadTabSpawner( TabID, FOnSpawnTab::CreateRaw(this, &FT1ContentBrowserSingleton::SpawnContentBrowserTab, BrowserIdx) )
			.SetDisplayName(DefaultDisplayName)
			.SetTooltipText( LOCTEXT( "ContentBrowserMenuTooltipText", "Open a Content Browser tab." ) )
			.SetGroup( ContentBrowserGroup )
			.SetIcon(ContentBrowserIcon);
	}

	// Register a couple legacy tab ids
	FGlobalTabmanager::Get()->AddLegacyTabType( "LevelEditorContentBrowser", "ContentBrowserTab1" );
	FGlobalTabmanager::Get()->AddLegacyTabType( "MajorContentBrowserTab", "ContentBrowserTab2" );

	// Register to be notified when properties are edited
	FEditorDelegates::LoadSelectedAssetsIfNeeded.AddRaw(this, &FT1ContentBrowserSingleton::OnEditorLoadSelectedAssetsIfNeeded);

	FT1ContentBrowserCommands::Register();
}

FT1ContentBrowserSingleton::~FT1ContentBrowserSingleton()
{
	FEditorDelegates::LoadSelectedAssetsIfNeeded.RemoveAll(this);

	if ( FSlateApplication::IsInitialized() )
	{
		for ( int32 BrowserIdx = 0; BrowserIdx < ARRAY_COUNT(ContentBrowserTabIDs); BrowserIdx++ )
		{
			FGlobalTabmanager::Get()->UnregisterNomadTabSpawner( ContentBrowserTabIDs[BrowserIdx] );
		}
	}
}

TSharedRef<SWidget> FT1ContentBrowserSingleton::CreateAssetPicker(const FT1AssetPickerConfig& AssetPickerConfig)
{
	return SNew( T1SAssetPicker )
		.IsEnabled( FSlateApplication::Get().GetNormalExecutionAttribute() )
		.AssetPickerConfig(AssetPickerConfig);
}

TSharedRef<SWidget> FT1ContentBrowserSingleton::CreatePathPicker(const FT1PathPickerConfig& PathPickerConfig)
{
	return SNew( ST1PathPicker )
		.IsEnabled( FSlateApplication::Get().GetNormalExecutionAttribute() )
		.PathPickerConfig(PathPickerConfig);
}

TSharedRef<class SWidget> FT1ContentBrowserSingleton::CreateCollectionPicker(const FT1CollectionPickerConfig& CollectionPickerConfig)
{
	return SNew( T1SCollectionPicker )
		.IsEnabled( FSlateApplication::Get().GetNormalExecutionAttribute() )
		.CollectionPickerConfig(CollectionPickerConfig);
}

void FT1ContentBrowserSingleton::CreateOpenAssetDialog(const FT1OpenAssetDialogConfig& InConfig,
													 const FT1OnAssetsChosenForOpen& InOnAssetsChosenForOpen,
													 const FT1OnAssetDialogCancelled& InOnAssetDialogCancelled)
{
	const bool bModal = false;
	TSharedRef<ST1AssetDialog> AssetDialog = SNew(ST1AssetDialog, InConfig);
	AssetDialog->SetOnAssetsChosenForOpen(InOnAssetsChosenForOpen);
	AssetDialog->SetOnAssetDialogCancelled(InOnAssetDialogCancelled);
	SharedCreateAssetDialogWindow(AssetDialog, InConfig, bModal);
}

TArray<FAssetData> FT1ContentBrowserSingleton::CreateModalOpenAssetDialog(const FT1OpenAssetDialogConfig& InConfig)
{
	struct FModalResults
	{
		void OnAssetsChosenForOpen(const TArray<FAssetData>& SelectedAssets)
		{
			SavedResults = SelectedAssets;
		}

		TArray<FAssetData> SavedResults;
	};

	FModalResults ModalWindowResults;
	FT1OnAssetsChosenForOpen OnAssetsChosenForOpenDelegate = FT1OnAssetsChosenForOpen::CreateRaw(&ModalWindowResults, &FModalResults::OnAssetsChosenForOpen);

	const bool bModal = true;
	TSharedRef<ST1AssetDialog> AssetDialog = SNew(ST1AssetDialog, InConfig);
	AssetDialog->SetOnAssetsChosenForOpen(OnAssetsChosenForOpenDelegate);
	SharedCreateAssetDialogWindow(AssetDialog, InConfig, bModal);

	return ModalWindowResults.SavedResults;
}

void FT1ContentBrowserSingleton::CreateSaveAssetDialog(const FT1SaveAssetDialogConfig& InConfig,
													 const FT1OnObjectPathChosenForSave& InOnObjectPathChosenForSave,
													 const FT1OnAssetDialogCancelled& InOnAssetDialogCancelled)
{
	const bool bModal = false;
	TSharedRef<ST1AssetDialog> AssetDialog = SNew(ST1AssetDialog, InConfig);
	AssetDialog->SetOnObjectPathChosenForSave(InOnObjectPathChosenForSave);
	AssetDialog->SetOnAssetDialogCancelled(InOnAssetDialogCancelled);
	SharedCreateAssetDialogWindow(AssetDialog, InConfig, bModal);
}

FString FT1ContentBrowserSingleton::CreateModalSaveAssetDialog(const FT1SaveAssetDialogConfig& InConfig)
{
	struct FModalResults
	{
		void OnObjectPathChosenForSave(const FString& ObjectPath)
		{
			SavedResult = ObjectPath;
		}

		FString SavedResult;
	};

	FModalResults ModalWindowResults;
	FT1OnObjectPathChosenForSave OnObjectPathChosenForSaveDelegate = FT1OnObjectPathChosenForSave::CreateRaw(&ModalWindowResults, &FModalResults::OnObjectPathChosenForSave);

	const bool bModal = true;
	TSharedRef<ST1AssetDialog> AssetDialog = SNew(ST1AssetDialog, InConfig);
	AssetDialog->SetOnObjectPathChosenForSave(OnObjectPathChosenForSaveDelegate);
	SharedCreateAssetDialogWindow(AssetDialog, InConfig, bModal);

	return ModalWindowResults.SavedResult;
}

bool FT1ContentBrowserSingleton::HasPrimaryContentBrowser() const
{
	if ( PrimaryContentBrowser.IsValid() )
	{
		// There is a primary content browser
		return true;
	}
	else
	{
		for (int32 BrowserIdx = 0; BrowserIdx < AllContentBrowsers.Num(); ++BrowserIdx)
		{
			if ( AllContentBrowsers[BrowserIdx].IsValid() )
			{
				// There is at least one valid content browser
				return true;
			}
		}

		// There were no valid content browsers
		return false;
	}
}

void FT1ContentBrowserSingleton::FocusPrimaryContentBrowser(bool bFocusSearch)
{
	// See if the primary content browser is still valid
	if ( !PrimaryContentBrowser.IsValid() )
	{
		ChooseNewPrimaryBrowser();
	}

	if ( PrimaryContentBrowser.IsValid() )
	{
		FocusContentBrowser( PrimaryContentBrowser.Pin() );
	}
	else
	{
		// If we couldn't find a primary content browser, open one
		SummonNewBrowser();
	}

	// Do we also want to focus on the search box of the content browser?
	if ( bFocusSearch && PrimaryContentBrowser.IsValid() )
	{
		PrimaryContentBrowser.Pin()->SetKeyboardFocusOnSearch();
	}
}

void FT1ContentBrowserSingleton::CreateNewAsset(const FString& DefaultAssetName, const FString& PackagePath, UClass* AssetClass, UFactory* Factory)
{
	FocusPrimaryContentBrowser(false);

	if ( PrimaryContentBrowser.IsValid() )
	{
		PrimaryContentBrowser.Pin()->CreateNewAsset(DefaultAssetName, PackagePath, AssetClass, Factory);
	}
}

TSharedPtr<ST1ContentBrowser> FT1ContentBrowserSingleton::FindContentBrowserToSync(bool bAllowLockedBrowsers)
{
	TSharedPtr<ST1ContentBrowser> ContentBrowserToSync;

	if ( !PrimaryContentBrowser.IsValid() )
	{
		ChooseNewPrimaryBrowser();
	}

	if ( PrimaryContentBrowser.IsValid() && (bAllowLockedBrowsers || !PrimaryContentBrowser.Pin()->IsLocked()) )
	{
		// If the primary content browser is not locked, sync it
		ContentBrowserToSync = PrimaryContentBrowser.Pin();
	}
	else
	{
		// If there is no primary or it is locked, find the first non-locked valid browser
		for (int32 BrowserIdx = 0; BrowserIdx < AllContentBrowsers.Num(); ++BrowserIdx)
		{
			if ( AllContentBrowsers[BrowserIdx].IsValid() && (bAllowLockedBrowsers || !AllContentBrowsers[BrowserIdx].Pin()->IsLocked()) )
			{
				ContentBrowserToSync = AllContentBrowsers[BrowserIdx].Pin();
				break;
			}
		}
	}

	if ( !ContentBrowserToSync.IsValid() )
	{
		// There are no valid, unlocked browsers, attempt to summon a new one.
		SummonNewBrowser(bAllowLockedBrowsers);

		// Now try to find a non-locked valid browser again, now that a new one may exist
		for (int32 BrowserIdx = 0; BrowserIdx < AllContentBrowsers.Num(); ++BrowserIdx)
		{
			if ( AllContentBrowsers[BrowserIdx].IsValid() && (bAllowLockedBrowsers || !AllContentBrowsers[BrowserIdx].Pin()->IsLocked()) )
			{
				ContentBrowserToSync = AllContentBrowsers[BrowserIdx].Pin();
				break;
			}
		}
	}

	if ( !ContentBrowserToSync.IsValid() )
	{
		UE_LOG( LogContentBrowser, Log, TEXT( "Unable to sync content browser, all browsers appear to be locked" ) );
	}

	return ContentBrowserToSync;
}

void FT1ContentBrowserSingleton::SyncBrowserToAssets(const TArray<FAssetData>& AssetDataList, bool bAllowLockedBrowsers, bool bFocusContentBrowser)
{
	TSharedPtr<ST1ContentBrowser> ContentBrowserToSync = FindContentBrowserToSync(bAllowLockedBrowsers);

	if ( ContentBrowserToSync.IsValid() )
	{
		// Finally, focus and sync the browser that was found
		if (bFocusContentBrowser)
		{
			FocusContentBrowser(ContentBrowserToSync);
		}
		ContentBrowserToSync->SyncToAssets(AssetDataList);
	}
}

void FT1ContentBrowserSingleton::SyncBrowserToAssets(const TArray<UObject*>& AssetList, bool bAllowLockedBrowsers, bool bFocusContentBrowser)
{
	// Convert UObject* array to FAssetData array
	TArray<FAssetData> AssetDataList;
	for (int32 AssetIdx = 0; AssetIdx < AssetList.Num(); ++AssetIdx)
	{
		if ( AssetList[AssetIdx] )
		{
			AssetDataList.Add( FAssetData(AssetList[AssetIdx]) );
		}
	}

	SyncBrowserToAssets(AssetDataList, bAllowLockedBrowsers, bFocusContentBrowser);
}

void FT1ContentBrowserSingleton::SyncBrowserToFolders(const TArray<FString>& FolderList, bool bAllowLockedBrowsers, bool bFocusContentBrowser)
{
	TSharedPtr<ST1ContentBrowser> ContentBrowserToSync = FindContentBrowserToSync(bAllowLockedBrowsers);

	if ( ContentBrowserToSync.IsValid() )
	{
		// Finally, focus and sync the browser that was found
		if (bFocusContentBrowser)
		{
			FocusContentBrowser(ContentBrowserToSync);
		}
		ContentBrowserToSync->SyncToFolders(FolderList);
	}
}

void FT1ContentBrowserSingleton::SyncBrowserTo(const FT1ContentBrowserSelection& ItemSelection, bool bAllowLockedBrowsers, bool bFocusContentBrowser)
{
	TSharedPtr<ST1ContentBrowser> ContentBrowserToSync = FindContentBrowserToSync(bAllowLockedBrowsers);

	if ( ContentBrowserToSync.IsValid() )
	{
		// Finally, focus and sync the browser that was found
		if (bFocusContentBrowser)
		{
			FocusContentBrowser(ContentBrowserToSync);
		}
		ContentBrowserToSync->SyncTo(ItemSelection);
	}
}

void FT1ContentBrowserSingleton::GetSelectedAssets(TArray<FAssetData>& SelectedAssets)
{
	if ( PrimaryContentBrowser.IsValid() )
	{
		PrimaryContentBrowser.Pin()->GetSelectedAssets(SelectedAssets);
	}
}

void FT1ContentBrowserSingleton::CaptureThumbnailFromViewport(FViewport* InViewport, TArray<FAssetData>& SelectedAssets)
{
	T1ContentBrowserUtils::CaptureThumbnailFromViewport(InViewport, SelectedAssets);
}


void FT1ContentBrowserSingleton::OnEditorLoadSelectedAssetsIfNeeded()
{
	if ( PrimaryContentBrowser.IsValid() )
	{
		PrimaryContentBrowser.Pin()->LoadSelectedObjectsIfNeeded();
	}
}

FT1ContentBrowserSingleton& FT1ContentBrowserSingleton::Get()
{
	static const FName ModuleName = "T1ContentBrowserEditor";
	T1ContentBrowserEditor& Module = FModuleManager::GetModuleChecked<T1ContentBrowserEditor>(ModuleName);
	return static_cast<FT1ContentBrowserSingleton&>(Module.Get());
}

void FT1ContentBrowserSingleton::SetPrimaryContentBrowser(const TSharedRef<ST1ContentBrowser>& NewPrimaryBrowser)
{
	if ( PrimaryContentBrowser.IsValid() && PrimaryContentBrowser.Pin().ToSharedRef() == NewPrimaryBrowser )
	{
		// This is already the primary content browser
		return;
	}

	if ( PrimaryContentBrowser.IsValid() )
	{
		PrimaryContentBrowser.Pin()->SetIsPrimaryContentBrowser(false);
	}

	PrimaryContentBrowser = NewPrimaryBrowser;
	NewPrimaryBrowser->SetIsPrimaryContentBrowser(true);
}

void FT1ContentBrowserSingleton::ContentBrowserClosed(const TSharedRef<ST1ContentBrowser>& ClosedBrowser)
{
	// Find the browser in the all browsers list
	for (int32 BrowserIdx = AllContentBrowsers.Num() - 1; BrowserIdx >= 0; --BrowserIdx)
	{
		if ( !AllContentBrowsers[BrowserIdx].IsValid() || AllContentBrowsers[BrowserIdx].Pin() == ClosedBrowser )
		{
			AllContentBrowsers.RemoveAt(BrowserIdx);
		}
	}

	if ( !PrimaryContentBrowser.IsValid() || ClosedBrowser == PrimaryContentBrowser.Pin() )
	{
		ChooseNewPrimaryBrowser();
	}

	BrowserToLastKnownTabManagerMap.Add(ClosedBrowser->GetInstanceName(), ClosedBrowser->GetTabManager());
}

TSharedRef<FT1NativeClassHierarchy> FT1ContentBrowserSingleton::GetNativeClassHierarchy()
{
	if(!NativeClassHierarchy.IsValid())
	{
		NativeClassHierarchy = MakeShareable(new FT1NativeClassHierarchy());
	}
	return NativeClassHierarchy.ToSharedRef();
}

TSharedRef<FT1EmptyFolderVisibilityManager> FT1ContentBrowserSingleton::GetEmptyFolderVisibilityManager()
{
	return EmptyFolderVisibilityManager;
}

void FT1ContentBrowserSingleton::SharedCreateAssetDialogWindow(const TSharedRef<ST1AssetDialog>& AssetDialog, const FT1SharedAssetDialogConfig& InConfig, bool bModal) const
{
	const FVector2D DefaultWindowSize(1152.0f, 648.0f);
	const FVector2D WindowSize = InConfig.WindowSizeOverride.IsZero() ? DefaultWindowSize : InConfig.WindowSizeOverride;
	const FText WindowTitle = InConfig.DialogTitleOverride.IsEmpty() ? LOCTEXT("GenericAssetDialogWindowHeader", "Asset Dialog") : InConfig.DialogTitleOverride;

	TSharedRef<SWindow> DialogWindow =
		SNew(SWindow)
		.Title(WindowTitle)
		.ClientSize(WindowSize);

	DialogWindow->SetContent(AssetDialog);

	IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	const TSharedPtr<SWindow>& MainFrameParentWindow = MainFrameModule.GetParentWindow();
	if (MainFrameParentWindow.IsValid())
	{
		if (bModal)
		{
			FSlateApplication::Get().AddModalWindow(DialogWindow, MainFrameParentWindow.ToSharedRef());
		}
		else if (FGlobalTabmanager::Get()->GetRootWindow().IsValid())
		{
			FSlateApplication::Get().AddWindowAsNativeChild(DialogWindow, MainFrameParentWindow.ToSharedRef());
		}
		else
		{
			FSlateApplication::Get().AddWindow(DialogWindow);
		}
	}
	else
	{
		if (ensureMsgf(!bModal, TEXT("Could not create asset dialog because modal windows must have a parent and this was called at a time where the mainframe window does not exist.")))
		{
			FSlateApplication::Get().AddWindow(DialogWindow);
		}
	}
}

void FT1ContentBrowserSingleton::ChooseNewPrimaryBrowser()
{
	// Find the first valid browser and trim any invalid browsers along the way
	for (int32 BrowserIdx = 0; BrowserIdx < AllContentBrowsers.Num(); ++BrowserIdx)
	{
		if ( AllContentBrowsers[BrowserIdx].IsValid() )
		{
			SetPrimaryContentBrowser(AllContentBrowsers[BrowserIdx].Pin().ToSharedRef());
			break;
		}
		else
		{
			// Trim any invalid content browsers
			AllContentBrowsers.RemoveAt(BrowserIdx);
			BrowserIdx--;
		}
	}
}

void FT1ContentBrowserSingleton::FocusContentBrowser(const TSharedPtr<ST1ContentBrowser>& BrowserToFocus)
{
	if ( BrowserToFocus.IsValid() )
	{
		TSharedRef<ST1ContentBrowser> Browser = BrowserToFocus.ToSharedRef();
		TSharedPtr<FTabManager> TabManager = Browser->GetTabManager();
		if ( TabManager.IsValid() )
		{
			TabManager->InvokeTab(Browser->GetInstanceName());
		}
	}
}

void FT1ContentBrowserSingleton::SummonNewBrowser(bool bAllowLockedBrowsers)
{
	TSet<FName> OpenBrowserIDs;

	// Find all currently open browsers to help find the first open slot
	for (int32 BrowserIdx = AllContentBrowsers.Num() - 1; BrowserIdx >= 0; --BrowserIdx)
	{
		const TWeakPtr<ST1ContentBrowser>& Browser = AllContentBrowsers[BrowserIdx];
		if ( Browser.IsValid() )
		{
			OpenBrowserIDs.Add(Browser.Pin()->GetInstanceName());
		}
	}
	
	FName NewTabName;
	for ( int32 BrowserIdx = 0; BrowserIdx < ARRAY_COUNT(ContentBrowserTabIDs); BrowserIdx++ )
	{
		FName TestTabID = ContentBrowserTabIDs[BrowserIdx];
		if ( !OpenBrowserIDs.Contains(TestTabID) && (bAllowLockedBrowsers || !IsLocked(TestTabID)) )
		{
			// Found the first index that is not currently open
			NewTabName = TestTabID;
			break;
		}
	}

	if ( NewTabName != NAME_None )
	{
		const TWeakPtr<FTabManager>& TabManagerToInvoke = BrowserToLastKnownTabManagerMap.FindRef(NewTabName);
		if ( TabManagerToInvoke.IsValid() )
		{
			TabManagerToInvoke.Pin()->InvokeTab(NewTabName);
		}
		else
		{
			FGlobalTabmanager::Get()->InvokeTab(NewTabName);
		}
	}
	else
	{
		// No available slots... don't summon anything
	}
}

TSharedRef<SWidget> FT1ContentBrowserSingleton::CreateContentBrowser( const FName InstanceName, TSharedPtr<SDockTab> ContainingTab, const FT1ContentBrowserConfig* ContentBrowserConfig )
{
	TSharedRef<ST1ContentBrowser> NewBrowser =
		SNew(ST1ContentBrowser, InstanceName, ContentBrowserConfig )
		.IsEnabled( FSlateApplication::Get().GetNormalExecutionAttribute() )
		.ContainingTab( ContainingTab );

	AllContentBrowsers.Add( NewBrowser );

	if( !PrimaryContentBrowser.IsValid() )
	{
		ChooseNewPrimaryBrowser();
	}

	return NewBrowser;
}


TSharedRef<SDockTab> FT1ContentBrowserSingleton::SpawnContentBrowserTab( const FSpawnTabArgs& SpawnTabArgs, int32 BrowserIdx )
{	
	TAttribute<FText> Label = TAttribute<FText>::Create( TAttribute<FText>::FGetter::CreateRaw( this, &FT1ContentBrowserSingleton::GetContentBrowserTabLabel, BrowserIdx ) );

	TSharedRef<SDockTab> NewTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.Label( Label )
		.ToolTip( IDocumentation::Get()->CreateToolTip( Label, nullptr, "Shared/ContentBrowser", "Tab" ) );

	TSharedRef<SWidget> NewBrowser = CreateContentBrowser( SpawnTabArgs.GetTabId().TabType, NewTab, nullptr );

	if ( !PrimaryContentBrowser.IsValid() )
	{
		ChooseNewPrimaryBrowser();
	}

	// Add wrapper for tutorial highlighting
	TSharedRef<SBox> Wrapper =
		SNew(SBox)
		.AddMetaData<FTutorialMetaData>(FTutorialMetaData(TEXT("T1ContentBrowser"), TEXT("ContentBrowserTab1")))
		[
			NewBrowser
		];

	NewTab->SetContent( Wrapper );

	return NewTab;
}

bool FT1ContentBrowserSingleton::IsLocked(const FName& InstanceName) const
{
	// First try all the open browsers, as their locked state might be newer than the configs
	for (int32 AllBrowsersIdx = AllContentBrowsers.Num() - 1; AllBrowsersIdx >= 0; --AllBrowsersIdx)
	{
		const TWeakPtr<ST1ContentBrowser>& Browser = AllContentBrowsers[AllBrowsersIdx];
		if ( Browser.IsValid() && Browser.Pin()->GetInstanceName() == InstanceName )
		{
			return Browser.Pin()->IsLocked();
		}
	}

	// Fallback to getting the locked state from the config instead
	bool bIsLocked = false;
	GConfig->GetBool(*ST1ContentBrowser::SettingsIniSection, *(InstanceName.ToString() + TEXT(".Locked")), bIsLocked, GEditorPerProjectIni);
	return bIsLocked;
}

FText FT1ContentBrowserSingleton::GetContentBrowserLabelWithIndex( int32 BrowserIdx )
{
	return FText::Format(LOCTEXT("ContentBrowserTabNameWithIndex", "Content Browser {0}"), FText::AsNumber(BrowserIdx + 1));
}

FText FT1ContentBrowserSingleton::GetContentBrowserTabLabel(int32 BrowserIdx)
{
	int32 NumOpenContentBrowsers = 0;
	for (int32 AllBrowsersIdx = AllContentBrowsers.Num() - 1; AllBrowsersIdx >= 0; --AllBrowsersIdx)
	{
		const TWeakPtr<ST1ContentBrowser>& Browser = AllContentBrowsers[AllBrowsersIdx];
		if ( Browser.IsValid() )
		{
			NumOpenContentBrowsers++;
		}
		else
		{
			AllContentBrowsers.RemoveAt(AllBrowsersIdx);
		}
	}

	if ( NumOpenContentBrowsers > 1 )
	{
		return GetContentBrowserLabelWithIndex( BrowserIdx );
	}
	else
	{
		return LOCTEXT("ContentBrowserTabName", "Content Browser");
	}
}

void FT1ContentBrowserSingleton::SetSelectedPaths(const TArray<FString>& FolderPaths, bool bNeedsRefresh/* = false*/)
{
	// Make sure we have a valid browser
	if (!PrimaryContentBrowser.IsValid())
	{
		ChooseNewPrimaryBrowser();

		if (!PrimaryContentBrowser.IsValid())
		{
			SummonNewBrowser();
		}
	}

	if (PrimaryContentBrowser.IsValid())
	{
		PrimaryContentBrowser.Pin()->SetSelectedPaths(FolderPaths, bNeedsRefresh);
	}
}

void FT1ContentBrowserSingleton::ForceShowPluginContent(bool bEnginePlugin)
{
	if (!PrimaryContentBrowser.IsValid())
	{
		ChooseNewPrimaryBrowser();

		if (!PrimaryContentBrowser.IsValid())
		{
			SummonNewBrowser();
		}
	}

	if (PrimaryContentBrowser.IsValid())
	{
		PrimaryContentBrowser.Pin()->ForceShowPluginContent(bEnginePlugin);
	}
}

#undef LOCTEXT_NAMESPACE
