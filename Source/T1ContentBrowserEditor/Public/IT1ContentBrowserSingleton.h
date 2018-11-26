// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SlateFwd.h"
#include "Misc/Attribute.h"
#include "AssetData.h"
#include "Developer/AssetTools/Public/AssetTypeCategories.h"
#include "ARFilter.h"
#include "T1ContentBrowserDelegates.h"
#include "Developer/CollectionManager/Public/CollectionManagerTypes.h"
#include "Misc/FilterCollection.h"
#include "Framework/Views/ITypedTableView.h"
#include "AssetThumbnail.h"

class FViewport;
class UFactory;

typedef const FAssetData& FT1AssetFilterType;
typedef TFilterCollection<FT1AssetFilterType> FT1AssetFilterCollectionType;

class UFactory;

/** The view modes used in SAssetView */
namespace ET1AssetViewType
{
	enum Type
	{
		List,
		Tile,
		Column,

		MAX
	};
}


/** A selection of items in the Content Browser */
struct FT1ContentBrowserSelection
{
	TArray<FAssetData> SelectedAssets;
	TArray<FString> SelectedFolders;

	int32 Num() const
	{
		return SelectedAssets.Num() + SelectedFolders.Num();
	}

	void Reset()
	{
		SelectedAssets.Reset();
		SelectedFolders.Reset();
	}

	void Empty()
	{
		SelectedAssets.Empty();
		SelectedFolders.Empty();
	}
};


/** A struct containing details about how the content browser should behave */
struct FT1ContentBrowserConfig
{
	/** The contents of the label on the thumbnail */
	EThumbnailLabel::Type ThumbnailLabel;

	/** The default scale for thumbnails. [0-1] range */
	TAttribute< float > ThumbnailScale;

	/** The default view mode */
	ET1AssetViewType::Type InitialAssetViewType;

	/** If true, show the bottom toolbar which shows # of assets selected, view mode buttons, etc... */
	bool bShowBottomToolbar;

	/** Indicates if this view is allowed to show classes */
	bool bCanShowClasses;

	/** Whether the sources view for choosing folders/collections is available or not */
	bool bUseSourcesView;

	/** Whether the sources view should initially be expanded or not */
	bool bExpandSourcesView;

	/** Whether asset paths are shown in the Content Browser.  Only useful if you only want to show collections */
	bool bShowAssetPathTree;

	/** Forces collections to be initially visible, regardless of defaults */
	bool bAlwaysShowCollections;

	/** Collection to view initially */
	FCollectionNameType SelectedCollectionName;

	/** Whether the path picker is available or not */
	bool bUsePathPicker;

	/** Whether to show filters */
	bool bCanShowFilters;

	/** Whether to show asset search */
	bool bCanShowAssetSearch;

	/** Indicates if the 'Show folders' option should be enabled or disabled */
	bool bCanShowFolders;

	/** Indicates if the 'Real-Time Thumbnails' option should be enabled or disabled */
	bool bCanShowRealTimeThumbnails;

	/** Indicates if the 'Show Developers' option should be enabled or disabled */
	bool bCanShowDevelopersFolder;

	/** Whether the 'lock' button is visible on the toolbar */
	bool bCanShowLockButton;

	FT1ContentBrowserConfig()
		: ThumbnailLabel( EThumbnailLabel::ClassName )
		, ThumbnailScale(0.1f)
		, InitialAssetViewType(ET1AssetViewType::Tile)
		, bShowBottomToolbar(true)
		, bCanShowClasses(true)
		, bUseSourcesView(true)
		, bExpandSourcesView(true)
		, bShowAssetPathTree(true)
		, bAlwaysShowCollections(false)
		, SelectedCollectionName( NAME_None, ECollectionShareType::CST_Local )
		, bUsePathPicker(true)
		, bCanShowFilters(true)
		, bCanShowAssetSearch(true)
		, bCanShowFolders(true)
		, bCanShowRealTimeThumbnails(true)
		, bCanShowDevelopersFolder(true)
		, bCanShowLockButton(true)
	{ }
};


/** A struct containing details about how the asset picker should behave */
struct FT1AssetPickerConfig
{
	/** The selection mode the picker should use */
	ESelectionMode::Type SelectionMode;

