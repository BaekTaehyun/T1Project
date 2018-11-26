// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "AssetData.h"
#include "Input/Reply.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Widgets/SToolTip.h"
#include "Developer/AssetTools/Public/IAssetTypeActions.h"

struct FARFilter;
struct FCollectionNameType;
class FUICommandList;


 enum class ET1MovedContentFolderFlags : uint8
{
	None = 0,
	Favorite = 1 << 0,
};

ENUM_CLASS_FLAGS(ET1MovedContentFolderFlags)

struct FT1MovedContentFolder
{
	FString OldPath;
	FString NewPath;
	ET1MovedContentFolderFlags Flags;

	FT1MovedContentFolder(const FString& InOldPath, const FString& InNewPath);
};

/** Called when a "Find in Asset Tree" is requested */
DECLARE_DELEGATE_OneParam(FT1OnFindInAssetTreeRequested, const TArray<FAssetData>& /*AssetsToFind*/);

/** Called when the user has committed a rename of one or more assets */
DECLARE_DELEGATE_OneParam(FT1OnAssetRenameCommitted, const TArray<FAssetData>& /*Assets*/);

/** Called when a collection is selected in the collections view */
DECLARE_DELEGATE_OneParam( FT1OnCollectionSelected, const FCollectionNameType& /*SelectedCollection*/);

/** Called to retrieve the tooltip for the specified asset */
DECLARE_DELEGATE_RetVal_OneParam( TSharedRef< SToolTip >, FT1ConstructToolTipForAsset, const FAssetData& /*Asset*/);

/** Called to check if an asset should be filtered out by external code. Return true to exclude the asset from the view. */
DECLARE_DELEGATE_RetVal_OneParam(bool, FT1OnShouldFilterAsset, const FAssetData& /*AssetData*/);

/** Called to check if an asset tag should be display in details view. Return false to exclude the asset from the view. */
DECLARE_DELEGATE_RetVal_TwoParams(bool, FT1OnShouldDisplayAssetTag, FName /*AssetType*/, FName /*TagName*/);

/** Called to clear the selection of the specified assetdata or all selection if an invalid assetdata is passed */
DECLARE_DELEGATE( FT1ClearSelectionDelegate );

/** Called when thumbnail scale changes and the thumbnail scale is bound to a delegate */
DECLARE_DELEGATE_OneParam( FT1OnThumbnailScaleChanged, const float /*NewScale*/);

/** Called to retrieve an array of the currently selected asset data */
DECLARE_DELEGATE_RetVal( TArray< FAssetData >, FT1GetCurrentSelectionDelegate );

/** Called to retrieve an array of the currently selected asset data */
DECLARE_DELEGATE_OneParam(FT1SyncToAssetsDelegate, const TArray< FAssetData >& /*AssetData*/);

/** Called to force the asset view to refresh */
DECLARE_DELEGATE_OneParam(FT1RefreshAssetViewDelegate, bool /*UpdateSources*/);

/** Called to set a new filter for an existing asset picker */
DECLARE_DELEGATE_OneParam(FT1SetARFilterDelegate, const FARFilter& /*NewFilter*/);

/** A pointer to an existing delegate that, when executed, will set the filter an the asset picker after it is created. */
DECLARE_DELEGATE_OneParam(FT1SetPathPickerPathsDelegate, const TArray<FString>& /*NewPaths*/);

/** Called to adjust the selection from the current assetdata, should be +1 to increment or -1 to decrement */
DECLARE_DELEGATE_OneParam(FT1AdjustSelectionDelegate, const int32 /*direction*/ );

/** Called when an asset is selected in the asset view */
DECLARE_DELEGATE_OneParam(FT1OnAssetSelected, const FAssetData& /*AssetData*/);

/** Called when the user double clicks, presses enter, or presses space on an asset */
DECLARE_DELEGATE_TwoParams(FT1OnAssetsActivated, const TArray<FAssetData>& /*ActivatedAssets*/, EAssetTypeActivationMethod::Type /*ActivationMethod*/);

/** Called when an asset has begun being dragged by the user */
DECLARE_DELEGATE_RetVal_OneParam(FReply, FT1OnAssetDragged, const TArray<FAssetData>& /*AssetData*/);

/** Called when an asset is clicked on in the asset view */
DECLARE_DELEGATE_OneParam( FT1OnAssetClicked, const FAssetData& /*AssetData*/ );

