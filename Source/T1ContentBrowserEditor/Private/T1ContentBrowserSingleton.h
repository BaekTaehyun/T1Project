// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "AssetData.h"
#include "T1ContentBrowserEditor/Public/IT1ContentBrowserSingleton.h"

class FT1CollectionAssetRegistryBridge;
class FT1EmptyFolderVisibilityManager;
class FT1NativeClassHierarchy;
class FSpawnTabArgs;
class FTabManager;
class FViewport;
class ST1ContentBrowser;
class UFactory;

#define MAX_CONTENT_BROWSERS 4

/**
 * Content browser module singleton implementation class
 */
class FT1ContentBrowserSingleton : public IT1ContentBrowserSingleton
{
public:
	/** Constructor, Destructor */
	FT1ContentBrowserSingleton();
	virtual ~FT1ContentBrowserSingleton();

	// IT1ContentBrowserSingleton interface
	virtual TSharedRef<class SWidget> CreateContentBrowser( const FName InstanceName, TSharedPtr<SDockTab> ContainingTab, const FT1ContentBrowserConfig* ContentBrowserConfig ) override;
	virtual TSharedRef<class SWidget> CreateAssetPicker(const FT1AssetPickerConfig& AssetPickerConfig) override;
	virtual TSharedRef<class SWidget> CreatePathPicker(const FT1PathPickerConfig& PathPickerConfig) override;
	virtual TSharedRef<class SWidget> CreateCollectionPicker(const FT1CollectionPickerConfig& CollectionPickerConfig) override;
	virtual void CreateOpenAssetDialog(const FT1OpenAssetDialogConfig& OpenAssetConfig, const FT1OnAssetsChosenForOpen& OnAssetsChosenForOpen, const FT1OnAssetDialogCancelled& OnAssetDialogCancelled) override;
	virtual TArray<FAssetData> CreateModalOpenAssetDialog(const FT1OpenAssetDialogConfig& InConfig) override;
	virtual void CreateSaveAssetDialog(const FT1SaveAssetDialogConfig& SaveAssetConfig, const FT1OnObjectPathChosenForSave& OnAssetNameChosenForSave, const FT1OnAssetDialogCancelled& OnAssetDialogCancelled) override;
	virtual FString CreateModalSaveAssetDialog(const FT1SaveAssetDialogConfig& SaveAssetConfig) override;
	virtual bool HasPrimaryContentBrowser() const override;
	virtual void FocusPrimaryContentBrowser(bool bFocusSearch) override;
	virtual void CreateNewAsset(const FString& DefaultAssetName, const FString& PackagePath, UClass* AssetClass, UFactory* Factory) override;
	virtual void SyncBrowserToAssets(const TArray<struct FAssetData>& AssetDataList, bool bAllowLockedBrowsers = false, bool bFocusContentBrowser = true) override;
	virtual void SyncBrowserToAssets(const TArray<UObject*>& AssetList, bool bAllowLockedBrowsers = false, bool bFocusContentBrowser = true) override;
	virtual void SyncBrowserToFolders(const TArray<FString>& FolderList, bool bAllowLockedBrowsers = false, bool bFocusContentBrowser = true) override;
	virtual void SyncBrowserTo(const FT1ContentBrowserSelection& ItemSelection, bool bAllowLockedBrowsers = false, bool bFocusContentBrowser = true) override;
	virtual void GetSelectedAssets(TArray<FAssetData>& SelectedAssets) override;
	virtual void CaptureThumbnailFromViewport(FViewport* InViewport, TArray<FAssetData>& SelectedAssets) override;
	virtual void SetSelectedPaths(const TArray<FString>& FolderPaths, bool bNeedsRefresh = false) override;
	virtual void ForceShowPluginContent(bool bEnginePlugin) override;


	/** Gets the content browser singleton as a FT1ContentBrowserSingleton */
	static FT1ContentBrowserSingleton& Get();
	
	/** Sets the current primary content browser. */
	void SetPrimaryContentBrowser(const TSharedRef<ST1ContentBrowser>& NewPrimaryBrowser);

	/** Notifies the singleton that a browser was closed */
	void ContentBrowserClosed(const TSharedRef<ST1ContentBrowser>& ClosedBrowser);

	TSharedRef<FT1NativeClassHierarchy> GetNativeClassHierarchy();

	TSharedRef<FT1EmptyFolderVisibilityManager> GetEmptyFolderVisibilityManager();

	/** Single storage location for content browser favorites */
	TArray<FString> FavoriteFolderPaths;

private:

	/** Util to get or create the content browser that should be used by the various Sync functions */
	TSharedPtr<ST1ContentBrowser> FindContentBrowserToSync(bool bAllowLockedBrowsers);

	/** Shared code to open an asset dialog window with a config */
	void SharedCreateAssetDialogWindow(const TSharedRef<class ST1AssetDialog>& AssetDialog, const FT1SharedAssetDialogConfig& InConfig, bool bModal) const;

	/** 
	 * Delegate handlers
	 **/
	void OnEditorLoadSelectedAssetsIfNeeded();

	/** Sets the primary content browser to the next valid browser in the list of all browsers */
	void ChooseNewPrimaryBrowser();

	/** Gives focus to the specified content browser */
	void FocusContentBrowser(const TSharedPtr<ST1ContentBrowser>& BrowserToFocus);

	/** Summons a new content browser */
	void SummonNewBrowser(bool bAllowLockedBrowsers = false);

	/** Handler for a request to spawn a new content browser tab */
	TSharedRef<SDockTab> SpawnContentBrowserTab( const FSpawnTabArgs& SpawnTabArgs, int32 BrowserIdx );

	/** Handler for a request to spawn a new content browser tab */
	FText GetContentBrowserTabLabel(int32 BrowserIdx);

	/** Returns true if this content browser is locked (can be used even when closed) */
	bool IsLocked(const FName& InstanceName) const;

	/** Returns a localized name for the tab/menu entry with index */
	static FText GetContentBrowserLabelWithIndex( int32 BrowserIdx );

public:
	/** The tab identifier/instance name for content browser tabs */
	FName ContentBrowserTabIDs[MAX_CONTENT_BROWSERS];

private:
	TArray<TWeakPtr<ST1ContentBrowser>> AllContentBrowsers;

	TMap<FName, TWeakPtr<FTabManager>> BrowserToLastKnownTabManagerMap;

	TWeakPtr<ST1ContentBrowser> PrimaryContentBrowser;

	TSharedPtr<FT1NativeClassHierarchy> NativeClassHierarchy;

	TSharedRef<FT1EmptyFolderVisibilityManager> EmptyFolderVisibilityManager;

	TSharedRef<FT1CollectionAssetRegistryBridge> CollectionAssetRegistryBridge;

	/** An incrementing int32 which is used when making unique settings strings */
	int32 SettingsStringID;
};