	/** An array of pointers to existing delegates which the AssetView will register a function which returns the current selection */
	TArray<FT1GetCurrentSelectionDelegate*> GetCurrentSelectionDelegates;

	/** An array of pointers to existing delegates which the AssetView will register a function which sync the asset list*/
	TArray<FT1SyncToAssetsDelegate*> SyncToAssetsDelegates;

	/** A pointer to an existing delegate that, when executed, will set the filter an the asset picker after it is created. */
	TArray<FT1SetARFilterDelegate*> SetFilterDelegates;

	/** A pointer to an existing delegate that, when executed, will refresh the asset view. */
	TArray<FT1RefreshAssetViewDelegate*> RefreshAssetViewDelegates;

	/** The asset registry filter to use to cull results */
	FARFilter Filter;

	/** Custom front end filters to be displayed */
	TArray< TSharedRef<class FT1FrontendFilter> > ExtraFrontendFilters;

	/** The names of columns to hide by default in the column view */
	TArray<FString> HiddenColumnNames;

	/** List of custom columns that fill out data with a callback */
	TArray<FT1AssetViewCustomColumn> CustomColumns;

	/** The contents of the label on the thumbnail */
	EThumbnailLabel::Type ThumbnailLabel;

	/** The default scale for thumbnails. [0-1] range */
	TAttribute< float > ThumbnailScale;

	/** Only display results in these collections */
	TArray<FCollectionNameType> Collections;

	/** The asset that should be initially selected */
	FAssetData InitialAssetSelection;

	/** The delegate that fires when an asset was selected */
	FT1OnAssetSelected OnAssetSelected;

	/** The delegate that fires when a folder was double clicked */
	FT1OnPathSelected OnFolderEntered;

	/** The delegate that fires when an asset is double clicked */
	FT1OnAssetDoubleClicked OnAssetDoubleClicked;

	/** The delegate that fires when an asset has enter pressed while selected */
	FT1OnAssetEnterPressed OnAssetEnterPressed;

	/** The delegate that fires when any number of assets are activated */
	FT1OnAssetsActivated OnAssetsActivated;

	/** The delegate that fires when an asset is right clicked and a context menu is requested */
	FT1OnGetAssetContextMenu OnGetAssetContextMenu;

	/** The delegate that fires when a folder is right clicked and a context menu is requested */
	FT1OnGetFolderContextMenu OnGetFolderContextMenu;

	/** Called to see if it is valid to get a custom asset tool tip */
	FT1OnIsAssetValidForCustomToolTip OnIsAssetValidForCustomToolTip;

	/** Fired when an asset item is constructed and a tooltip is requested. If unbound the item will use the default widget */
	FT1OnGetCustomAssetToolTip OnGetCustomAssetToolTip;

	/** Called to add extra asset data to the asset view, to display virtual assets. These get treated similar to Class assets */
	FT1OnGetCustomSourceAssets OnGetCustomSourceAssets;

	/** Fired when an asset item is about to show its tool tip */
	FT1OnVisualizeAssetToolTip OnVisualizeAssetToolTip;

	/** Fired when an asset item's tooltip is closing */
	FT1OnAssetToolTipClosing OnAssetToolTipClosing;

	/** If more detailed filtering is required than simply Filter, this delegate will get fired for every asset to determine if it should be culled. */
	FT1OnShouldFilterAsset OnShouldFilterAsset;

	/** This delegate will be called in Details view when a new asset registry searchable tag is encountered, to
	    determine if it should be displayed or not.  If it returns true or isn't bound, the tag will be displayed normally. */
	FT1OnShouldDisplayAssetTag OnAssetTagWantsToBeDisplayed;

	/** The default view mode */
	ET1AssetViewType::Type InitialAssetViewType;

	/** The text to show when there are no assets to show */
	TAttribute< FText > AssetShowWarningText;

	/** If set, view settings will be saved and loaded for the asset view using this name in ini files */
	FString SaveSettingsName;

	/** If true, the search box will gain focus when the asset picker is created */
	bool bFocusSearchBoxWhenOpened;

	/** If true, a "None" item will always appear at the top of the list */
	bool bAllowNullSelection;

	/** If true, show the bottom toolbar which shows # of assets selected, view mode buttons, etc... */
	bool bShowBottomToolbar;

	/** If false, auto-hide the search bar above */
	bool bAutohideSearchBar;