/** Called when an asset is double clicked in the asset view */
DECLARE_DELEGATE_OneParam(FT1OnAssetDoubleClicked, const FAssetData& /*AssetData*/);

/** Called when enter is pressed on an asset in the asset view */
DECLARE_DELEGATE_OneParam(FT1OnAssetEnterPressed, const TArray<FAssetData>& /*SelectedAssets*/);

/** Called when a new folder is starting to be created */
DECLARE_DELEGATE_TwoParams(FT1OnCreateNewFolder, const FString& /*FolderName*/, const FString& /*FolderPath*/);

/** Called to request the menu when right clicking on an asset */
DECLARE_DELEGATE_RetVal_OneParam(TSharedPtr<SWidget>, FT1OnGetAssetContextMenu, const TArray<FAssetData>& /*SelectedAssets*/);

/** Called when a path is selected in the path picker */
DECLARE_DELEGATE_OneParam(FT1OnPathSelected, const FString& /*Path*/);

/** Called when a path is double clicked in the asset view */
DECLARE_DELEGATE_OneParam(FT1OnPathDoubleClicked, const FString& /*Path*/);

/** Called when registering a custom command/keybinding for the content browser */
DECLARE_DELEGATE_TwoParams(FT1OnContentBrowserGetSelection, TArray<FAssetData>& /*SelectedAssets*/, TArray<FString>& /*SelectedPaths*/);
DECLARE_DELEGATE_TwoParams(FT1ContentBrowserCommandExtender, TSharedRef<FUICommandList> /*CommandList*/, FT1OnContentBrowserGetSelection /*GetSelectionDelegate*/);

/** Called to request the menu when right clicking on a path */
DECLARE_DELEGATE_RetVal(TSharedRef<FExtender>, FT1ContentBrowserMenuExtender);
DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<FExtender>, FT1ContentBrowserMenuExtender_SelectedAssets, const TArray<FAssetData>& /*SelectedAssets*/);
DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<FExtender>, FT1ContentBrowserMenuExtender_SelectedPaths, const TArray<FString>& /*SelectedPaths*/);

/** Called to request the menu when right clicking on an asset */
DECLARE_DELEGATE_RetVal_ThreeParams(TSharedPtr<SWidget>, FT1OnGetFolderContextMenu, const TArray<FString>& /*SelectedPaths*/, FT1ContentBrowserMenuExtender_SelectedPaths /*MenuExtender*/, FT1OnCreateNewFolder /*CreationDelegate*/);

/** Called to see if it is valid to request a custom asset item tooltip */
DECLARE_DELEGATE_RetVal_OneParam(bool, FT1OnIsAssetValidForCustomToolTip, FAssetData& /*AssetData*/);

/** Called to request a custom asset item tooltip */
DECLARE_DELEGATE_RetVal_OneParam( TSharedRef<SToolTip>, FT1OnGetCustomAssetToolTip, FAssetData& /*AssetData*/);

/** Called to get string/text value for a custom column, will get converted as necessary */
DECLARE_DELEGATE_RetVal_TwoParams(FString, FT1OnGetCustomAssetColumnData, FAssetData& /*AssetData*/, FName /*ColumnName*/);
DECLARE_DELEGATE_RetVal_TwoParams(FText, FT1OnGetCustomAssetColumnDisplayText, FAssetData& /*AssetData*/, FName /*ColumnName*/);

/** Called to add extra asset data to the asset view, to display virtual assets. These get treated similar to Class assets */
DECLARE_DELEGATE_TwoParams(FT1OnGetCustomSourceAssets, const FARFilter& /*SourceFilter*/, TArray<FAssetData>& /*AddedAssets*/);

/** Called when an asset item visualizes its tooltip */
DECLARE_DELEGATE_RetVal_TwoParams(bool, FT1OnVisualizeAssetToolTip, const TSharedPtr<SWidget>& /*ToolTipContent*/, FAssetData& /*AssetData*/);

/** Called from the Asset Dialog when a non-modal dialog is closed or cancelled */
DECLARE_DELEGATE(FT1OnAssetDialogCancelled);

/** Called when an asset item's tooltip is closing */
DECLARE_DELEGATE( FT1OnAssetToolTipClosing );

/** Called from the Asset Dialog when assets are chosen in non-modal Open dialogs */
DECLARE_DELEGATE_OneParam(FT1OnAssetsChosenForOpen, const TArray<FAssetData>& /*SelectedAssets*/);