	/** Whether to allow dragging of items */
	bool bAllowDragging;

	/** Indicates if this view is allowed to show classes */
	bool bCanShowClasses;

	/** Indicates if the 'Show folders' option should be enabled or disabled */
	bool bCanShowFolders;

	/** Indicates if the 'Real-Time Thumbnails' option should be enabled or disabled */
	bool bCanShowRealTimeThumbnails;

	/** Indicates if the 'Show Developers' option should be enabled or disabled */
	bool bCanShowDevelopersFolder;

	/** Indicates if the context menu is going to load the assets, and if so to preload before the context menu is shown, and warn about the pending load. */
	bool bPreloadAssetsForContextMenu;

	/** Indicates that we would like to build the filter UI with the Asset Picker */
	bool bAddFilterUI;

	/** If true, show path in column view */
	bool bShowPathInColumnView; 
	/** If true, show class in column view */
	bool bShowTypeInColumnView;
	/** If true, sort by path in column view. Only works if initial view type is Column */
	bool bSortByPathInColumnView;

	/** Override the default filter context menu layout */
	EAssetTypeCategories::Type DefaultFilterMenuExpansion;

	FT1AssetPickerConfig()
		: SelectionMode( ESelectionMode::Multi )
		, ThumbnailLabel( EThumbnailLabel::ClassName )
		, ThumbnailScale(0.1f)
		, InitialAssetViewType(ET1AssetViewType::Tile)
		, bFocusSearchBoxWhenOpened(true)
		, bAllowNullSelection(false)
		, bShowBottomToolbar(true)
		, bAutohideSearchBar(false)
		, bAllowDragging(true)
		, bCanShowClasses(true)
		, bCanShowFolders(false)
		, bCanShowRealTimeThumbnails(false)
		, bCanShowDevelopersFolder(true)
		, bPreloadAssetsForContextMenu(true)
		, bAddFilterUI(false)
		, bShowPathInColumnView(false)
		, bShowTypeInColumnView(true)
		, bSortByPathInColumnView(false)
		, DefaultFilterMenuExpansion(EAssetTypeCategories::Basic)
	{}
};

/** A struct containing details about how the path picker should behave */
struct FT1PathPickerConfig
{
	/** The initial path to select. Leave empty to skip initial selection. */
	FString DefaultPath;

	/** The delegate that fires when a path was selected */
	FT1OnPathSelected OnPathSelected;

	/** The delegate that fires when a path is right clicked and a context menu is requested */
	FT1ContentBrowserMenuExtender_SelectedPaths OnGetPathContextMenuExtender;

	/** The delegate that fires when a folder is right clicked and a context menu is requested */
	FT1OnGetFolderContextMenu OnGetFolderContextMenu;

	/** A pointer to an existing delegate that, when executed, will set the paths for the path picker after it is created. */
	TArray<FT1SetPathPickerPathsDelegate*> SetPathsDelegates;

	/** If true, the search box will gain focus when the path picker is created */
	bool bFocusSearchBoxWhenOpened;

	/** If false, the context menu will not open when an item is right clicked */
	bool bAllowContextMenu;

	/** If true, will allow class folders to be shown in the picker */
	bool bAllowClassesFolder;

	/** If true, will add the path specified in DefaultPath to the tree if it doesn't exist already */
	bool bAddDefaultPath;

	FT1PathPickerConfig()
		: bFocusSearchBoxWhenOpened(true)
		, bAllowContextMenu(true)
		, bAllowClassesFolder(false)
		, bAddDefaultPath(false)
	{}
};

/** A struct containing details about how the collection picker should behave */
struct FT1CollectionPickerConfig
{
	/** If true, collection buttons will be displayed */
	bool AllowCollectionButtons;

	/** If true, users will be able to access the right-click menu of a collection */
	bool AllowRightClickMenu;

	/** Called when a collection was selected */
	FT1OnCollectionSelected OnCollectionSelected;

	FT1CollectionPickerConfig()
		: AllowCollectionButtons(true)
		, AllowRightClickMenu(true)
		, OnCollectionSelected()
	{}
};

namespace ET1AssetDialogType
{
	enum Type
	{
		Open,
		Save
	};
}

/** A struct containing shared details about how asset dialogs should behave. You should not instanciate this config, but use FOpenAssetDialogConfig or FSaveAssetDialogConfig instead. */
struct FT1SharedAssetDialogConfig
{
	FText DialogTitleOverride;
	FString DefaultPath;
	TArray<FName> AssetClassNames;
	FVector2D WindowSizeOverride;

	virtual ET1AssetDialogType::Type GetDialogType() const = 0;

	FT1SharedAssetDialogConfig()
		: WindowSizeOverride(EForceInit::ForceInitToZero)
	{}

	virtual ~FT1SharedAssetDialogConfig()
	{}
};

/** A struct containing details about how the open asset dialog should behave. */
struct FT1OpenAssetDialogConfig : public FT1SharedAssetDialogConfig
{
	bool bAllowMultipleSelection;

	virtual ET1AssetDialogType::Type GetDialogType() const override { return ET1AssetDialogType::Open; }

	FT1OpenAssetDialogConfig()
		: FT1SharedAssetDialogConfig()
		, bAllowMultipleSelection(false)
	{}
};

/** An enum to choose the behavior of the save asset dialog when the user chooses an asset that already exists */
namespace ET1SaveAssetDialogExistingAssetPolicy
{
	enum Type
	{
		/** Display an error and disallow the save */
		Disallow,

		/** Allow the save, but warn that the existing file will be overwritten */
		AllowButWarn
	};
}

/** A struct containing details about how the save asset dialog should behave. */
struct FT1SaveAssetDialogConfig : public FT1SharedAssetDialogConfig
{
	FString DefaultAssetName;
	ET1SaveAssetDialogExistingAssetPolicy::Type ExistingAssetPolicy;

	virtual ET1AssetDialogType::Type GetDialogType() const override { return ET1AssetDialogType::Save; }

	FT1SaveAssetDialogConfig()
		: FT1SharedAssetDialogConfig()
		, ExistingAssetPolicy(ET1SaveAssetDialogExistingAssetPolicy::Disallow)
	{}
};

/**
 * Content browser module singleton
 */
class IT1ContentBrowserSingleton
{
public:
	/** Virtual destructor */
	virtual ~IT1ContentBrowserSingleton() {}

	/**
	 * Generates a content browser.  Generally you should not call this function, but instead use CreateAssetPicker().
	 *
	 * @param	InstanceName			Global name of this content browser instance
	 * @param	ContainingTab			The tab the browser is contained within or nullptr
	 * @param	ContentBrowserConfig	Initial defaults for the new content browser, or nullptr to use saved settings
	 *
	 * @return The newly created content browser widget
	 */
	virtual TSharedRef<class SWidget> CreateContentBrowser( const FName InstanceName, TSharedPtr<SDockTab> ContainingTab, const FT1ContentBrowserConfig* ContentBrowserConfig ) = 0;

	/**
	 * Generates an asset picker widget locked to the specified FARFilter.
	 *
	 * @param AssetPickerConfig		A struct containing details about how the asset picker should behave				
	 * @return The asset picker widget
	 */
	virtual TSharedRef<class SWidget> CreateAssetPicker(const FT1AssetPickerConfig& AssetPickerConfig) = 0;

	/**
	 * Generates a path picker widget.
	 *
	 * @param PathPickerConfig		A struct containing details about how the path picker should behave				
	 * @return The path picker widget
	 */
	virtual TSharedRef<class SWidget> CreatePathPicker(const FT1PathPickerConfig& PathPickerConfig) = 0;

	/**
	 * Generates a collection picker widget.
	 *
	 * @param CollectionPickerConfig		A struct containing details about how the collection picker should behave				
	 * @return The collection picker widget
	 */
	virtual TSharedRef<class SWidget> CreateCollectionPicker(const FT1CollectionPickerConfig& CollectionPickerConfig) = 0;

	/**
	 * Opens the Open Asset dialog in a non-modal window
	 *
	 * @param OpenAssetConfig				A struct containing details about how the open asset dialog should behave
	 * @param OnAssetsChosenForOpen			A delegate that is fired when assets are chosen and the open button is pressed
	 * @param OnAssetDialogCancelled		A delegate that is fired when the asset dialog is closed or cancelled
	 */
	virtual void CreateOpenAssetDialog(const FT1OpenAssetDialogConfig& OpenAssetConfig, const FT1OnAssetsChosenForOpen& OnAssetsChosenForOpen, const FT1OnAssetDialogCancelled& OnAssetDialogCancelled) = 0;