/** Called from the Asset Dialog when an asset name is chosen in non-modal Save dialogs */
DECLARE_DELEGATE_OneParam(FT1OnObjectPathChosenForSave, const FString& /*ObjectPath*/);

/** Called when a favorite folder is moved or renamed in the content browser */
DECLARE_DELEGATE_OneParam(FT1OnFolderPathChanged, const TArray<struct FT1MovedContentFolder>& /*MovedFolders*/);

/** Contains the delegates used to handle a custom drag-and-drop in the asset view */
struct FT1AssetViewDragAndDropExtender
{
	struct FPayload
	{
		FPayload(TSharedPtr<FDragDropOperation> InDragDropOp, const TArray<FName>& InPackagePaths, const TArray<FCollectionNameType>& InCollections)
			: DragDropOp(MoveTemp(InDragDropOp))
			, PackagePaths(InPackagePaths)
			, Collections(InCollections)
		{ }

		TSharedPtr<FDragDropOperation> DragDropOp;
		const TArray<FName>& PackagePaths;
		const TArray<FCollectionNameType>& Collections;
	};

	DECLARE_DELEGATE_RetVal_OneParam(bool, FT1OnDropDelegate, const FPayload&);
	DECLARE_DELEGATE_RetVal_OneParam(bool, FT1OnDragOverDelegate, const FPayload&);
	DECLARE_DELEGATE_RetVal_OneParam(bool, FT1OnDragLeaveDelegate, const FPayload&);

	FT1AssetViewDragAndDropExtender(FT1OnDropDelegate InOnDropDelegate)
		: OnDropDelegate(MoveTemp(InOnDropDelegate))
		, OnDragOverDelegate()
		, OnDragLeaveDelegate()
	{ }

	FT1AssetViewDragAndDropExtender(FT1OnDropDelegate InOnDropDelegate, FT1OnDragOverDelegate InOnDragOverDelegate)
		: OnDropDelegate(MoveTemp(InOnDropDelegate))
		, OnDragOverDelegate(MoveTemp(InOnDragOverDelegate))
		, OnDragLeaveDelegate()
	{ }

	FT1AssetViewDragAndDropExtender(FT1OnDropDelegate InOnDropDelegate, FT1OnDragOverDelegate InOnDragOverDelegate, FT1OnDragLeaveDelegate InOnDragLeaveDelegate)
		: OnDropDelegate(MoveTemp(InOnDropDelegate))
		, OnDragOverDelegate(MoveTemp(InOnDragOverDelegate))
		, OnDragLeaveDelegate(MoveTemp(InOnDragLeaveDelegate))
	{ }

	FT1OnDropDelegate OnDropDelegate;
	FT1OnDragOverDelegate OnDragOverDelegate;
	FT1OnDragLeaveDelegate OnDragLeaveDelegate;
};

/** Struct to define a custom column for the asset view */
struct FT1AssetViewCustomColumn
{
	/** Internal name of the column */
	FName ColumnName;

	/** Display name of the column */
	FText DisplayName;

	/** Tooltip for the column */
	FText TooltipText;

	/** Type of column, used for sorting */
	UObject::FAssetRegistryTag::ETagType DataType;

	/** Delegate to get String value for this column, used for sorting and internal use */
	FT1OnGetCustomAssetColumnData OnGetColumnData;

	/** Delegate to get Text value for this column, used to actually display */
	FT1OnGetCustomAssetColumnDisplayText OnGetColumnDisplayText;

	FT1AssetViewCustomColumn()
		: DataType(UObject::FAssetRegistryTag::TT_Alphabetical) 
	{ }

	FT1AssetViewCustomColumn(FName InColumnName, const FText& InDisplayName, const FText& InTooltipText, UObject::FAssetRegistryTag::ETagType InDataType, const FT1OnGetCustomAssetColumnData& InOnGetColumnData, const FT1OnGetCustomAssetColumnDisplayText& InOnGetColumnDisplayText = FT1OnGetCustomAssetColumnDisplayText())
		: ColumnName(InColumnName)
		, DisplayName(InDisplayName)
		, TooltipText(InTooltipText)
		, DataType(InDataType)
		, OnGetColumnData(InOnGetColumnData)
		, OnGetColumnDisplayText(InOnGetColumnDisplayText)
	{ }
};