	/**
	 * Opens the Open Asset dialog in a modal window
	 *
	 * @param OpenAssetConfig				A struct containing details about how the open asset dialog should behave
	 * @return The assets that were chosen to be opened
	 */
	virtual TArray<FAssetData> CreateModalOpenAssetDialog(const FT1OpenAssetDialogConfig& InConfig) = 0;

	/**
	 * Opens the Save Asset dialog in a non-modal window
	 *
	 * @param SaveAssetConfig				A struct containing details about how the save asset dialog should behave
	 * @param OnAssetNameChosenForSave		A delegate that is fired when an object path is chosen and the save button is pressed
	 * @param OnAssetDialogCancelled		A delegate that is fired when the asset dialog is closed or cancelled
	 */
	virtual void CreateSaveAssetDialog(const FT1SaveAssetDialogConfig& SaveAssetConfig, const FT1OnObjectPathChosenForSave& OnAssetNameChosenForSave, const FT1OnAssetDialogCancelled& OnAssetDialogCancelled) = 0;

	/**
	 * Opens the Save Asset dialog in a modal window
	 *
	 * @param SaveAssetConfig				A struct containing details about how the save asset dialog should behave
	 * @return The object path that was chosen
	 */
	virtual FString CreateModalSaveAssetDialog(const FT1SaveAssetDialogConfig& SaveAssetConfig) = 0;

	/** Returns true if there is at least one browser open that is eligible to be a primary content browser */
	virtual bool HasPrimaryContentBrowser() const = 0;

	/** Brings the primary content browser to the front or opens one if it does not exist. */
	virtual void FocusPrimaryContentBrowser(bool bFocusSearch) = 0;

	/** Sets up an inline-name for the creation of a new asset in the primary content browser using the specified path and the specified class and/or factory */
	virtual void CreateNewAsset(const FString& DefaultAssetName, const FString& PackagePath, UClass* AssetClass, UFactory* Factory) = 0;

	/** Selects the supplied assets in all content browsers. If bAllowLockedBrowsers is true, even locked browsers may handle the sync. Only set to true if the sync doesn't seem external to the content browser. */
	virtual void SyncBrowserToAssets(const TArray<struct FAssetData>& AssetDataList, bool bAllowLockedBrowsers = false, bool bFocusContentBrowser = true) = 0;
	virtual void SyncBrowserToAssets(const TArray<UObject*>& AssetList, bool bAllowLockedBrowsers = false, bool bFocusContentBrowser = true) = 0;

	/** Selects the supplied folders in all content browsers. If bAllowLockedBrowsers is true, even locked browsers may handle the sync. Only set to true if the sync doesn't seem external to the content browser. */
	virtual void SyncBrowserToFolders(const TArray<FString>& FolderList, bool bAllowLockedBrowsers = false, bool bFocusContentBrowser = true) = 0;

	/** Selects the supplied items in all content browsers. If bAllowLockedBrowsers is true, even locked browsers may handle the sync. Only set to true if the sync doesn't seem external to the content browser. */
	virtual void SyncBrowserTo(const FT1ContentBrowserSelection& ItemSelection, bool bAllowLockedBrowsers = false, bool bFocusContentBrowser = true) = 0;

	/** Generates a list of assets that are selected in the primary content browser */
	virtual void GetSelectedAssets(TArray<FAssetData>& SelectedAssets) = 0;

	/**
	 * Capture active viewport to thumbnail and assigns that thumbnail to incoming assets
	 *
	 * @param InViewport - viewport to sample from
	 * @param InAssetsToAssign - assets that should receive the new thumbnail ONLY if they are assets that use GenericThumbnails
	 */
	virtual void CaptureThumbnailFromViewport(FViewport* InViewport, TArray<FAssetData>& SelectedAssets) = 0;

	/**
	 * Sets the content browser to display the selected paths
	 */
	virtual void SetSelectedPaths(const TArray<FString>& FolderPaths, bool bNeedsRefresh = false) = 0;

	/**
	* Forces the content browser to show plugin content if it's not already showing.
	*
	* @param bEnginePlugin	If this is true, it will also force the content browser to show engine content
	*/
	virtual void ForceShowPluginContent(bool bEnginePlugin) = 0;
};
