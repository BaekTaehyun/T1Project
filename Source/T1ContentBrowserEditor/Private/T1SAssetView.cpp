// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "T1SAssetView.h"
#include "HAL/FileManager.h"
#include "UObject/UnrealType.h"
#include "Widgets/SOverlay.h"
#include "Engine/GameViewportClient.h"
#include "Factories/Factory.h"
#include "Framework/Commands/UIAction.h"
#include "Textures/SlateIcon.h"
#include "Misc/CommandLine.h"
#include "Misc/ConfigCacheIni.h"
#include "SlateOptMacros.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBorder.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Input/SSlider.h"
#include "Framework/Docking/TabManager.h"
#include "EditorStyleSet.h"
#include "EditorReimportHandler.h"
#include "Settings/ContentBrowserSettings.h"
#include "Engine/Blueprint.h"
#include "Editor.h"
#include "FileHelpers.h"
#include "AssetSelection.h"
#include "AssetRegistryModule.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "T1ContentBrowserLog.h"
#include "T1ContentBrowserEditor/Public/T1FrontendFilterBase.h"
#include "T1ContentBrowserSingleton.h"
#include "Editor/EditorWidgets/Public/EditorWidgetsModule.h"
#include "T1AssetViewTypes.h"
#include "DragAndDrop/AssetDragDropOp.h"
#include "T1DragDropHandler.h"
#include "T1AssetViewWidgets.h"
#include "T1ContentBrowserEditor/Public/T1ContentBrowserEditor.h"
#include "ObjectTools.h"
#include "T1NativeClassHierarchy.h"
#include "T1EmptyFolderVisibilityManager.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Widgets/Layout/SSplitter.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Misc/FileHelper.h"

#define LOCTEXT_NAMESPACE "T1ContentBrowserEditor"

namespace
{
	/** Time delay between recently added items being added to the filtered asset items list */
	const double TimeBetweenAddingNewAssets = 4.0;

	/** Time delay between performing the last jump, and the jump term being reset */
	const double JumpDelaySeconds = 2.0;
}

#define MAX_THUMBNAIL_SIZE 4096

ST1AssetView::~ST1AssetView()
{
	// Load the asset registry module to unregister delegates
	if ( FModuleManager::Get().IsModuleLoaded("AssetRegistry") )
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		AssetRegistryModule.Get().OnAssetAdded().RemoveAll( this );
		AssetRegistryModule.Get().OnAssetRemoved().RemoveAll( this );
		AssetRegistryModule.Get().OnAssetRenamed().RemoveAll( this );
		AssetRegistryModule.Get().OnPathAdded().RemoveAll( this );
		AssetRegistryModule.Get().OnPathRemoved().RemoveAll( this );
	}

	// Unregister listener for asset loading and object property changes
	FCoreUObjectDelegates::OnAssetLoaded.RemoveAll(this);
	FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);

	// Unsubscribe from folder population events
	{
		TSharedRef<FT1EmptyFolderVisibilityManager> EmptyFolderVisibilityManager = FT1ContentBrowserSingleton::Get().GetEmptyFolderVisibilityManager();
		EmptyFolderVisibilityManager->OnFolderPopulated().RemoveAll(this);
	}

	// Unsubscribe from class events
	if ( bCanShowClasses )
	{
		TSharedRef<FT1NativeClassHierarchy> NativeClassHierarchy = FT1ContentBrowserSingleton::Get().GetNativeClassHierarchy();
		NativeClassHierarchy->OnClassHierarchyUpdated().RemoveAll( this );
	}

	// Remove the listener for when view settings are changed
	UContentBrowserSettings::OnSettingChanged().RemoveAll(this);

	if ( FrontendFilters.IsValid() )
	{
		// Clear the frontend filter changed delegate
		FrontendFilters->OnChanged().RemoveAll( this );
	}

	// Release all rendering resources being held onto
	AssetThumbnailPool.Reset();
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void ST1AssetView::Construct( const FArguments& InArgs )
{
	bIsWorking = false;
	TotalAmortizeTime = 0;
	AmortizeStartTime = 0;
	MaxSecondsPerFrame = 0.015;

	bFillEmptySpaceInTileView = InArgs._FillEmptySpaceInTileView;
	FillScale = 1.0f;

	ThumbnailHintFadeInSequence.JumpToStart();
	ThumbnailHintFadeInSequence.AddCurve(0, 0.5f, ECurveEaseFunction::Linear);

	// Load the asset registry module to listen for updates
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	AssetRegistryModule.Get().OnAssetAdded().AddSP( this, &ST1AssetView::OnAssetAdded );
	AssetRegistryModule.Get().OnAssetRemoved().AddSP( this, &ST1AssetView::OnAssetRemoved );
	AssetRegistryModule.Get().OnAssetRenamed().AddSP( this, &ST1AssetView::OnAssetRenamed );
	AssetRegistryModule.Get().OnPathAdded().AddSP( this, &ST1AssetView::OnAssetRegistryPathAdded );
	AssetRegistryModule.Get().OnPathRemoved().AddSP( this, &ST1AssetView::OnAssetRegistryPathRemoved );

	FCollectionManagerModule& CollectionManagerModule = FCollectionManagerModule::GetModule();
	CollectionManagerModule.Get().OnAssetsAdded().AddSP( this, &ST1AssetView::OnAssetsAddedToCollection );
	CollectionManagerModule.Get().OnAssetsRemoved().AddSP( this, &ST1AssetView::OnAssetsRemovedFromCollection );
	CollectionManagerModule.Get().OnCollectionRenamed().AddSP( this, &ST1AssetView::OnCollectionRenamed );
	CollectionManagerModule.Get().OnCollectionUpdated().AddSP( this, &ST1AssetView::OnCollectionUpdated );

	// Listen for when assets are loaded or changed to update item data
	FCoreUObjectDelegates::OnAssetLoaded.AddSP(this, &ST1AssetView::OnAssetLoaded);
	FCoreUObjectDelegates::OnObjectPropertyChanged.AddSP(this, &ST1AssetView::OnObjectPropertyChanged);

	// Listen to find out when the available classes are changed, so that we can refresh our paths
	if ( bCanShowClasses )
	{
		TSharedRef<FT1NativeClassHierarchy> NativeClassHierarchy = FT1ContentBrowserSingleton::Get().GetNativeClassHierarchy();
		NativeClassHierarchy->OnClassHierarchyUpdated().AddSP( this, &ST1AssetView::OnClassHierarchyUpdated );
	}

	// Listen to find out when previously empty paths are populated with content
	{
		TSharedRef<FT1EmptyFolderVisibilityManager> EmptyFolderVisibilityManager = FT1ContentBrowserSingleton::Get().GetEmptyFolderVisibilityManager();
		EmptyFolderVisibilityManager->OnFolderPopulated().AddSP(this, &ST1AssetView::OnFolderPopulated);
	}

	// Listen for when view settings are changed
	UContentBrowserSettings::OnSettingChanged().AddSP(this, &ST1AssetView::HandleSettingChanged);

	// Get desktop metrics
	FDisplayMetrics DisplayMetrics;
	FSlateApplication::Get().GetDisplayMetrics( DisplayMetrics );

	const FVector2D DisplaySize(
		DisplayMetrics.PrimaryDisplayWorkAreaRect.Right - DisplayMetrics.PrimaryDisplayWorkAreaRect.Left,
		DisplayMetrics.PrimaryDisplayWorkAreaRect.Bottom - DisplayMetrics.PrimaryDisplayWorkAreaRect.Top );

	const float ThumbnailScaleRangeScalar = ( DisplaySize.Y / 1080 );

	// Create a thumbnail pool for rendering thumbnails	
	AssetThumbnailPool = MakeShareable( new FAssetThumbnailPool(1024, InArgs._AreRealTimeThumbnailsAllowed) );
	NumOffscreenThumbnails = 64;
	ListViewThumbnailResolution = 128;
	ListViewThumbnailSize = 64;
	ListViewThumbnailPadding = 4;
	TileViewThumbnailResolution = 256;
	TileViewThumbnailSize = 128;
	TileViewThumbnailPadding = 5;

	TileViewNameHeight = 36;
	ThumbnailScaleSliderValue = InArgs._ThumbnailScale; 

	if ( !ThumbnailScaleSliderValue.IsBound() )
	{
		ThumbnailScaleSliderValue = FMath::Clamp<float>(ThumbnailScaleSliderValue.Get(), 0.0f, 1.0f);
	}

	MinThumbnailScale = 0.2f * ThumbnailScaleRangeScalar;
	MaxThumbnailScale = 2.0f * ThumbnailScaleRangeScalar;

	bCanShowClasses = InArgs._CanShowClasses;

	bCanShowFolders = InArgs._CanShowFolders;

	bFilterRecursivelyWithBackendFilter = InArgs._FilterRecursivelyWithBackendFilter;
		
	bCanShowRealTimeThumbnails = InArgs._CanShowRealTimeThumbnails;

	bCanShowDevelopersFolder = InArgs._CanShowDevelopersFolder;

	bCanShowCollections = InArgs._CanShowCollections;
	bCanShowFavorites = InArgs._CanShowFavorites;
	bPreloadAssetsForContextMenu = InArgs._PreloadAssetsForContextMenu;

	SelectionMode = InArgs._SelectionMode;

	bShowPathInColumnView = InArgs._ShowPathInColumnView;
	bShowTypeInColumnView = InArgs._ShowTypeInColumnView;
	bSortByPathInColumnView = bShowPathInColumnView & InArgs._SortByPathInColumnView;

	bPendingUpdateThumbnails = false;
	bShouldNotifyNextAssetSync = true;
	CurrentThumbnailSize = TileViewThumbnailSize;

	T1SourcesData = InArgs._InitialSourcesData;
	BackendFilter = InArgs._InitialBackendFilter;

	FrontendFilters = InArgs._FrontendFilters;
	if ( FrontendFilters.IsValid() )
	{
		FrontendFilters->OnChanged().AddSP( this, &ST1AssetView::OnFrontendFiltersChanged );
	}

	OnShouldFilterAsset = InArgs._OnShouldFilterAsset;
	OnAssetSelected = InArgs._OnAssetSelected;
	OnAssetSelectionChanged = InArgs._OnAssetSelectionChanged;
	OnAssetsActivated = InArgs._OnAssetsActivated;
	OnGetAssetContextMenu = InArgs._OnGetAssetContextMenu;
	OnGetFolderContextMenu = InArgs._OnGetFolderContextMenu;
	OnGetPathContextMenuExtender = InArgs._OnGetPathContextMenuExtender;
	OnFindInAssetTreeRequested = InArgs._OnFindInAssetTreeRequested;
	OnAssetRenameCommitted = InArgs._OnAssetRenameCommitted;
	OnAssetTagWantsToBeDisplayed = InArgs._OnAssetTagWantsToBeDisplayed;
	OnIsAssetValidForCustomToolTip = InArgs._OnIsAssetValidForCustomToolTip;
	OnGetCustomAssetToolTip = InArgs._OnGetCustomAssetToolTip;
	OnVisualizeAssetToolTip = InArgs._OnVisualizeAssetToolTip;
	OnAssetToolTipClosing = InArgs._OnAssetToolTipClosing;
	OnGetCustomSourceAssets = InArgs._OnGetCustomSourceAssets;
	HighlightedText = InArgs._HighlightedText;
	ThumbnailLabel = InArgs._ThumbnailLabel;
	AllowThumbnailHintLabel = InArgs._AllowThumbnailHintLabel;
	AssetShowWarningText = InArgs._AssetShowWarningText;
	bAllowDragging = InArgs._AllowDragging;
	bAllowFocusOnSync = InArgs._AllowFocusOnSync;
	OnPathSelected = InArgs._OnPathSelected;
	HiddenColumnNames = DefaultHiddenColumnNames = InArgs._HiddenColumnNames;
	CustomColumns = InArgs._CustomColumns;
	OnSearchOptionsChanged = InArgs._OnSearchOptionsChanged;

	if ( InArgs._InitialViewType >= 0 && InArgs._InitialViewType < ET1AssetViewType::MAX )
	{
		CurrentViewType = InArgs._InitialViewType;
	}
	else
	{
		CurrentViewType = ET1AssetViewType::Tile;
	}

	bPendingSortFilteredItems = false;
	bQuickFrontendListRefreshRequested = false;
	bSlowFullListRefreshRequested = false;
	LastSortTime = 0;
	SortDelaySeconds = 8;

	LastProcessAddsTime = 0;

	bBulkSelecting = false;
	bAllowThumbnailEditMode = InArgs._AllowThumbnailEditMode;
	bThumbnailEditMode = false;
	bUserSearching = false;
	bPendingFocusOnSync = false;
	bWereItemsRecursivelyFiltered = false;

	NumVisibleColumns = 0;

	FEditorWidgetsModule& EditorWidgetsModule = FModuleManager::LoadModuleChecked<FEditorWidgetsModule>("EditorWidgets");
	TSharedRef<SWidget> AssetDiscoveryIndicator = EditorWidgetsModule.CreateAssetDiscoveryIndicator(EAssetDiscoveryIndicatorScaleMode::Scale_Vertical);

	TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox);

	ChildSlot
	[
		VerticalBox
	];

	// Assets area
	VerticalBox->AddSlot()
	.FillHeight(1.f)
	[
		SNew( SVerticalBox ) 

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew( SBox )
			.Visibility_Lambda([this] { return bIsWorking ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed; })
			.HeightOverride( 2 )
			[
				SNew( SProgressBar )
				.Percent( this, &ST1AssetView::GetIsWorkingProgressBarState )
				.Style( FEditorStyle::Get(), "WorkingBar" )
				.BorderPadding( FVector2D(0,0) )
			]
		]
		
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		[
			SNew(SOverlay)

			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				// Container for the view types
				SAssignNew(ViewContainer, SBorder)
				.Padding(0)
				.BorderImage(FEditorStyle::GetBrush("NoBorder"))
			]

			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Center)
			.Padding(FMargin(0, 14, 0, 0))
			[
				// A warning to display when there are no assets to show
				SNew( STextBlock )
				.Justification( ETextJustify::Center )
				.Text( this, &ST1AssetView::GetAssetShowWarningText )
				.Visibility( this, &ST1AssetView::IsAssetShowWarningTextVisible )
				.AutoWrapText( true )
			]

			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Bottom)
			.Padding(FMargin(24, 0, 24, 0))
			[
				// Asset discovery indicator
				AssetDiscoveryIndicator
			]

			+ SOverlay::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Bottom)
			.Padding(FMargin(8, 0))
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("ErrorReporting.EmptyBox"))
				.BorderBackgroundColor(this, &ST1AssetView::GetQuickJumpColor)
				.Visibility(this, &ST1AssetView::IsQuickJumpVisible)
				[
					SNew(STextBlock)
					.Text(this, &ST1AssetView::GetQuickJumpTerm)
				]
			]
		]
	];

	// Thumbnail edit mode banner
	VerticalBox->AddSlot()
	.AutoHeight()
	.Padding(0, 4)
	[
		SNew(SBorder)
		.Visibility( this, &ST1AssetView::GetEditModeLabelVisibility )
		.BorderImage( FEditorStyle::GetBrush("ContentBrowser.EditModeLabelBorder") )
		.Content()
		[
			SNew( SHorizontalBox )

			+SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.Padding(4, 0, 0, 0)
			.FillWidth(1.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("ThumbnailEditModeLabel", "Editing Thumbnails. Drag a thumbnail to rotate it if there is a 3D environment."))
				.TextStyle( FEditorStyle::Get(), "T1ContentBrowser.EditModeLabelFont" )
			]

			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SButton)
				.Text( LOCTEXT("EndThumbnailEditModeButton", "Done Editing") )
				.OnClicked( this, &ST1AssetView::EndThumbnailEditModeClicked )
			]
		]
	];

	if (InArgs._ShowBottomToolbar)
	{
		// Bottom panel
		VerticalBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			// Asset count
			+SHorizontalBox::Slot()
			.FillWidth(1.f)
			.VAlign(VAlign_Center)
			.Padding(8, 0)
			[
				SNew(STextBlock)
				.Text(this, &ST1AssetView::GetAssetCountText)
			]

			// View mode combo button
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew( ViewOptionsComboButton, SComboButton )
				.ContentPadding(0)
				.ForegroundColor( this, &ST1AssetView::GetViewButtonForegroundColor )
				.ButtonStyle( FEditorStyle::Get(), "ToggleButton" ) // Use the tool bar item style for this button
				.OnGetMenuContent( this, &ST1AssetView::GetViewButtonContent )
				.ButtonContent()
				[
					SNew(SHorizontalBox)
 
					+SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					[
						SNew(SImage).Image( FEditorStyle::GetBrush("GenericViewButton") )
					]
 
					+SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(2, 0, 0, 0)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock).Text( LOCTEXT("ViewButton", "View Options") )
					]
				]
			]
		];
	}

	CreateCurrentView();

	if( InArgs._InitialAssetSelection.IsValid() )
	{
		// sync to the initial item without notifying of selection
		bShouldNotifyNextAssetSync = false;
		TArray<FAssetData> AssetsToSync;
		AssetsToSync.Add( InArgs._InitialAssetSelection );
		SyncToAssets( AssetsToSync );
	}

	// If currently looking at column, and you could choose to sort by path in column first and then name
	// Generalizing this is a bit difficult because the column ID is not accessible or is not known
	// Currently I assume this won't work, if this view mode is not column. Otherwise, I don't think sorting by path
	// is a good idea. 
	if (CurrentViewType == ET1AssetViewType::Column && bSortByPathInColumnView)
	{
		SortManager.SetSortColumnId(EColumnSortPriority::Primary, SortManager.PathColumnId);
		SortManager.SetSortColumnId(EColumnSortPriority::Secondary, SortManager.NameColumnId);
		SortManager.SetSortMode(EColumnSortPriority::Primary, EColumnSortMode::Ascending);
		SortManager.SetSortMode(EColumnSortPriority::Secondary, EColumnSortMode::Ascending);
		SortList();
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

TOptional< float > ST1AssetView::GetIsWorkingProgressBarState() const
{
	return bIsWorking ? TOptional< float >() : 0.0f; 
}

void ST1AssetView::SetSourcesData(const FT1SourcesData& InSourcesData)
{
	// Update the path and collection lists
	T1SourcesData = InSourcesData;
	RequestSlowFullListRefresh();
	ClearSelection();
}

const FT1SourcesData& ST1AssetView::GetSourcesData() const
{
	return T1SourcesData;
}

bool ST1AssetView::IsAssetPathSelected() const
{
	int32 NumAssetPaths, NumClassPaths;
	T1ContentBrowserUtils::CountPathTypes(T1SourcesData.PackagePaths, NumAssetPaths, NumClassPaths);

	// Check that only asset paths are selected
	return NumAssetPaths > 0 && NumClassPaths == 0;
}

void ST1AssetView::SetBackendFilter(const FARFilter& InBackendFilter)
{
	// Update the path and collection lists
	BackendFilter = InBackendFilter;
	RequestSlowFullListRefresh();
}

void ST1AssetView::OnCreateNewFolder(const FString& FolderName, const FString& FolderPath)
{
	// we should only be creating one deferred folder per tick
	check(!DeferredFolderToCreate.IsValid());

	// Make sure we are showing the location of the new folder (we may have created it in a folder)
	OnPathSelected.Execute(FolderPath);

	DeferredFolderToCreate = MakeShareable(new FCreateDeferredFolderData());
	DeferredFolderToCreate->FolderName = FolderName;
	DeferredFolderToCreate->FolderPath = FolderPath;
}

void ST1AssetView::DeferredCreateNewFolder()
{
	if(DeferredFolderToCreate.IsValid())
	{
		TSharedPtr<FT1AssetViewFolder> NewItem = MakeShareable(new FT1AssetViewFolder(DeferredFolderToCreate->FolderPath / DeferredFolderToCreate->FolderName));
		NewItem->bNewFolder = true;
		NewItem->bRenameWhenScrolledIntoview = true;
		FilteredAssetItems.Insert( NewItem, 0 );

		SetSelection(NewItem);
		RequestScrollIntoView(NewItem);

		DeferredFolderToCreate.Reset();
	}
}

void ST1AssetView::CreateNewAsset(const FString& DefaultAssetName, const FString& PackagePath, UClass* AssetClass, UFactory* Factory)
{
	if ( !ensure(AssetClass || Factory) )
	{
		return;
	}

	if ( AssetClass && Factory && !ensure(AssetClass->IsChildOf(Factory->GetSupportedClass())) )
	{
		return;
	}
	
	// we should only be creating one deferred asset per tick
	check(!DeferredAssetToCreate.IsValid());

	// Make sure we are showing the location of the new asset (we may have created it in a folder)
	OnPathSelected.Execute(PackagePath);

	// Defer asset creation until next tick, so we get a chance to refresh the view
	DeferredAssetToCreate = MakeShareable(new FCreateDeferredAssetData());
	DeferredAssetToCreate->DefaultAssetName = DefaultAssetName;
	DeferredAssetToCreate->PackagePath = PackagePath;
	DeferredAssetToCreate->AssetClass = AssetClass;
	DeferredAssetToCreate->Factory = Factory;
}

void ST1AssetView::DeferredCreateNewAsset()
{
	if(DeferredAssetToCreate.IsValid())
	{
		FString PackageNameStr = DeferredAssetToCreate->PackagePath + "/" + DeferredAssetToCreate->DefaultAssetName;
		FName PackageName = FName(*PackageNameStr);
		FName PackagePathFName = FName(*DeferredAssetToCreate->PackagePath);
		FName AssetName = FName(*DeferredAssetToCreate->DefaultAssetName);
		FName AssetClassName = DeferredAssetToCreate->AssetClass->GetFName();

		FAssetData NewAssetData(PackageName, PackagePathFName, AssetName, AssetClassName);
		TSharedPtr<FT1AssetViewItem> NewItem = MakeShareable(new FAssetViewCreation(NewAssetData, DeferredAssetToCreate->AssetClass, DeferredAssetToCreate->Factory));

		NewItem->bRenameWhenScrolledIntoview = true;
		FilteredAssetItems.Insert( NewItem, 0 );
		SortManager.SortList(FilteredAssetItems, MajorityAssetType, CustomColumns);

		SetSelection(NewItem);
		RequestScrollIntoView(NewItem);

		FEditorDelegates::OnNewAssetCreated.Broadcast(DeferredAssetToCreate->Factory);

		DeferredAssetToCreate.Reset();
	}
}

void ST1AssetView::DuplicateAsset(const FString& PackagePath, const TWeakObjectPtr<UObject>& OriginalObject)
{
	if ( !ensure(OriginalObject.IsValid()) )
	{
		return;
	}

	FString AssetNameStr;
	FString PackageNameStr;

	// Find a unique default name for the duplicated asset
	static FName AssetToolsModuleName = FName("AssetTools");
	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>(AssetToolsModuleName);
	AssetToolsModule.Get().CreateUniqueAssetName(PackagePath + TEXT("/") + OriginalObject->GetName(), TEXT(""), PackageNameStr, AssetNameStr);

	FName PackageName = FName(*PackageNameStr);
	FName PackagePathFName = FName(*PackagePath);
	FName AssetName = FName(*AssetNameStr);
	FName AssetClass = OriginalObject->GetClass()->GetFName();
	
	FAssetData NewAssetData(PackageName, PackagePathFName, AssetName, AssetClass);
	TSharedPtr<FT1AssetViewItem> NewItem = MakeShareable(new FT1AssetViewDuplication(NewAssetData, OriginalObject));
	NewItem->bRenameWhenScrolledIntoview = true;

	// Insert into the list and sort
	FilteredAssetItems.Insert( NewItem, 0 );
	SortManager.SortList(FilteredAssetItems, MajorityAssetType, CustomColumns);

	SetSelection(NewItem);
	RequestScrollIntoView(NewItem);
}

void ST1AssetView::RenameAsset(const FAssetData& ItemToRename)
{
	for ( auto ItemIt = FilteredAssetItems.CreateConstIterator(); ItemIt; ++ItemIt )
	{
		const TSharedPtr<FT1AssetViewItem>& Item = *ItemIt;
		if ( Item.IsValid() && Item->GetType() != EAssetItemType::Folder )	
		{
			const TSharedPtr<FT1AssetViewAsset>& ItemAsAsset = StaticCastSharedPtr<FT1AssetViewAsset>(Item);
			if ( ItemAsAsset->Data.ObjectPath == ItemToRename.ObjectPath )
			{
				ItemAsAsset->bRenameWhenScrolledIntoview = true;

				SetSelection(Item);
				RequestScrollIntoView(Item);
				break;
			}
		}
	}
}

void ST1AssetView::RenameFolder(const FString& FolderToRename)
{
	for ( auto ItemIt = FilteredAssetItems.CreateConstIterator(); ItemIt; ++ItemIt )
	{
		const TSharedPtr<FT1AssetViewItem>& Item = *ItemIt;
		if ( Item.IsValid() && Item->GetType() == EAssetItemType::Folder )	
		{
			const TSharedPtr<FT1AssetViewFolder>& ItemAsFolder = StaticCastSharedPtr<FT1AssetViewFolder>(Item);
			if ( ItemAsFolder->FolderPath == FolderToRename )
			{
				ItemAsFolder->bRenameWhenScrolledIntoview = true;

				SetSelection(Item);
				RequestScrollIntoView(Item);
				break;
			}
		}
	}
}

void ST1AssetView::SyncToAssets( const TArray<FAssetData>& AssetDataList, const bool bFocusOnSync )
{
	PendingSyncItems.Reset();
	for (const FAssetData& AssetData : AssetDataList)
	{
		PendingSyncItems.SelectedAssets.Add(AssetData.ObjectPath);
	}

	bPendingFocusOnSync = bFocusOnSync;
}

void ST1AssetView::SyncToFolders(const TArray<FString>& FolderList, const bool bFocusOnSync)
{
	PendingSyncItems.Reset();
	PendingSyncItems.SelectedFolders = TSet<FString>(FolderList);

	bPendingFocusOnSync = bFocusOnSync;
}

void ST1AssetView::SyncTo(const FT1ContentBrowserSelection& ItemSelection, const bool bFocusOnSync)
{
	PendingSyncItems.Reset();
	PendingSyncItems.SelectedFolders = TSet<FString>(ItemSelection.SelectedFolders);
	for (const FAssetData& AssetData : ItemSelection.SelectedAssets)
	{
		PendingSyncItems.SelectedAssets.Add(AssetData.ObjectPath);
	}

	bPendingFocusOnSync = bFocusOnSync;
}

void ST1AssetView::SyncToSelection( const bool bFocusOnSync )
{
	PendingSyncItems.Reset();

	TArray<TSharedPtr<FT1AssetViewItem>> SelectedItems = GetSelectedItems();
	for (const TSharedPtr<FT1AssetViewItem>& Item : SelectedItems)
	{
		if (Item.IsValid())
		{
			if (Item->GetType() == EAssetItemType::Folder)
			{
				PendingSyncItems.SelectedFolders.Add(StaticCastSharedPtr<FT1AssetViewFolder>(Item)->FolderPath);
			}
			else
			{
				PendingSyncItems.SelectedAssets.Add(StaticCastSharedPtr<FT1AssetViewAsset>(Item)->Data.ObjectPath);
			}
		}
	}

	bPendingFocusOnSync = bFocusOnSync;
}

void ST1AssetView::ApplyHistoryData( const FT1HistoryData& History )
{
	SetSourcesData(History.T1SourcesData);
	PendingSyncItems = History.SelectionData;
	bPendingFocusOnSync = true;
}

TArray<TSharedPtr<FT1AssetViewItem>> ST1AssetView::GetSelectedItems() const
{
	switch ( GetCurrentViewType() )
	{
		case ET1AssetViewType::List: return ListView->GetSelectedItems();
		case ET1AssetViewType::Tile: return TileView->GetSelectedItems();
		case ET1AssetViewType::Column: return ColumnView->GetSelectedItems();
		default:
		ensure(0); // Unknown list type
		return TArray<TSharedPtr<FT1AssetViewItem>>();
	}
}

TArray<FAssetData> ST1AssetView::GetSelectedAssets() const
{
	TArray<TSharedPtr<FT1AssetViewItem>> SelectedItems = GetSelectedItems();
	TArray<FAssetData> SelectedAssets;
	for ( auto ItemIt = SelectedItems.CreateConstIterator(); ItemIt; ++ItemIt )
	{
		const TSharedPtr<FT1AssetViewItem>& Item = *ItemIt;

		// Only report non-temporary & non-folder items
		if ( Item.IsValid() && !Item->IsTemporaryItem() && Item->GetType() != EAssetItemType::Folder )	
		{
			SelectedAssets.Add(StaticCastSharedPtr<FT1AssetViewAsset>(Item)->Data);
		}
	}

	return SelectedAssets;
}

TArray<FString> ST1AssetView::GetSelectedFolders() const
{
	TArray<TSharedPtr<FT1AssetViewItem>> SelectedItems = GetSelectedItems();
	TArray<FString> SelectedFolders;
	for ( auto ItemIt = SelectedItems.CreateConstIterator(); ItemIt; ++ItemIt )
	{
		const TSharedPtr<FT1AssetViewItem>& Item = *ItemIt;
		if ( Item.IsValid() && Item->GetType() == EAssetItemType::Folder )	
		{
			SelectedFolders.Add(StaticCastSharedPtr<FT1AssetViewFolder>(Item)->FolderPath);
		}
	}

	return SelectedFolders;
}

void ST1AssetView::RequestSlowFullListRefresh()
{
	bSlowFullListRefreshRequested = true;
}

void ST1AssetView::RequestQuickFrontendListRefresh()
{
	bQuickFrontendListRefreshRequested = true;
}

void ST1AssetView::RequestAddNewAssetsNextFrame()
{
	LastProcessAddsTime = FPlatformTime::Seconds() - TimeBetweenAddingNewAssets;
}

FString ST1AssetView::GetThumbnailScaleSettingPath(const FString& SettingsString) const
{
	return SettingsString + TEXT(".ThumbnailSizeScale");
}

FString ST1AssetView::GetCurrentViewTypeSettingPath(const FString& SettingsString) const
{
	return SettingsString + TEXT(".CurrentViewType");
}

void ST1AssetView::SaveSettings(const FString& IniFilename, const FString& IniSection, const FString& SettingsString) const
{
	GConfig->SetFloat(*IniSection, *GetThumbnailScaleSettingPath(SettingsString), ThumbnailScaleSliderValue.Get(), IniFilename);
	GConfig->SetInt(*IniSection, *GetCurrentViewTypeSettingPath(SettingsString), CurrentViewType, IniFilename);
	
	GConfig->SetArray(*IniSection, *(SettingsString + TEXT(".HiddenColumns")), HiddenColumnNames, IniFilename);
}

void ST1AssetView::LoadSettings(const FString& IniFilename, const FString& IniSection, const FString& SettingsString)
{
	float Scale = 0.f;
	if ( GConfig->GetFloat(*IniSection, *GetThumbnailScaleSettingPath(SettingsString), Scale, IniFilename) )
	{
		// Clamp value to normal range and update state
		Scale = FMath::Clamp<float>(Scale, 0.f, 1.f);
		SetThumbnailScale(Scale);
	}

	int32 ViewType = ET1AssetViewType::Tile;
	if ( GConfig->GetInt(*IniSection, *GetCurrentViewTypeSettingPath(SettingsString), ViewType, IniFilename) )
	{
		// Clamp value to normal range and update state
		if ( ViewType < 0 || ViewType >= ET1AssetViewType::MAX)
		{
			ViewType = ET1AssetViewType::Tile;
		}
		SetCurrentViewType( (ET1AssetViewType::Type)ViewType );
	}
	
	TArray<FString> LoadedHiddenColumnNames;
	GConfig->GetArray(*IniSection, *(SettingsString + TEXT(".HiddenColumns")), LoadedHiddenColumnNames, IniFilename);
	if (LoadedHiddenColumnNames.Num() > 0)
	{
		HiddenColumnNames = LoadedHiddenColumnNames;
	}
}

// Adjusts the selected asset by the selection delta, which should be +1 or -1)
void ST1AssetView::AdjustActiveSelection(int32 SelectionDelta)
{
	// Find the index of the first selected item
	TArray<TSharedPtr<FT1AssetViewItem>> SelectionSet = GetSelectedItems();
	
	int32 SelectedSuggestion = INDEX_NONE;

	if (SelectionSet.Num() > 0)
	{
		if (!FilteredAssetItems.Find(SelectionSet[0], /*out*/ SelectedSuggestion))
		{
			// Should never happen
			ensureMsgf(false, TEXT("ST1AssetView has a selected item that wasn't in the filtered list"));
			return;
		}
	}
	else
	{
		SelectedSuggestion = 0;
		SelectionDelta = 0;
	}

	if (FilteredAssetItems.Num() > 0)
	{
		// Move up or down one, wrapping around
		SelectedSuggestion = (SelectedSuggestion + SelectionDelta + FilteredAssetItems.Num()) % FilteredAssetItems.Num();

		// Pick the new asset
		const TSharedPtr<FT1AssetViewItem>& NewSelection = FilteredAssetItems[SelectedSuggestion];

		RequestScrollIntoView(NewSelection);
		SetSelection(NewSelection);
	}
	else
	{
		ClearSelection();
	}
}

void ST1AssetView::ProcessRecentlyLoadedOrChangedAssets()
{
	if ( RecentlyLoadedOrChangedAssets.Num() > 0 )
	{
		TMap< FName, TWeakObjectPtr<UObject> > NextRecentlyLoadedOrChangedMap = RecentlyLoadedOrChangedAssets;

		for (int32 AssetIdx = FilteredAssetItems.Num() - 1; AssetIdx >= 0; --AssetIdx)
		{
			if(FilteredAssetItems[AssetIdx]->GetType() != EAssetItemType::Folder)
			{
				const TSharedPtr<FT1AssetViewAsset>& ItemAsAsset = StaticCastSharedPtr<FT1AssetViewAsset>(FilteredAssetItems[AssetIdx]);
				const FName ObjectPath = ItemAsAsset->Data.ObjectPath;
				const TWeakObjectPtr<UObject>* WeakAssetPtr = RecentlyLoadedOrChangedAssets.Find( ObjectPath );
				if ( WeakAssetPtr && (*WeakAssetPtr).IsValid() )
				{
					NextRecentlyLoadedOrChangedMap.Remove(ObjectPath);

					// Found the asset in the filtered items list, update it
					const UObject* Asset = (*WeakAssetPtr).Get();
					FAssetData AssetData(Asset);

					bool bShouldRemoveAsset = false;
					TArray<FAssetData> AssetDataThatPassesFilter;
					AssetDataThatPassesFilter.Add(AssetData);
					RunAssetsThroughBackendFilter(AssetDataThatPassesFilter);
					if ( AssetDataThatPassesFilter.Num() == 0 )
					{
						bShouldRemoveAsset = true;
					}

					if ( !bShouldRemoveAsset && OnShouldFilterAsset.IsBound() && OnShouldFilterAsset.Execute(AssetData) )
					{
						bShouldRemoveAsset = true;
					}

					if ( !bShouldRemoveAsset && (IsFrontendFilterActive() && !PassesCurrentFrontendFilter(AssetData)) )
					{
						bShouldRemoveAsset = true;
					}

					if ( bShouldRemoveAsset )
					{
						FilteredAssetItems.RemoveAt(AssetIdx);
					}
					else
					{
						// Update the asset data on the item
						ItemAsAsset->SetAssetData(AssetData);

						// Update the custom column data
						ItemAsAsset->CacheCustomColumns(CustomColumns, true, true, true);
					}

					RefreshList();
				}
			}
		}

		if( FilteredRecentlyAddedAssets.Num() > 0 || RecentlyAddedAssets.Num() > 0 )
		{
			//Keep unprocessed items as we are still processing assets
			RecentlyLoadedOrChangedAssets = NextRecentlyLoadedOrChangedMap;
		}
		else
		{
			//No more assets coming in so if we haven't found them now we aren't going to
			RecentlyLoadedOrChangedAssets.Empty();
		}
	}
}

void ST1AssetView::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
	CalculateFillScale( AllottedGeometry );

	CurrentTime = InCurrentTime;

	// If there were any assets that were recently added via the asset registry, process them now
	ProcessRecentlyAddedAssets();

	// If there were any assets loaded since last frame that we are currently displaying thumbnails for, push them on the render stack now.
	ProcessRecentlyLoadedOrChangedAssets();

	CalculateThumbnailHintColorAndOpacity();

	if ( bPendingUpdateThumbnails )
	{
		UpdateThumbnails();
		bPendingUpdateThumbnails = false;
	}

	if (bSlowFullListRefreshRequested)
	{
		RefreshSourceItems();
		bSlowFullListRefreshRequested = false;
		bQuickFrontendListRefreshRequested = true;
	}

	if (QueriedAssetItems.Num() > 0)
	{
		check(OnShouldFilterAsset.IsBound());
		double TickStartTime = FPlatformTime::Seconds();

		// Mark the first amortize time
		if (AmortizeStartTime == 0)
		{
			AmortizeStartTime = FPlatformTime::Seconds();
			bIsWorking = true;
		}

		ProcessQueriedItems(TickStartTime);

		if (QueriedAssetItems.Num() == 0)
		{
			TotalAmortizeTime += FPlatformTime::Seconds() - AmortizeStartTime;
			AmortizeStartTime = 0;
			bIsWorking = false;
		}
		else
		{
			// Need to finish processing queried items before rest of function is safe
			return;
		}
	}

	if (bQuickFrontendListRefreshRequested)
	{
		ResetQuickJump();
		
		RefreshFilteredItems();
		RefreshFolders();
		// Don't sync to selection if we are just going to do it below
		SortList(!PendingSyncItems.Num());

		bQuickFrontendListRefreshRequested = false;
	}

	if ( PendingSyncItems.Num() > 0 )
	{
		if (bPendingSortFilteredItems)
		{
			// Don't sync to selection because we are just going to do it below
			SortList(/*bSyncToSelection=*/false);
		}
		
		bBulkSelecting = true;
		ClearSelection();
		bool bFoundScrollIntoViewTarget = false;

		for ( auto ItemIt = FilteredAssetItems.CreateConstIterator(); ItemIt; ++ItemIt )
		{
			const auto& Item = *ItemIt;
			if(Item.IsValid())
			{
				if(Item->GetType() == EAssetItemType::Folder)
				{
					const TSharedPtr<FT1AssetViewFolder>& ItemAsFolder = StaticCastSharedPtr<FT1AssetViewFolder>(Item);
					if ( PendingSyncItems.SelectedFolders.Contains(ItemAsFolder->FolderPath) )
					{
						SetItemSelection(*ItemIt, true, ESelectInfo::OnNavigation);

						// Scroll the first item in the list that can be shown into view
						if ( !bFoundScrollIntoViewTarget )
						{
							RequestScrollIntoView(Item);
							bFoundScrollIntoViewTarget = true;
						}
					}
				}
				else
				{
					const TSharedPtr<FT1AssetViewAsset>& ItemAsAsset = StaticCastSharedPtr<FT1AssetViewAsset>(Item);
					if ( PendingSyncItems.SelectedAssets.Contains(ItemAsAsset->Data.ObjectPath) )
					{
						SetItemSelection(*ItemIt, true, ESelectInfo::OnNavigation);

						// Scroll the first item in the list that can be shown into view
						if ( !bFoundScrollIntoViewTarget )
						{
							RequestScrollIntoView(Item);
							bFoundScrollIntoViewTarget = true;
						}
					}
				}
			}
		}
	
		bBulkSelecting = false;

		if (bShouldNotifyNextAssetSync && !bUserSearching)
		{
			AssetSelectionChanged(TSharedPtr<FT1AssetViewAsset>(), ESelectInfo::Direct);
		}

		// Default to always notifying
		bShouldNotifyNextAssetSync = true;

		PendingSyncItems.Reset();

		if (bAllowFocusOnSync && bPendingFocusOnSync)
		{
			FocusList();
		}
	}

	if ( IsHovered() )
	{
		// This prevents us from sorting the view immediately after the cursor leaves it
		LastSortTime = CurrentTime;
	}
	else if ( bPendingSortFilteredItems && InCurrentTime > LastSortTime + SortDelaySeconds )
	{
		SortList();
	}

	// create any assets & folders we need to now
	DeferredCreateNewAsset();
	DeferredCreateNewFolder();

	// Do quick-jump last as the Tick function might have canceled it
	if(QuickJumpData.bHasChangedSinceLastTick)
	{
		QuickJumpData.bHasChangedSinceLastTick = false;

		const bool bWasJumping = QuickJumpData.bIsJumping;
		QuickJumpData.bIsJumping = true;

		QuickJumpData.LastJumpTime = InCurrentTime;
		QuickJumpData.bHasValidMatch = PerformQuickJump(bWasJumping);
	}
	else if(QuickJumpData.bIsJumping && InCurrentTime > QuickJumpData.LastJumpTime + JumpDelaySeconds)
	{
		ResetQuickJump();
	}

	TSharedPtr<FT1AssetViewItem> AssetAwaitingRename = AwaitingRename.Pin();
	if (AssetAwaitingRename.IsValid())
	{
		TSharedPtr<SWindow> OwnerWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
		if (!OwnerWindow.IsValid())
		{
			AssetAwaitingRename->bRenameWhenScrolledIntoview = false;
			AwaitingRename = nullptr;
		}
		else if (OwnerWindow->HasAnyUserFocusOrFocusedDescendants())
		{
			AssetAwaitingRename->RenamedRequestEvent.ExecuteIfBound();
			AssetAwaitingRename->bRenameWhenScrolledIntoview = false;
			AwaitingRename = nullptr;
		}
	}
}

void ST1AssetView::CalculateFillScale( const FGeometry& AllottedGeometry )
{
	if ( bFillEmptySpaceInTileView && CurrentViewType == ET1AssetViewType::Tile )
	{
		float ItemWidth = GetTileViewItemBaseWidth();

		// Scrollbars are 16, but we add 1 to deal with half pixels.
		const float ScrollbarWidth = 16 + 1;
		float TotalWidth = AllottedGeometry.GetLocalSize().X - ( ScrollbarWidth / AllottedGeometry.Scale );
		float Coverage = TotalWidth / ItemWidth;
		int32 Items = (int)( TotalWidth / ItemWidth );

		// If there isn't enough room to support even a single item, don't apply a fill scale.
		if ( Items > 0 )
		{
			float GapSpace = ItemWidth * ( Coverage - Items );
			float ExpandAmount = GapSpace / (float)Items;
			FillScale = ( ItemWidth + ExpandAmount ) / ItemWidth;
			FillScale = FMath::Max( 1.0f, FillScale );
		}
		else
		{
			FillScale = 1.0f;
		}
	}
	else
	{
		FillScale = 1.0f;
	}
}

void ST1AssetView::CalculateThumbnailHintColorAndOpacity()
{
	if ( HighlightedText.Get().IsEmpty() )
	{
		if ( ThumbnailHintFadeInSequence.IsPlaying() )
		{
			if ( ThumbnailHintFadeInSequence.IsForward() )
			{
				ThumbnailHintFadeInSequence.Reverse();
			}
		}
		else if ( ThumbnailHintFadeInSequence.IsAtEnd() ) 
		{
			ThumbnailHintFadeInSequence.PlayReverse(this->AsShared());
		}
	}
	else 
	{
		if ( ThumbnailHintFadeInSequence.IsPlaying() )
		{
			if ( ThumbnailHintFadeInSequence.IsInReverse() )
			{
				ThumbnailHintFadeInSequence.Reverse();
			}
		}
		else if ( ThumbnailHintFadeInSequence.IsAtStart() ) 
		{
			ThumbnailHintFadeInSequence.Play(this->AsShared());
		}
	}

	const float Opacity = ThumbnailHintFadeInSequence.GetLerp();
	ThumbnailHintColorAndOpacity = FLinearColor( 1.0, 1.0, 1.0, Opacity );
}

void ST1AssetView::ProcessQueriedItems( const double TickStartTime )
{
	const bool bFlushFullBuffer = TickStartTime < 0;

	bool ListNeedsRefresh = false;
	int32 AssetIndex = 0;
	for ( AssetIndex = QueriedAssetItems.Num() - 1; AssetIndex >= 0 ; AssetIndex--)
	{
		if ( !OnShouldFilterAsset.Execute( QueriedAssetItems[AssetIndex] ) )
		{
			AssetItems.Add( QueriedAssetItems[AssetIndex] );

			if ( !IsFrontendFilterActive() || PassesCurrentFrontendFilter(QueriedAssetItems[AssetIndex]))
			{
				const FAssetData& AssetData = QueriedAssetItems[AssetIndex];
				FilteredAssetItems.Add(MakeShareable(new FT1AssetViewAsset(AssetData)));
				ListNeedsRefresh = true;
				bPendingSortFilteredItems = true;
			}
		}

		// Check to see if we have run out of time in this tick
		if ( !bFlushFullBuffer && (FPlatformTime::Seconds() - TickStartTime) > MaxSecondsPerFrame)
		{
			break;
		}
	}

	// Trim the results array
	if (AssetIndex > 0)
	{
		QueriedAssetItems.RemoveAt( AssetIndex, QueriedAssetItems.Num() - AssetIndex );
	}
	else
	{
		QueriedAssetItems.Empty();
	}

	if ( ListNeedsRefresh )
	{
		RefreshList();
	}
}

void ST1AssetView::OnDragLeave( const FDragDropEvent& DragDropEvent )
{
	TSharedPtr< FAssetDragDropOp > AssetDragDropOp = DragDropEvent.GetOperationAs< FAssetDragDropOp >();
	if( AssetDragDropOp.IsValid() )
	{
		AssetDragDropOp->ResetToDefaultToolTip();
		return;
	}

	TSharedPtr<FDragDropOperation> DragDropOp = DragDropEvent.GetOperation();
	if (DragDropOp.IsValid())
	{
		// Do we have a custom handler for this drag event?
		T1ContentBrowserEditor& ContentBrowserModule = FModuleManager::GetModuleChecked<T1ContentBrowserEditor>("T1ContentBrowserEditor");
		const TArray<FT1AssetViewDragAndDropExtender>& AssetViewDragAndDropExtenders = ContentBrowserModule.GetAssetViewDragAndDropExtenders();
		for (const auto& AssetViewDragAndDropExtender : AssetViewDragAndDropExtenders)
		{
			if (AssetViewDragAndDropExtender.OnDragLeaveDelegate.IsBound() && AssetViewDragAndDropExtender.OnDragLeaveDelegate.Execute(FT1AssetViewDragAndDropExtender::FPayload(DragDropOp, T1SourcesData.PackagePaths, T1SourcesData.Collections)))
			{
				return;
			}
		}
	}
}

FReply ST1AssetView::OnDragOver( const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent )
{
	TSharedPtr<FDragDropOperation> DragDropOp = DragDropEvent.GetOperation();
	if (DragDropOp.IsValid())
	{
		// Do we have a custom handler for this drag event?
		T1ContentBrowserEditor& ContentBrowserModule = FModuleManager::GetModuleChecked<T1ContentBrowserEditor>("T1ContentBrowserEditor");
		const TArray<FT1AssetViewDragAndDropExtender>& AssetViewDragAndDropExtenders = ContentBrowserModule.GetAssetViewDragAndDropExtenders();
		for (const auto& AssetViewDragAndDropExtender : AssetViewDragAndDropExtenders)
		{
			if (AssetViewDragAndDropExtender.OnDragOverDelegate.IsBound() && AssetViewDragAndDropExtender.OnDragOverDelegate.Execute(FT1AssetViewDragAndDropExtender::FPayload(DragDropOp, T1SourcesData.PackagePaths, T1SourcesData.Collections)))
			{
				return FReply::Handled();
			}
		}
	}

	if (T1SourcesData.HasPackagePaths())
	{
		// Note: We don't test IsAssetPathSelected here as we need to prevent dropping assets on class paths
		const FString DestPath = T1SourcesData.PackagePaths[0].ToString();

		bool bUnused = false;
		T1DragDropHandler::ValidateDragDropOnAssetFolder(MyGeometry, DragDropEvent, DestPath, bUnused);
		return FReply::Handled();
	}
	else if (HasSingleCollectionSource())
	{
		TArray< FAssetData > AssetDatas = AssetUtil::ExtractAssetDataFromDrag(DragDropEvent);

		if (AssetDatas.Num() > 0)
		{
			TSharedPtr<FAssetDragDropOp> AssetDragDropOp = DragDropEvent.GetOperationAs< FAssetDragDropOp >();
			if (AssetDragDropOp.IsValid())
			{
				TArray< FName > ObjectPaths;
				FCollectionManagerModule& CollectionManagerModule = FCollectionManagerModule::GetModule();
				const FCollectionNameType& Collection = T1SourcesData.Collections[0];
				CollectionManagerModule.Get().GetObjectsInCollection(Collection.Name, Collection.Type, ObjectPaths);

				bool IsValidDrop = false;
				for (const auto& AssetData : AssetDatas)
				{
					if (AssetData.GetClass()->IsChildOf(UClass::StaticClass()))
					{
						continue;
					}

					if (!ObjectPaths.Contains(AssetData.ObjectPath))
					{
						IsValidDrop = true;
						break;
					}
				}

				if (IsValidDrop)
				{
					AssetDragDropOp->SetToolTip(NSLOCTEXT("AssetView", "OnDragOverCollection", "Add to Collection"), FEditorStyle::GetBrush(TEXT("Graph.ConnectorFeedback.OK")));
				}
			}

			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}

FReply ST1AssetView::OnDrop( const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent )
{
	TSharedPtr<FDragDropOperation> DragDropOp = DragDropEvent.GetOperation();
	if (DragDropOp.IsValid())
	{
		// Do we have a custom handler for this drag event?
		T1ContentBrowserEditor& ContentBrowserModule = FModuleManager::GetModuleChecked<T1ContentBrowserEditor>("T1ContentBrowserEditor");
		const TArray<FT1AssetViewDragAndDropExtender>& AssetViewDragAndDropExtenders = ContentBrowserModule.GetAssetViewDragAndDropExtenders();
		for (const auto& AssetViewDragAndDropExtender : AssetViewDragAndDropExtenders)
		{
			if (AssetViewDragAndDropExtender.OnDropDelegate.IsBound() && AssetViewDragAndDropExtender.OnDropDelegate.Execute(FT1AssetViewDragAndDropExtender::FPayload(DragDropOp, T1SourcesData.PackagePaths, T1SourcesData.Collections)))
			{
				return FReply::Handled();
			}
		}
	}

	if (T1SourcesData.HasPackagePaths())
	{
		// Note: We don't test IsAssetPathSelected here as we need to prevent dropping assets on class paths
		const FString DestPath = T1SourcesData.PackagePaths[0].ToString();

		bool bUnused = false;
		if (T1DragDropHandler::ValidateDragDropOnAssetFolder(MyGeometry, DragDropEvent, DestPath, bUnused))
		{
			// Handle drag drop for import
			TSharedPtr<FExternalDragOperation> ExternalDragDropOp = DragDropEvent.GetOperationAs<FExternalDragOperation>();
			if (ExternalDragDropOp.IsValid())
			{
				if (ExternalDragDropOp->HasFiles())
				{
					TArray<FString> ImportFiles;
					TMap<FString, UObject*> ReimportFiles;
					FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
					FString RootDestinationPath = T1SourcesData.PackagePaths[0].ToString();
					TArray<TPair<FString, FString>> FilesAndDestinations;
					const TArray<FString>& DragFiles = ExternalDragDropOp->GetFiles();
					AssetToolsModule.Get().ExpandDirectories(DragFiles, RootDestinationPath, FilesAndDestinations);

					TArray<int32> ReImportIndexes;
					for (int32 FileIdx = 0; FileIdx < FilesAndDestinations.Num(); ++FileIdx)
					{
						const FString& Filename = FilesAndDestinations[FileIdx].Key;
						const FString& DestinationPath = FilesAndDestinations[FileIdx].Value;
						FString Name = ObjectTools::SanitizeObjectName(FPaths::GetBaseFilename(Filename));
						FString PackageName = ObjectTools::SanitizeInvalidChars(DestinationPath + TEXT("/") + Name, INVALID_LONGPACKAGE_CHARACTERS);


						// We can not create assets that share the name of a map file in the same location
						if (FEditorFileUtils::IsMapPackageAsset(PackageName))
						{
							//The error message will be log in the import process
							ImportFiles.Add(Filename);
							continue;
						}
						//Check if package exist in memory
						UPackage* Pkg = FindPackage(nullptr, *PackageName);
						bool IsPkgExist = Pkg != nullptr;
						//check if package exist on file
						if (!IsPkgExist && !FPackageName::DoesPackageExist(PackageName))
						{
							ImportFiles.Add(Filename);
							continue;
						}
						if (Pkg == nullptr)
						{
							Pkg = CreatePackage(nullptr, *PackageName);
							if (Pkg == nullptr)
							{
								//Cannot create a package that don't exist on disk or in memory!!!
								//The error message will be log in the import process
								ImportFiles.Add(Filename);
								continue;
							}
						}
						// Make sure the destination package is loaded
						Pkg->FullyLoad();

						// Check for an existing object
						UObject* ExistingObject = StaticFindObject(UObject::StaticClass(), Pkg, *Name);
						if (ExistingObject != nullptr)
						{
							ReimportFiles.Add(Filename, ExistingObject);
							ReImportIndexes.Add(FileIdx);
						}
						else
						{
							ImportFiles.Add(Filename);
						}
					}
					//Reimport
					for (auto kvp : ReimportFiles)
					{
						FReimportManager::Instance()->Reimport(kvp.Value, false, true, kvp.Key);
					}
					//Import
					if (ImportFiles.Num() > 0)
					{
						//Remove it in reverse so the smaller index are still valid
						for (int32 IndexToRemove = ReImportIndexes.Num() - 1; IndexToRemove >= 0; --IndexToRemove)
						{
							FilesAndDestinations.RemoveAt(ReImportIndexes[IndexToRemove]);
						}
						AssetToolsModule.Get().ImportAssets(ImportFiles, T1SourcesData.PackagePaths[0].ToString(), nullptr, true, &FilesAndDestinations);
					}
				}
			}

			TSharedPtr<FAssetDragDropOp> AssetDragDropOp = DragDropEvent.GetOperationAs<FAssetDragDropOp>();
			if (AssetDragDropOp.IsValid())
			{
				OnAssetsOrPathsDragDropped(AssetDragDropOp->GetAssets(), AssetDragDropOp->GetAssetPaths(), DestPath);
			}
		}
		return FReply::Handled();
	}
	else if (HasSingleCollectionSource())
	{
		TArray<FAssetData> SelectedAssetDatas = AssetUtil::ExtractAssetDataFromDrag(DragDropEvent);

		if (SelectedAssetDatas.Num() > 0)
		{
			TArray<FName> ObjectPaths;
			for (const auto& AssetData : SelectedAssetDatas)
			{
				if (!AssetData.GetClass()->IsChildOf(UClass::StaticClass()))
				{
					ObjectPaths.Add(AssetData.ObjectPath);
				}
			}

			if (ObjectPaths.Num() > 0)
			{
				FCollectionManagerModule& CollectionManagerModule = FCollectionManagerModule::GetModule();
				const FCollectionNameType& Collection = T1SourcesData.Collections[0];
				CollectionManagerModule.Get().AddToCollection(Collection.Name, Collection.Type, ObjectPaths);
			}

			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}

FReply ST1AssetView::OnKeyChar( const FGeometry& MyGeometry,const FCharacterEvent& InCharacterEvent )
{
	const bool bIsControlOrCommandDown = InCharacterEvent.IsControlDown() || InCharacterEvent.IsCommandDown();
	
	const bool bTestOnly = false;
	if(HandleQuickJumpKeyDown(InCharacterEvent.GetCharacter(), bIsControlOrCommandDown, InCharacterEvent.IsAltDown(), bTestOnly).IsEventHandled())
	{
		return FReply::Handled();
	}

	// If the user pressed a key we couldn't handle, reset the quick-jump search
	ResetQuickJump();

	return FReply::Unhandled();
}

static bool IsValidObjectPath(const FString& Path)
{
	int32 NameStartIndex = INDEX_NONE;
	Path.FindChar(TCHAR('\''), NameStartIndex);
	if (NameStartIndex != INDEX_NONE)
	{
		int32 NameEndIndex = INDEX_NONE;
		Path.FindLastChar(TCHAR('\''), NameEndIndex);
		if (NameEndIndex > NameStartIndex)
		{
			const FString ClassName = Path.Left(NameStartIndex);
			const FString PathName = Path.Mid(NameStartIndex + 1, NameEndIndex - NameStartIndex - 1);

			UClass* Class = FindObject<UClass>(ANY_PACKAGE, *ClassName, true);
			if (Class)
			{
				return FPackageName::IsValidLongPackageName(FPackageName::ObjectPathToPackageName(PathName));
			}
		}
	}

	return false;
}

static bool ContainsT3D(const FString& ClipboardText)
{
	return (ClipboardText.StartsWith(TEXT("Begin Object")) && ClipboardText.EndsWith(TEXT("End Object")))
		|| (ClipboardText.StartsWith(TEXT("Begin Map")) && ClipboardText.EndsWith(TEXT("End Map")));
}

FReply ST1AssetView::OnKeyDown( const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent )
{
	const bool bIsControlOrCommandDown = InKeyEvent.IsControlDown() || InKeyEvent.IsCommandDown();
	
	if (bIsControlOrCommandDown && InKeyEvent.GetCharacter() == 'V' && IsAssetPathSelected())
	{
		FString AssetPaths;
		TArray<FString> AssetPathsSplit;

		// Get the copied asset paths
		FPlatformApplicationMisc::ClipboardPaste(AssetPaths);

		// Make sure the clipboard does not contain T3D
		AssetPaths.TrimEndInline();
		if (!ContainsT3D(AssetPaths))
		{
			AssetPaths.ParseIntoArrayLines(AssetPathsSplit);

			// Get assets and copy them
			TArray<UObject*> AssetsToCopy;
			for (const FString& AssetPath : AssetPathsSplit)
			{
				// Validate string
				if (IsValidObjectPath(AssetPath))
				{
					UObject* ObjectToCopy = LoadObject<UObject>(nullptr, *AssetPath);
					if (ObjectToCopy && !ObjectToCopy->IsA(UClass::StaticClass()))
					{
						AssetsToCopy.Add(ObjectToCopy);
					}
				}
			}

			if (AssetsToCopy.Num())
			{
				T1ContentBrowserUtils::CopyAssets(AssetsToCopy, T1SourcesData.PackagePaths[0].ToString());
			}
		}

		return FReply::Handled();
	}
	// Swallow the key-presses used by the quick-jump in OnKeyChar to avoid other things (such as the viewport commands) getting them instead
	// eg) Pressing "W" without this would set the viewport to "translate" mode
	else if(HandleQuickJumpKeyDown(InKeyEvent.GetCharacter(), bIsControlOrCommandDown, InKeyEvent.IsAltDown(), /*bTestOnly*/true).IsEventHandled())
	{
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply ST1AssetView::OnMouseWheel( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent )
{
	if( MouseEvent.IsControlDown() )
	{
		const float DesiredScale = FMath::Clamp<float>(GetThumbnailScale() + ( MouseEvent.GetWheelDelta() * 0.05f ), 0.0f, 1.0f);
		if ( DesiredScale != GetThumbnailScale() )
		{
			SetThumbnailScale( DesiredScale );
		}		
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void ST1AssetView::OnFocusChanging( const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	ResetQuickJump();
}

TSharedRef<ST1AssetTileView> ST1AssetView::CreateTileView()
{
	return SNew(ST1AssetTileView)
		.SelectionMode( SelectionMode )
		.ListItemsSource(&FilteredAssetItems)
		.OnGenerateTile(this, &ST1AssetView::MakeTileViewWidget)
		.OnItemScrolledIntoView(this, &ST1AssetView::ItemScrolledIntoView)
		.OnContextMenuOpening(this, &ST1AssetView::OnGetContextMenuContent)
		.OnMouseButtonDoubleClick(this, &ST1AssetView::OnListMouseButtonDoubleClick)
		.OnSelectionChanged(this, &ST1AssetView::AssetSelectionChanged)
		.ItemHeight(this, &ST1AssetView::GetTileViewItemHeight)
		.ItemWidth(this, &ST1AssetView::GetTileViewItemWidth);
}

TSharedRef<ST1AssetListView> ST1AssetView::CreateListView()
{
	return SNew(ST1AssetListView)
		.SelectionMode( SelectionMode )
		.ListItemsSource(&FilteredAssetItems)
		.OnGenerateRow(this, &ST1AssetView::MakeListViewWidget)
		.OnItemScrolledIntoView(this, &ST1AssetView::ItemScrolledIntoView)
		.OnContextMenuOpening(this, &ST1AssetView::OnGetContextMenuContent)
		.OnMouseButtonDoubleClick(this, &ST1AssetView::OnListMouseButtonDoubleClick)
		.OnSelectionChanged(this, &ST1AssetView::AssetSelectionChanged)
		.ItemHeight(this, &ST1AssetView::GetListViewItemHeight);
}

TSharedRef<ST1AssetColumnView> ST1AssetView::CreateColumnView()
{
	TSharedPtr<ST1AssetColumnView> NewColumnView = SNew(ST1AssetColumnView)
		.SelectionMode( SelectionMode )
		.ListItemsSource(&FilteredAssetItems)
		.OnGenerateRow(this, &ST1AssetView::MakeColumnViewWidget)
		.OnItemScrolledIntoView(this, &ST1AssetView::ItemScrolledIntoView)
		.OnContextMenuOpening(this, &ST1AssetView::OnGetContextMenuContent)
		.OnMouseButtonDoubleClick(this, &ST1AssetView::OnListMouseButtonDoubleClick)
		.OnSelectionChanged(this, &ST1AssetView::AssetSelectionChanged)
		.Visibility(this, &ST1AssetView::GetColumnViewVisibility)
		.HeaderRow
		(
			SNew(SHeaderRow)
			.ResizeMode(ESplitterResizeMode::FixedSize)
			+ SHeaderRow::Column(SortManager.NameColumnId)
			.FillWidth(300)
			.SortMode( TAttribute< EColumnSortMode::Type >::Create( TAttribute< EColumnSortMode::Type >::FGetter::CreateSP( this, &ST1AssetView::GetColumnSortMode, SortManager.NameColumnId ) ) )
			.SortPriority(TAttribute< EColumnSortPriority::Type >::Create(TAttribute< EColumnSortPriority::Type >::FGetter::CreateSP(this, &ST1AssetView::GetColumnSortPriority, SortManager.NameColumnId)))
			.OnSort( FOnSortModeChanged::CreateSP( this, &ST1AssetView::OnSortColumnHeader ) )
			.DefaultLabel( LOCTEXT("Column_Name", "Name") )
			.ShouldGenerateWidget(TAttribute<bool>::Create(TAttribute<bool>::FGetter::CreateSP(this, &ST1AssetView::ShouldColumnGenerateWidget, SortManager.NameColumnId.ToString())))
			.MenuContent()
			[
				CreateRowHeaderMenuContent(SortManager.NameColumnId.ToString())
			]
		);

	NewColumnView->GetHeaderRow()->SetOnGetMaxRowSizeForColumn(FOnGetMaxRowSizeForColumn::CreateRaw(NewColumnView.Get(), &ST1AssetColumnView::GetMaxRowSizeForColumn));


	NumVisibleColumns = HiddenColumnNames.Contains(SortManager.NameColumnId.ToString()) ? 0 : 1;

	if(bShowTypeInColumnView)
	{
		NewColumnView->GetHeaderRow()->AddColumn(
				SHeaderRow::Column(SortManager.ClassColumnId)
				.FillWidth(160)
				.SortMode(TAttribute< EColumnSortMode::Type >::Create(TAttribute< EColumnSortMode::Type >::FGetter::CreateSP(this, &ST1AssetView::GetColumnSortMode, SortManager.ClassColumnId)))
				.SortPriority(TAttribute< EColumnSortPriority::Type >::Create(TAttribute< EColumnSortPriority::Type >::FGetter::CreateSP(this, &ST1AssetView::GetColumnSortPriority, SortManager.ClassColumnId)))
				.OnSort(FOnSortModeChanged::CreateSP(this, &ST1AssetView::OnSortColumnHeader))
				.DefaultLabel(LOCTEXT("Column_Class", "Type"))
				.ShouldGenerateWidget(TAttribute<bool>::Create(TAttribute<bool>::FGetter::CreateSP(this, &ST1AssetView::ShouldColumnGenerateWidget, SortManager.ClassColumnId.ToString())))
				.MenuContent()
				[
					CreateRowHeaderMenuContent(SortManager.ClassColumnId.ToString())
				]
			);

		NumVisibleColumns += HiddenColumnNames.Contains(SortManager.ClassColumnId.ToString()) ? 0 : 1;
	}


	if (bShowPathInColumnView)
	{
		NewColumnView->GetHeaderRow()->AddColumn(
				SHeaderRow::Column(SortManager.PathColumnId)
				.FillWidth(160)
				.SortMode(TAttribute< EColumnSortMode::Type >::Create(TAttribute< EColumnSortMode::Type >::FGetter::CreateSP(this, &ST1AssetView::GetColumnSortMode, SortManager.PathColumnId)))
				.SortPriority(TAttribute< EColumnSortPriority::Type >::Create(TAttribute< EColumnSortPriority::Type >::FGetter::CreateSP(this, &ST1AssetView::GetColumnSortPriority, SortManager.PathColumnId)))
				.OnSort(FOnSortModeChanged::CreateSP(this, &ST1AssetView::OnSortColumnHeader))
				.DefaultLabel(LOCTEXT("Column_Path", "Path"))
				.ShouldGenerateWidget(TAttribute<bool>::Create(TAttribute<bool>::FGetter::CreateSP(this, &ST1AssetView::ShouldColumnGenerateWidget, SortManager.PathColumnId.ToString())))
				.MenuContent()
				[
					CreateRowHeaderMenuContent(SortManager.PathColumnId.ToString())
				]
			);


		NumVisibleColumns += HiddenColumnNames.Contains(SortManager.PathColumnId.ToString()) ? 0 : 1;
	}

	return NewColumnView.ToSharedRef();
}

bool ST1AssetView::IsValidSearchToken(const FString& Token) const
{
	if ( Token.Len() == 0 )
	{
		return false;
	}

	// A token may not be only apostrophe only, or it will match every asset because the text filter compares against the pattern Class'ObjectPath'
	if ( Token.Len() == 1 && Token[0] == '\'' )
	{
		return false;
	}

	return true;
}

void ST1AssetView::RefreshSourceItems()
{
	// Load the asset registry module
	static const FName AssetRegistryName(TEXT("AssetRegistry"));
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryName);

	RecentlyLoadedOrChangedAssets.Empty();
	RecentlyAddedAssets.Empty();
	FilteredRecentlyAddedAssets.Empty();
	QueriedAssetItems.Empty();
	AssetItems.Empty();
	FilteredAssetItems.Empty();
	VisibleItems.Empty();
	RelevantThumbnails.Empty();
	Folders.Empty();

	TArray<FAssetData>& Items = OnShouldFilterAsset.IsBound() ? QueriedAssetItems : AssetItems;

	const bool bShowAll = T1SourcesData.IsEmpty() && BackendFilter.IsEmpty();

	bool bShowClasses = false;
	TArray<FName> ClassPathsToShow;

	if ( bShowAll )
	{
		AssetRegistryModule.Get().GetAllAssets(Items);
		bShowClasses = IsShowingCppContent();
		bWereItemsRecursivelyFiltered = true;
	}
	else
	{
		// Assemble the filter using the current sources
		// force recursion when the user is searching
		const bool bRecurse = ShouldFilterRecursively();
		const bool bUsingFolders = IsShowingFolders();
		const bool bIsDynamicCollection = T1SourcesData.IsDynamicCollection();
		FARFilter Filter = T1SourcesData.MakeFilter(bRecurse, bUsingFolders);

		// Add the backend filters from the filter list
		Filter.Append(BackendFilter);

		bWereItemsRecursivelyFiltered = bRecurse;

		// Move any class paths into their own array
		Filter.PackagePaths.RemoveAll([&ClassPathsToShow](const FName& PackagePath) -> bool
		{
			if(T1ContentBrowserUtils::IsClassPath(PackagePath.ToString()))
			{
				ClassPathsToShow.Add(PackagePath);
				return true;
			}
			return false;
		});

		// Only show classes if we have class paths, and the filter allows classes to be shown
		const bool bFilterAllowsClasses = IsShowingCppContent() && (Filter.ClassNames.Num() == 0 || Filter.ClassNames.Contains(NAME_Class));
		bShowClasses = (ClassPathsToShow.Num() > 0 || bIsDynamicCollection) && bFilterAllowsClasses;

		if ( T1SourcesData.HasCollections() && Filter.ObjectPaths.Num() == 0 && !bIsDynamicCollection )
		{
			// This is an empty collection, no asset will pass the check
		}
		else if ( ClassPathsToShow.Num() > 0 && Filter.PackagePaths.Num() == 0 )
		{
			// Only class paths are selected, no asset will pass the check
		}
		else
		{
			// Add assets found in the asset registry
			AssetRegistryModule.Get().GetAssets(Filter, Items);
		}

		if ( bFilterAllowsClasses )
		{
			FCollectionManagerModule& CollectionManagerModule = FCollectionManagerModule::GetModule();

			// Include objects from child collections if we're recursing
			const ECollectionRecursionFlags::Flags CollectionRecursionMode = (Filter.bRecursivePaths) ? ECollectionRecursionFlags::SelfAndChildren : ECollectionRecursionFlags::Self;

			TArray< FName > ClassPaths;
			for (const FCollectionNameType& Collection : T1SourcesData.Collections)
			{
				CollectionManagerModule.Get().GetClassesInCollection( Collection.Name, Collection.Type, ClassPaths, CollectionRecursionMode );
			}

			for (const FName& ClassPath : ClassPaths)
			{
				UClass* Class = FindObject<UClass>(ANY_PACKAGE, *ClassPath.ToString());

				if ( Class != NULL )
				{
					Items.Add( Class );
				}
			}
		}

		// Add any custom assets 
		if (OnGetCustomSourceAssets.IsBound())
		{
			OnGetCustomSourceAssets.Execute(Filter, Items);
		}
	}

	// If we are showing classes in the asset list...
	if (bShowClasses)
	{
		// Load the native class hierarchy
		TSharedRef<FT1NativeClassHierarchy> NativeClassHierarchy = FT1ContentBrowserSingleton::Get().GetNativeClassHierarchy();

		FT1NativeClassHierarchyFilter ClassFilter;
		ClassFilter.ClassPaths = ClassPathsToShow;
		ClassFilter.bRecursivePaths = ShouldFilterRecursively() || !IsShowingFolders() || !ClassPathsToShow.Num();

		// Find all the classes that match the current criteria
		TArray<UClass*> MatchingClasses;
		NativeClassHierarchy->GetMatchingClasses(ClassFilter, MatchingClasses);
		for(UClass* CurrentClass : MatchingClasses)
		{
			Items.Add(FAssetData(CurrentClass));
		}
	}

	// Remove any assets that should be filtered out any redirectors and non-assets
	const bool bDisplayEngine = IsShowingEngineContent();
	const bool bDisplayPlugins = IsShowingPluginContent();
	const bool bDisplayL10N = IsShowingLocalizedContent();
	const TArray<TSharedRef<IPlugin>> Plugins = IPluginManager::Get().GetEnabledPluginsWithContent();
	for (int32 AssetIdx = Items.Num() - 1; AssetIdx >= 0; --AssetIdx)
	{
		const FAssetData& Item = Items[AssetIdx];
		const FString PackagePath = Item.PackagePath.ToString();
		// Do not show redirectors if they are not the main asset in the uasset file.
		const bool IsMainlyARedirector = Item.AssetClass == UObjectRedirector::StaticClass()->GetFName() && !Item.IsUAsset();
		// If this is an engine folder, and we don't want to show them, remove
		const bool IsHiddenEngineFolder = !bDisplayEngine && T1ContentBrowserUtils::IsEngineFolder(PackagePath);
		// If this is a plugin folder (engine or project), and we don't want to show them, remove
		bool IsHiddenPluginFolder = false;
		if (!bDisplayPlugins || !bDisplayEngine)
		{
			EPluginLoadedFrom PluginSource;
			const bool bIsPluginFolder = T1ContentBrowserUtils::IsPluginFolder(PackagePath, Plugins, &PluginSource);
			IsHiddenPluginFolder = bIsPluginFolder && (!bDisplayPlugins || (!bDisplayEngine && PluginSource == EPluginLoadedFrom::Engine));
		}
		// Do not show localized content folders.
		const bool IsTheHiddenLocalizedContentFolder = !bDisplayL10N && T1ContentBrowserUtils::IsLocalizationFolder(PackagePath);

		const bool ShouldFilterOut = IsMainlyARedirector || IsHiddenEngineFolder || IsHiddenPluginFolder || IsTheHiddenLocalizedContentFolder;
		if (ShouldFilterOut)
		{
			Items.RemoveAtSwap(AssetIdx);
		}
	}
}

bool ST1AssetView::ShouldFilterRecursively() const
{
	// Quick check for conditions which force recursive filtering
	if (bUserSearching)
	{
		return true;
	}

	// In some cases we want to not filter recursively even if we have a backend filter (e.g. the open level window)
	// Most of the time, bFilterRecursivelyWithBackendFilter is true
	if ( bFilterRecursivelyWithBackendFilter && !BackendFilter.IsEmpty() )
	{
		return true;
	}

	// Otherwise, check if there are any non-inverse frontend filters selected
	if (FrontendFilters.IsValid())
	{
		for (int32 FilterIndex = 0; FilterIndex < FrontendFilters->Num(); ++FilterIndex)
		{
			const auto* Filter = static_cast<FT1FrontendFilter*>(FrontendFilters->GetFilterAtIndex(FilterIndex).Get());
			if (Filter)
			{
				if (!Filter->IsInverseFilter())
				{
					return true;
				}
			}
		}
	}

	// No filters, do not override folder view with recursive filtering
	return false;
}

void ST1AssetView::RefreshFilteredItems()
{
	//Build up a map of the existing AssetItems so we can preserve them while filtering
	TMap< FName, TSharedPtr< FT1AssetViewAsset > > ItemToObjectPath;
	for (int Index = 0; Index < FilteredAssetItems.Num(); Index++)
	{
		if(FilteredAssetItems[Index].IsValid() && FilteredAssetItems[Index]->GetType() != EAssetItemType::Folder)
		{
			TSharedPtr<FT1AssetViewAsset> Item = StaticCastSharedPtr<FT1AssetViewAsset>(FilteredAssetItems[Index]);

			// Clear custom column data
			Item->CustomColumnData.Reset();
			Item->CustomColumnDisplayText.Reset();

			ItemToObjectPath.Add( Item->Data.ObjectPath, Item );
		}
	}

	// Empty all the filtered lists
	FilteredAssetItems.Empty();
	VisibleItems.Empty();
	RelevantThumbnails.Empty();
	Folders.Empty();

	// true if the results from the asset registry query are filtered further by the content browser
	const bool bIsFrontendFilterActive = IsFrontendFilterActive();

	// true if we are looking at columns so we need to determine the majority asset type
	const bool bGatherAssetTypeCount = CurrentViewType == ET1AssetViewType::Column;
	TMap<FName, int32> AssetTypeCount;

	if ( bIsFrontendFilterActive && FrontendFilters.IsValid() )
	{
		const bool bRecurse = ShouldFilterRecursively();
		const bool bUsingFolders = IsShowingFolders();
		FARFilter CombinedFilter = T1SourcesData.MakeFilter(bRecurse, bUsingFolders);
		CombinedFilter.Append(BackendFilter);

		// Let the frontend filters know the currently used filter in case it is necessary to conditionally filter based on path or class filters
		for ( int32 FilterIdx = 0; FilterIdx < FrontendFilters->Num(); ++FilterIdx )
		{
			// There are only FFrontendFilters in this collection
			const TSharedPtr<FT1FrontendFilter>& Filter = StaticCastSharedPtr<FT1FrontendFilter>( FrontendFilters->GetFilterAtIndex(FilterIdx) );
			if ( Filter.IsValid() )
			{
				Filter->SetCurrentFilter(CombinedFilter);
			}
		}
	}

	if ( bIsFrontendFilterActive && bGatherAssetTypeCount )
	{
		// Check the frontend filter for every asset and keep track of how many assets were found of each type
		for (int32 AssetIdx = 0; AssetIdx < AssetItems.Num(); ++AssetIdx)
		{
			const FAssetData& AssetData = AssetItems[AssetIdx];
			if ( PassesCurrentFrontendFilter(AssetData) )
			{
				const TSharedPtr< FT1AssetViewAsset >* AssetItem = ItemToObjectPath.Find( AssetData.ObjectPath );

				if ( AssetItem != NULL )
				{
					FilteredAssetItems.Add(*AssetItem);
				}
				else
				{
					FilteredAssetItems.Add(MakeShareable(new FT1AssetViewAsset(AssetData)));
				}

				int32* TypeCount = AssetTypeCount.Find(AssetData.AssetClass);
				if ( TypeCount )
				{
					(*TypeCount)++;
				}
				else
				{
					AssetTypeCount.Add(AssetData.AssetClass, 1);
				}
			}
		}
	}
	else if ( bIsFrontendFilterActive && !bGatherAssetTypeCount )
	{
		// Check the frontend filter for every asset and don't worry about asset type counts
		for (int32 AssetIdx = 0; AssetIdx < AssetItems.Num(); ++AssetIdx)
		{
			const FAssetData& AssetData = AssetItems[AssetIdx];
			if ( PassesCurrentFrontendFilter(AssetData) )
			{
				const TSharedPtr< FT1AssetViewAsset >* AssetItem = ItemToObjectPath.Find( AssetData.ObjectPath );

				if ( AssetItem != NULL )
				{
					FilteredAssetItems.Add(*AssetItem);
				}
				else
				{
					FilteredAssetItems.Add(MakeShareable(new FT1AssetViewAsset(AssetData)));
				}
			}
		}
	}
	else if ( !bIsFrontendFilterActive && bGatherAssetTypeCount )
	{
		// Don't need to check the frontend filter for every asset but keep track of how many assets were found of each type
		for (int32 AssetIdx = 0; AssetIdx < AssetItems.Num(); ++AssetIdx)
		{
			const FAssetData& AssetData = AssetItems[AssetIdx];
			const TSharedPtr< FT1AssetViewAsset >* AssetItem = ItemToObjectPath.Find( AssetData.ObjectPath );

			if ( AssetItem != NULL )
			{
				FilteredAssetItems.Add(*AssetItem);
			}
			else
			{
				FilteredAssetItems.Add(MakeShareable(new FT1AssetViewAsset(AssetData)));
			}

			int32* TypeCount = AssetTypeCount.Find(AssetData.AssetClass);
			if ( TypeCount )
			{
				(*TypeCount)++;
			}
			else
			{
				AssetTypeCount.Add(AssetData.AssetClass, 1);
			}
		}
	}
	else if ( !bIsFrontendFilterActive && !bGatherAssetTypeCount )
	{
		// Don't check the frontend filter and don't count the number of assets of each type. Just add all assets.
		for (int32 AssetIdx = 0; AssetIdx < AssetItems.Num(); ++AssetIdx)
		{
			const FAssetData& AssetData = AssetItems[AssetIdx];
			const TSharedPtr< FT1AssetViewAsset >* AssetItem = ItemToObjectPath.Find( AssetData.ObjectPath );

			if ( AssetItem != NULL )
			{
				FilteredAssetItems.Add(*AssetItem);
			}
			else
			{
				FilteredAssetItems.Add(MakeShareable(new FT1AssetViewAsset(AssetData)));
			}
		}
	}
	else
	{
		// The above cases should handle all combinations of bIsFrontendFilterActive and bGatherAssetTypeCount
		ensure(0);
	}

	if ( bGatherAssetTypeCount )
	{
		int32 HighestCount = 0;
		FName HighestType;
		for ( auto TypeIt = AssetTypeCount.CreateConstIterator(); TypeIt; ++TypeIt )
		{
			if ( TypeIt.Value() > HighestCount )
			{
				HighestType = TypeIt.Key();
				HighestCount = TypeIt.Value();
			}
		}

		SetMajorityAssetType(HighestType);
	}
}

void ST1AssetView::RefreshFolders()
{
	if(!IsShowingFolders() || ShouldFilterRecursively())
	{
		return;
	}
	
	// Split the selected paths into asset and class paths
	TArray<FName> AssetPathsToShow;
	TArray<FName> ClassPathsToShow;
	for(const FName& PackagePath : T1SourcesData.PackagePaths)
	{
		if(T1ContentBrowserUtils::IsClassPath(PackagePath.ToString()))
		{
			ClassPathsToShow.Add(PackagePath);
		}
		else
		{
			AssetPathsToShow.Add(PackagePath);
		}
	}

	TArray<FString> FoldersToAdd;

	TSharedRef<FT1EmptyFolderVisibilityManager> EmptyFolderVisibilityManager = FT1ContentBrowserSingleton::Get().GetEmptyFolderVisibilityManager();

	const bool bDisplayEmpty = IsShowingEmptyFolders();
	const bool bDisplayDev = IsShowingDevelopersContent();
	const bool bDisplayL10N = IsShowingLocalizedContent();
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	{
		TArray<FString> SubPaths;
		for(const FName& PackagePath : AssetPathsToShow)
		{
			SubPaths.Reset();
			AssetRegistryModule.Get().GetSubPaths(PackagePath.ToString(), SubPaths, false);

			for(const FString& SubPath : SubPaths)
			{
				if (!bDisplayEmpty && !EmptyFolderVisibilityManager->ShouldShowPath(SubPath))
				{
					continue;
				}

				if (!bDisplayDev && T1ContentBrowserUtils::IsDevelopersFolder(SubPath))
				{
					continue;
				}

				if (!bDisplayL10N && T1ContentBrowserUtils::IsLocalizationFolder(SubPath))
				{
					continue;
				}

				if(!Folders.Contains(SubPath))
				{
					FoldersToAdd.Add(SubPath);
				}
			}
		}
	}

	// If we are showing classes in the asset list then we need to show their folders too
	if(IsShowingCppContent() && ClassPathsToShow.Num() > 0)
	{
		// Load the native class hierarchy
		TSharedRef<FT1NativeClassHierarchy> NativeClassHierarchy = FT1ContentBrowserSingleton::Get().GetNativeClassHierarchy();

		FT1NativeClassHierarchyFilter ClassFilter;
		ClassFilter.ClassPaths = ClassPathsToShow;
		ClassFilter.bRecursivePaths = false;

		// Find all the classes that match the current criteria
		TArray<FString> MatchingFolders;
		NativeClassHierarchy->GetMatchingFolders(ClassFilter, MatchingFolders);
		FoldersToAdd.Append(MatchingFolders);
	}

	// Add folders for any child collections of the currently selected collections
	if(T1SourcesData.HasCollections())
	{
		FCollectionManagerModule& CollectionManagerModule = FCollectionManagerModule::GetModule();
		
		TArray<FCollectionNameType> ChildCollections;
		for(const FCollectionNameType& Collection : T1SourcesData.Collections)
		{
			ChildCollections.Reset();
			CollectionManagerModule.Get().GetChildCollections(Collection.Name, Collection.Type, ChildCollections);

			for(const FCollectionNameType& ChildCollection : ChildCollections)
			{
				// Use "Collections" as the root of the path to avoid this being confused with other asset view folders - see T1ContentBrowserUtils::IsCollectionPath
				FoldersToAdd.Add(FString::Printf(TEXT("/Collections/%s/%s"), ECollectionShareType::ToString(ChildCollection.Type), *ChildCollection.Name.ToString()));
			}
		}
	}

	if(FoldersToAdd.Num() > 0)
	{
		for(const FString& FolderPath : FoldersToAdd)
		{
			FilteredAssetItems.Add(MakeShareable(new FT1AssetViewFolder(FolderPath)));
			Folders.Add(FolderPath);
		}

		RefreshList();
		bPendingSortFilteredItems = true;
	}
}

void ST1AssetView::SetMajorityAssetType(FName NewMajorityAssetType)
{
	auto IsFixedColumn = [this](FName InColumnId)
	{
		const bool bIsFixedNameColumn = InColumnId == SortManager.NameColumnId;
		const bool bIsFixedClassColumn = bShowTypeInColumnView && InColumnId == SortManager.ClassColumnId;
		const bool bIsFixedPathColumn = bShowPathInColumnView && InColumnId == SortManager.PathColumnId;
		return bIsFixedNameColumn || bIsFixedClassColumn || bIsFixedPathColumn;
	};

	if ( NewMajorityAssetType != MajorityAssetType )
	{
		UE_LOG(LogContentBrowser, Verbose, TEXT("The majority of assets in the view are of type: %s"), *NewMajorityAssetType.ToString());

		MajorityAssetType = NewMajorityAssetType;

		TArray<FName> AddedColumns;

		// Since the asset type has changed, remove all columns except name and class
		const TIndirectArray<SHeaderRow::FColumn>& Columns = ColumnView->GetHeaderRow()->GetColumns();

		for ( int32 ColumnIdx = Columns.Num() - 1; ColumnIdx >= 0; --ColumnIdx )
		{
			const FName ColumnId = Columns[ColumnIdx].ColumnId;

			if ( ColumnId != NAME_None && !IsFixedColumn(ColumnId) )
			{
				ColumnView->GetHeaderRow()->RemoveColumn(ColumnId);
			}
		}

		// Keep track of the current column name to see if we need to change it now that columns are being removed
		// Name, Class, and Path are always relevant
		struct FSortOrder
		{
			bool bSortRelevant;
			FName SortColumn;
			FSortOrder(bool bInSortRelevant, const FName& InSortColumn) : bSortRelevant(bInSortRelevant), SortColumn(InSortColumn) {}
		};
		TArray<FSortOrder> CurrentSortOrder;
		for (int32 PriorityIdx = 0; PriorityIdx < EColumnSortPriority::Max; PriorityIdx++)
		{
			const FName SortColumn = SortManager.GetSortColumnId(static_cast<EColumnSortPriority::Type>(PriorityIdx));
			if (SortColumn != NAME_None)
			{
				const bool bSortRelevant = SortColumn == FT1AssetViewSortManager::NameColumnId
					|| SortColumn == FT1AssetViewSortManager::ClassColumnId
					|| SortColumn == FT1AssetViewSortManager::PathColumnId;
				CurrentSortOrder.Add(FSortOrder(bSortRelevant, SortColumn));
			}
		}

		// Add custom columns
		for (const FT1AssetViewCustomColumn& Column : CustomColumns)
		{
			FName TagName = Column.ColumnName;

			if (AddedColumns.Contains(TagName))
			{
				continue;
			}
			AddedColumns.Add(TagName);

			ColumnView->GetHeaderRow()->AddColumn(
				SHeaderRow::Column(TagName)
				.SortMode(TAttribute< EColumnSortMode::Type >::Create(TAttribute< EColumnSortMode::Type >::FGetter::CreateSP(this, &ST1AssetView::GetColumnSortMode, TagName)))
				.SortPriority(TAttribute< EColumnSortPriority::Type >::Create(TAttribute< EColumnSortPriority::Type >::FGetter::CreateSP(this, &ST1AssetView::GetColumnSortPriority, TagName)))
				.OnSort(FOnSortModeChanged::CreateSP(this, &ST1AssetView::OnSortColumnHeader))
				.DefaultLabel(Column.DisplayName)
				.DefaultTooltip(Column.TooltipText)
				.FillWidth(180)
				.ShouldGenerateWidget(TAttribute<bool>::Create(TAttribute<bool>::FGetter::CreateSP(this, &ST1AssetView::ShouldColumnGenerateWidget, TagName.ToString())))
				.MenuContent()
				[
					CreateRowHeaderMenuContent(TagName.ToString())
				]);

			NumVisibleColumns += HiddenColumnNames.Contains(TagName.ToString()) ? 0 : 1;

			// If we found a tag the matches the column we are currently sorting on, there will be no need to change the column
			for (int32 SortIdx = 0; SortIdx < CurrentSortOrder.Num(); SortIdx++)
			{
				if (TagName == CurrentSortOrder[SortIdx].SortColumn)
				{
					CurrentSortOrder[SortIdx].bSortRelevant = true;
				}
			}
		}

		// If we have a new majority type, add the new type's columns
		if ( NewMajorityAssetType != NAME_None )
		{
			// Determine the columns by querying the CDO for the tag map
			UClass* TypeClass = FindObject<UClass>(ANY_PACKAGE, *NewMajorityAssetType.ToString());
			if ( TypeClass )
			{
				UObject* CDO = TypeClass->GetDefaultObject();
				if ( CDO )
				{
					TArray<UObject::FAssetRegistryTag> AssetRegistryTags;
					CDO->GetAssetRegistryTags(AssetRegistryTags);

					// Add a column for every tag that isn't hidden or using a reserved name
					for ( auto TagIt = AssetRegistryTags.CreateConstIterator(); TagIt; ++TagIt )
					{
						if ( TagIt->Type != UObject::FAssetRegistryTag::TT_Hidden )
						{
							const FName TagName = TagIt->Name;

							if (IsFixedColumn(TagName))
							{
								// Reserved name
								continue;
							}

							if ( !OnAssetTagWantsToBeDisplayed.IsBound() || OnAssetTagWantsToBeDisplayed.Execute(NewMajorityAssetType, TagName) )
							{
								if (AddedColumns.Contains(TagName))
								{
									continue;
								}
								AddedColumns.Add(TagName);

								// Get tag metadata
								TMap<FName, UObject::FAssetRegistryTagMetadata> MetadataMap;
								CDO->GetAssetRegistryTagMetadata(MetadataMap);
								const UObject::FAssetRegistryTagMetadata* Metadata = MetadataMap.Find(TagName);

								FText DisplayName;
								if (Metadata != nullptr && !Metadata->DisplayName.IsEmpty())
								{
									DisplayName = Metadata->DisplayName;
								}
								else
								{
									DisplayName = FText::FromName(TagName);
								}

								FText TooltipText;
								if (Metadata != nullptr && !Metadata->TooltipText.IsEmpty())
								{
									TooltipText = Metadata->TooltipText;
								}
								else
								{
									// If the tag name corresponds to a property name, use the property tooltip
									UProperty* Property = FindField<UProperty>(TypeClass, TagName);
									TooltipText = (Property != nullptr) ? Property->GetToolTipText() : FText::FromString(FName::NameToDisplayString(TagName.ToString(), false));
								}

								ColumnView->GetHeaderRow()->AddColumn(
									SHeaderRow::Column(TagName)
									.SortMode(TAttribute< EColumnSortMode::Type >::Create(TAttribute< EColumnSortMode::Type >::FGetter::CreateSP(this, &ST1AssetView::GetColumnSortMode, TagName)))
									.SortPriority(TAttribute< EColumnSortPriority::Type >::Create(TAttribute< EColumnSortPriority::Type >::FGetter::CreateSP(this, &ST1AssetView::GetColumnSortPriority, TagName)))
									.OnSort(FOnSortModeChanged::CreateSP(this, &ST1AssetView::OnSortColumnHeader))
									.DefaultLabel(DisplayName)
									.DefaultTooltip(TooltipText)
									.FillWidth(180)
									.ShouldGenerateWidget(TAttribute<bool>::Create(TAttribute<bool>::FGetter::CreateSP(this, &ST1AssetView::ShouldColumnGenerateWidget, TagName.ToString())))
									.MenuContent()
									[
										CreateRowHeaderMenuContent(TagName.ToString())
									]);								
								
								NumVisibleColumns += HiddenColumnNames.Contains(TagName.ToString()) ? 0 : 1;

								// If we found a tag the matches the column we are currently sorting on, there will be no need to change the column
								for (int32 SortIdx = 0; SortIdx < CurrentSortOrder.Num(); SortIdx++)
								{
									if (TagName == CurrentSortOrder[SortIdx].SortColumn)
									{
										CurrentSortOrder[SortIdx].bSortRelevant = true;
									}
								}
							}
						}
					}
				}			
			}	
		}

		// Are any of the sort columns irrelevant now, if so remove them from the list
		bool CurrentSortChanged = false;
		for (int32 SortIdx = CurrentSortOrder.Num() - 1; SortIdx >= 0; SortIdx--)
		{
			if (!CurrentSortOrder[SortIdx].bSortRelevant)
			{
				CurrentSortOrder.RemoveAt(SortIdx);
				CurrentSortChanged = true;
			}
		}
		if (CurrentSortOrder.Num() > 0 && CurrentSortChanged)
		{
			// Sort order has changed, update the columns keeping those that are relevant
			int32 PriorityNum = EColumnSortPriority::Primary;
			for (int32 SortIdx = 0; SortIdx < CurrentSortOrder.Num(); SortIdx++)
			{
				check(CurrentSortOrder[SortIdx].bSortRelevant);
				if (!SortManager.SetOrToggleSortColumn(static_cast<EColumnSortPriority::Type>(PriorityNum), CurrentSortOrder[SortIdx].SortColumn))
				{
					// Toggle twice so mode is preserved if this isn't a new column assignation
					SortManager.SetOrToggleSortColumn(static_cast<EColumnSortPriority::Type>(PriorityNum), CurrentSortOrder[SortIdx].SortColumn);
				}				
				bPendingSortFilteredItems = true;
				PriorityNum++;
			}
		}
		else if (CurrentSortOrder.Num() == 0)
		{
			// If the current sort column is no longer relevant, revert to "Name" and resort when convenient
			SortManager.ResetSort();
			bPendingSortFilteredItems = true;
		}
	}
}

void ST1AssetView::OnAssetsAddedToCollection( const FCollectionNameType& Collection, const TArray< FName >& ObjectPaths )
{
	if ( !T1SourcesData.Collections.Contains( Collection ) )
	{
		return;
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	for (int Index = 0; Index < ObjectPaths.Num(); Index++)
	{
		OnAssetAdded( AssetRegistryModule.Get().GetAssetByObjectPath( ObjectPaths[Index] ) );
	}
}

void ST1AssetView::OnAssetAdded(const FAssetData& AssetData)
{
	RecentlyAddedAssets.Add(AssetData);
}

void ST1AssetView::ProcessRecentlyAddedAssets()
{
	if (
		(RecentlyAddedAssets.Num() > 2048) ||
		(RecentlyAddedAssets.Num() > 0 && FPlatformTime::Seconds() - LastProcessAddsTime >= TimeBetweenAddingNewAssets)
		)
	{
		RunAssetsThroughBackendFilter(RecentlyAddedAssets);
		FilteredRecentlyAddedAssets.Append(RecentlyAddedAssets);
		RecentlyAddedAssets.Empty();
		LastProcessAddsTime = FPlatformTime::Seconds();
	}

	if (FilteredRecentlyAddedAssets.Num() > 0)
	{
		double TickStartTime = FPlatformTime::Seconds();
		bool bNeedsRefresh = false;

		TSet<FName> ExistingObjectPaths;
		for ( auto AssetIt = AssetItems.CreateConstIterator(); AssetIt; ++AssetIt )
		{
			ExistingObjectPaths.Add((*AssetIt).ObjectPath);
		}

		for ( auto AssetIt = QueriedAssetItems.CreateConstIterator(); AssetIt; ++AssetIt )
		{
			ExistingObjectPaths.Add((*AssetIt).ObjectPath);
		}

		int32 AssetIdx = 0;
		for ( ; AssetIdx < FilteredRecentlyAddedAssets.Num(); ++AssetIdx )
		{
			const FAssetData& AssetData = FilteredRecentlyAddedAssets[AssetIdx];
			if ( !ExistingObjectPaths.Contains(AssetData.ObjectPath) )
			{
				if ( AssetData.AssetClass != UObjectRedirector::StaticClass()->GetFName() || AssetData.IsUAsset() )
				{
					if ( !OnShouldFilterAsset.IsBound() || !OnShouldFilterAsset.Execute(AssetData) )
					{
						// Add the asset to the list
						int32 AddedAssetIdx = AssetItems.Add(AssetData);
						ExistingObjectPaths.Add(AssetData.ObjectPath);
						if (!IsFrontendFilterActive() || PassesCurrentFrontendFilter(AssetData))
						{
							FilteredAssetItems.Add(MakeShareable(new FT1AssetViewAsset(AssetData)));
							bNeedsRefresh = true;
							bPendingSortFilteredItems = true;
						}
					}
				}
			}

			if ( (FPlatformTime::Seconds() - TickStartTime) > MaxSecondsPerFrame)
			{
				// Increment the index to properly trim the buffer below
				++AssetIdx;
				break;
			}
		}

		// Trim the results array
		if (AssetIdx > 0)
		{
			FilteredRecentlyAddedAssets.RemoveAt(0, AssetIdx);
		}

		if (bNeedsRefresh)
		{
			RefreshList();
		}
	}
}

void ST1AssetView::OnAssetsRemovedFromCollection( const FCollectionNameType& Collection, const TArray< FName >& ObjectPaths )
{
	if ( !T1SourcesData.Collections.Contains( Collection ) )
	{
		return;
	}

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	for (int Index = 0; Index < ObjectPaths.Num(); Index++)
	{
		OnAssetRemoved( AssetRegistryModule.Get().GetAssetByObjectPath( ObjectPaths[Index] ) );
	}
}

void ST1AssetView::OnAssetRemoved(const FAssetData& AssetData)
{
	RemoveAssetByPath( AssetData.ObjectPath );
	RecentlyAddedAssets.RemoveSingleSwap(AssetData);
}

void ST1AssetView::OnAssetRegistryPathAdded(const FString& Path)
{
	if(IsShowingFolders() && !ShouldFilterRecursively())
	{
		TSharedRef<FT1EmptyFolderVisibilityManager> EmptyFolderVisibilityManager = FT1ContentBrowserSingleton::Get().GetEmptyFolderVisibilityManager();

		// If this isn't a developer folder or we want to show them, continue
		const bool bDisplayEmpty = IsShowingEmptyFolders();
		const bool bDisplayDev = IsShowingDevelopersContent();
		const bool bDisplayL10N = IsShowingLocalizedContent();
		if ((bDisplayEmpty || EmptyFolderVisibilityManager->ShouldShowPath(Path)) &&  
			(bDisplayDev || !T1ContentBrowserUtils::IsDevelopersFolder(Path)) && 
			(bDisplayL10N || !T1ContentBrowserUtils::IsLocalizationFolder(Path))
			)
		{
			for (const FName& SourcePathName : T1SourcesData.PackagePaths)
			{
				const FString SourcePath = SourcePathName.ToString();
				if(Path.StartsWith(SourcePath))
				{
					const FString SubPath = Path.RightChop(SourcePath.Len());
					
					TArray<FString> SubPathItemList;
					SubPath.ParseIntoArray(SubPathItemList, TEXT("/"), /*InCullEmpty=*/true);

					if(SubPathItemList.Num() > 0)
					{
						const FString NewSubFolder = SourcePath / SubPathItemList[0];
						if(!Folders.Contains(NewSubFolder))
						{
							FilteredAssetItems.Add(MakeShareable(new FT1AssetViewFolder(NewSubFolder)));
							RefreshList();
							Folders.Add(NewSubFolder);
							bPendingSortFilteredItems = true;
						}
					}
				}
			}
		}
	}
}

void ST1AssetView::OnAssetRegistryPathRemoved(const FString& Path)
{
	FString* Folder = Folders.Find(Path);
	if(Folder != NULL)
	{
		Folders.Remove(Path);

		for (int32 AssetIdx = 0; AssetIdx < FilteredAssetItems.Num(); ++AssetIdx)
		{
			if(FilteredAssetItems[AssetIdx]->GetType() == EAssetItemType::Folder)
			{
				if ( StaticCastSharedPtr<FT1AssetViewFolder>(FilteredAssetItems[AssetIdx])->FolderPath == Path )
				{
					// Found the folder in the filtered items list, remove it
					FilteredAssetItems.RemoveAt(AssetIdx);
					RefreshList();
					break;
				}
			}
		}
	}
}

void ST1AssetView::OnFolderPopulated(const FString& Path)
{
	OnAssetRegistryPathAdded(Path);
}

void ST1AssetView::RemoveAssetByPath( const FName& ObjectPath )
{
	bool bFoundAsset = false;
	for (int32 AssetIdx = 0; AssetIdx < AssetItems.Num(); ++AssetIdx)
	{
		if ( AssetItems[AssetIdx].ObjectPath == ObjectPath )
		{
			// Found the asset in the cached list, remove it
			AssetItems.RemoveAt(AssetIdx);
			bFoundAsset = true;
			break;
		}
	}

	if ( bFoundAsset )
	{
		// If it was in the AssetItems list, see if it is also in the FilteredAssetItems list
		for (int32 AssetIdx = 0; AssetIdx < FilteredAssetItems.Num(); ++AssetIdx)
		{
			if(FilteredAssetItems[AssetIdx].IsValid() && FilteredAssetItems[AssetIdx]->GetType() != EAssetItemType::Folder)
			{
				if ( StaticCastSharedPtr<FT1AssetViewAsset>(FilteredAssetItems[AssetIdx])->Data.ObjectPath == ObjectPath && !FilteredAssetItems[AssetIdx]->IsTemporaryItem() )
				{
					// Found the asset in the filtered items list, remove it
					FilteredAssetItems.RemoveAt(AssetIdx);
					RefreshList();
					break;
				}
			}
		}
	}
	else
	{
		//Make sure we don't have the item still queued up for processing
		for (int32 AssetIdx = 0; AssetIdx < QueriedAssetItems.Num(); ++AssetIdx)
		{
			if ( QueriedAssetItems[AssetIdx].ObjectPath == ObjectPath )
			{
				// Found the asset in the cached list, remove it
				QueriedAssetItems.RemoveAt(AssetIdx);
				bFoundAsset = true;
				break;
			}
		}
	}
}

void ST1AssetView::OnCollectionRenamed( const FCollectionNameType& OriginalCollection, const FCollectionNameType& NewCollection )
{
	int32 FoundIndex = INDEX_NONE;
	if ( T1SourcesData.Collections.Find( OriginalCollection, FoundIndex ) )
	{
		T1SourcesData.Collections[ FoundIndex ] = NewCollection;
	}
}

void ST1AssetView::OnCollectionUpdated( const FCollectionNameType& Collection )
{
	// A collection has changed in some way, so we need to refresh our backend list
	RequestSlowFullListRefresh();
}

void ST1AssetView::OnAssetRenamed(const FAssetData& AssetData, const FString& OldObjectPath)
{
	// Remove the old asset, if it exists
	FName OldObjectPackageName = *OldObjectPath;
	RemoveAssetByPath( OldObjectPackageName );
	RecentlyAddedAssets.RemoveAllSwap( [&](const FAssetData& Other) { return Other.ObjectPath == OldObjectPackageName; } );

	// Add the new asset, if it should be in the cached list
	OnAssetAdded( AssetData );

	// Force an update of the recently added asset next frame
	RequestAddNewAssetsNextFrame();
}

void ST1AssetView::OnAssetLoaded(UObject* Asset)
{
	if ( Asset != NULL )
	{
		RecentlyLoadedOrChangedAssets.Add( FName(*Asset->GetPathName()), Asset );
	}
}

void ST1AssetView::OnObjectPropertyChanged(UObject* Object, FPropertyChangedEvent& PropertyChangedEvent)
{
	if (Object != nullptr && Object->IsAsset())
	{
		RecentlyLoadedOrChangedAssets.Add( FName(*Object->GetPathName()), Object);
	}
}

void ST1AssetView::OnClassHierarchyUpdated()
{
	// The class hierarchy has changed in some way, so we need to refresh our backend list
	RequestSlowFullListRefresh();
}

void ST1AssetView::OnFrontendFiltersChanged()
{
	RequestQuickFrontendListRefresh();

	// If we're not operating on recursively filtered data, we need to ensure a full slow
	// refresh is performed.
	if ( ShouldFilterRecursively() && !bWereItemsRecursivelyFiltered )
	{
		RequestSlowFullListRefresh();
	}
}

bool ST1AssetView::IsFrontendFilterActive() const
{
	return ( FrontendFilters.IsValid() && FrontendFilters->Num() > 0 );
}

bool ST1AssetView::PassesCurrentFrontendFilter(const FAssetData& Item) const
{
	// Check the frontend filters list
	if ( FrontendFilters.IsValid() && !FrontendFilters->PassesAllFilters(Item) )
	{
		return false;
	}

	return true;
}

void ST1AssetView::RunAssetsThroughBackendFilter(TArray<FAssetData>& InOutAssetDataList) const
{
	const bool bRecurse = ShouldFilterRecursively();
	const bool bUsingFolders = IsShowingFolders();
	const bool bIsDynamicCollection = T1SourcesData.IsDynamicCollection();
	FARFilter Filter = T1SourcesData.MakeFilter(bRecurse, bUsingFolders);
	
	if ( T1SourcesData.HasCollections() && Filter.ObjectPaths.Num() == 0 && !bIsDynamicCollection )
	{
		// This is an empty collection, no asset will pass the check
		InOutAssetDataList.Empty();
	}
	else
	{
		// Actually append the backend filter
		Filter.Append(BackendFilter);

		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		AssetRegistryModule.Get().RunAssetsThroughFilter(InOutAssetDataList, Filter);

		if ( T1SourcesData.HasCollections() && !bIsDynamicCollection )
		{
			// Include objects from child collections if we're recursing
			const ECollectionRecursionFlags::Flags CollectionRecursionMode = (Filter.bRecursivePaths) ? ECollectionRecursionFlags::SelfAndChildren : ECollectionRecursionFlags::Self;

			FCollectionManagerModule& CollectionManagerModule = FCollectionManagerModule::GetModule();
			TArray< FName > CollectionObjectPaths;
			for (const FCollectionNameType& Collection : T1SourcesData.Collections)
			{
				CollectionManagerModule.Get().GetObjectsInCollection(Collection.Name, Collection.Type, CollectionObjectPaths, CollectionRecursionMode);
			}

			for ( int32 AssetDataIdx = InOutAssetDataList.Num() - 1; AssetDataIdx >= 0; --AssetDataIdx )
			{
				const FAssetData& AssetData = InOutAssetDataList[AssetDataIdx];

				if ( !CollectionObjectPaths.Contains( AssetData.ObjectPath ) )
				{
					InOutAssetDataList.RemoveAtSwap(AssetDataIdx);
				}
			}
		}
	}
}

void ST1AssetView::SortList(bool bSyncToSelection)
{
	if ( !IsRenamingAsset() )
	{
		SortManager.SortList(FilteredAssetItems, MajorityAssetType, CustomColumns);

		// Update the thumbnails we were using since the order has changed
		bPendingUpdateThumbnails = true;

		if ( bSyncToSelection )
		{
			// Make sure the selection is in view
			const bool bFocusOnSync = false;
			SyncToSelection(bFocusOnSync);
		}

		RefreshList();
		bPendingSortFilteredItems = false;
		LastSortTime = CurrentTime;
	}
	else
	{
		bPendingSortFilteredItems = true;
	}
}

FLinearColor ST1AssetView::GetThumbnailHintColorAndOpacity() const
{
	//We update this color in tick instead of here as an optimization
	return ThumbnailHintColorAndOpacity;
}

FSlateColor ST1AssetView::GetViewButtonForegroundColor() const
{
	static const FName InvertedForegroundName("InvertedForeground");
	static const FName DefaultForegroundName("DefaultForeground");

	return ViewOptionsComboButton->IsHovered() ? FEditorStyle::GetSlateColor(InvertedForegroundName) : FEditorStyle::GetSlateColor(DefaultForegroundName);
}

TSharedRef<SWidget> ST1AssetView::GetViewButtonContent()
{
	// Get all menu extenders for this context menu from the content browser module
	T1ContentBrowserEditor& ContentBrowserModule = FModuleManager::GetModuleChecked<T1ContentBrowserEditor>( TEXT("T1ContentBrowserEditor") );
	TArray<FT1ContentBrowserMenuExtender> MenuExtenderDelegates = ContentBrowserModule.GetAllAssetViewViewMenuExtenders();

	TArray<TSharedPtr<FExtender>> Extenders;
	for (int32 i = 0; i < MenuExtenderDelegates.Num(); ++i)
	{
		if (MenuExtenderDelegates[i].IsBound())
		{
			Extenders.Add(MenuExtenderDelegates[i].Execute());
		}
	}
	TSharedPtr<FExtender> MenuExtender = FExtender::Combine(Extenders);

	FMenuBuilder MenuBuilder(/*bInShouldCloseWindowAfterMenuSelection=*/true, NULL, MenuExtender, /*bCloseSelfOnly=*/ true);

	MenuBuilder.BeginSection("AssetViewType", LOCTEXT("ViewTypeHeading", "View Type"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("TileViewOption", "Tiles"),
			LOCTEXT("TileViewOptionToolTip", "View assets as tiles in a grid."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP( this, &ST1AssetView::SetCurrentViewType, ET1AssetViewType::Tile ),
				FCanExecuteAction(),
				FIsActionChecked::CreateSP( this, &ST1AssetView::IsCurrentViewType, ET1AssetViewType::Tile )
				),
			NAME_None,
			EUserInterfaceActionType::RadioButton
			);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("ListViewOption", "List"),
			LOCTEXT("ListViewOptionToolTip", "View assets in a list with thumbnails."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP( this, &ST1AssetView::SetCurrentViewType, ET1AssetViewType::List ),
				FCanExecuteAction(),
				FIsActionChecked::CreateSP( this, &ST1AssetView::IsCurrentViewType, ET1AssetViewType::List )
				),
			NAME_None,
			EUserInterfaceActionType::RadioButton
			);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("ColumnViewOption", "Columns"),
			LOCTEXT("ColumnViewOptionToolTip", "View assets in a list with columns of details."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP( this, &ST1AssetView::SetCurrentViewType, ET1AssetViewType::Column ),
				FCanExecuteAction(),
				FIsActionChecked::CreateSP( this, &ST1AssetView::IsCurrentViewType, ET1AssetViewType::Column )
				),
			NAME_None,
			EUserInterfaceActionType::RadioButton
			);
	}
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection("View", LOCTEXT("ViewHeading", "View"));
	{
		auto CreateShowFoldersSubMenu = [this](FMenuBuilder& SubMenuBuilder)
		{
			SubMenuBuilder.AddMenuEntry(
				LOCTEXT("ShowEmptyFoldersOption", "Show Empty Folders"),
				LOCTEXT("ShowEmptyFoldersOptionToolTip", "Show empty folders in the view as well as assets?"),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateSP( this, &ST1AssetView::ToggleShowEmptyFolders ),
					FCanExecuteAction::CreateSP( this, &ST1AssetView::IsToggleShowEmptyFoldersAllowed ),
					FIsActionChecked::CreateSP( this, &ST1AssetView::IsShowingEmptyFolders )
				),
				NAME_None,
				EUserInterfaceActionType::ToggleButton
			);
		};

		MenuBuilder.AddSubMenu(
			LOCTEXT("ShowFoldersOption", "Show Folders"),
			LOCTEXT("ShowFoldersOptionToolTip", "Show folders in the view as well as assets?"),
			FNewMenuDelegate::CreateLambda(CreateShowFoldersSubMenu),
			FUIAction(
				FExecuteAction::CreateSP( this, &ST1AssetView::ToggleShowFolders ),
				FCanExecuteAction::CreateSP( this, &ST1AssetView::IsToggleShowFoldersAllowed ),
				FIsActionChecked::CreateSP( this, &ST1AssetView::IsShowingFolders )
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("ShowCollectionOption", "Show Collections"),
			LOCTEXT("ShowCollectionOptionToolTip", "Show the collections list in the view?"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP( this, &ST1AssetView::ToggleShowCollections ),
				FCanExecuteAction::CreateSP( this, &ST1AssetView::IsToggleShowCollectionsAllowed ),
				FIsActionChecked::CreateSP( this, &ST1AssetView::IsShowingCollections )
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("ShowFavoriteOptions", "Show Favorites"),
			LOCTEXT("ShowFavoriteOptionToolTip", "Show the favorite folders in the view?"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP(this, &ST1AssetView::ToggleShowFavorites),
				FCanExecuteAction::CreateSP(this, &ST1AssetView::IsToggleShowFavoritesAllowed),
				FIsActionChecked::CreateSP(this, &ST1AssetView::IsShowingFavorites)
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);
	}
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection("Content", LOCTEXT("ContentHeading", "Content"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("ShowCppClassesOption", "Show C++ Classes"),
			LOCTEXT("ShowCppClassesOptionToolTip", "Show C++ classes in the view?"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP( this, &ST1AssetView::ToggleShowCppContent ),
				FCanExecuteAction::CreateSP( this, &ST1AssetView::IsToggleShowCppContentAllowed ),
				FIsActionChecked::CreateSP( this, &ST1AssetView::IsShowingCppContent )
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("ShowDevelopersContentOption", "Show Developers Content"),
			LOCTEXT("ShowDevelopersContentOptionToolTip", "Show developers content in the view?"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP( this, &ST1AssetView::ToggleShowDevelopersContent ),
				FCanExecuteAction::CreateSP( this, &ST1AssetView::IsToggleShowDevelopersContentAllowed ),
				FIsActionChecked::CreateSP( this, &ST1AssetView::IsShowingDevelopersContent )
				),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("ShowEngineFolderOption", "Show Engine Content"),
			LOCTEXT("ShowEngineFolderOptionToolTip", "Show engine content in the view?"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP( this, &ST1AssetView::ToggleShowEngineContent ),
				FCanExecuteAction(),
				FIsActionChecked::CreateSP( this, &ST1AssetView::IsShowingEngineContent )
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("ShowPluginFolderOption", "Show Plugin Content"),
			LOCTEXT("ShowPluginFolderOptionToolTip", "Show plugin content in the view?"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP( this, &ST1AssetView::ToggleShowPluginContent ),
				FCanExecuteAction(),
				FIsActionChecked::CreateSP( this, &ST1AssetView::IsShowingPluginContent )
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("ShowLocalizedContentOption", "Show Localized Content"),
			LOCTEXT("ShowLocalizedContentOptionToolTip", "Show localized content in the view?"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP(this, &ST1AssetView::ToggleShowLocalizedContent),
				FCanExecuteAction::CreateSP(this, &ST1AssetView::IsToggleShowLocalizedContentAllowed),
				FIsActionChecked::CreateSP(this, &ST1AssetView::IsShowingLocalizedContent)
				),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
			);
	}
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection("Search", LOCTEXT("SearchHeading", "Search"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("IncludeClassNameOption", "Search Asset Class Names"),
			LOCTEXT("IncludeClassesNameOptionTooltip", "Include asset type names in search criteria?  (e.g. Blueprint, Texture, Sound)"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP(this, &ST1AssetView::ToggleIncludeClassNames),
				FCanExecuteAction::CreateSP(this, &ST1AssetView::IsToggleIncludeClassNamesAllowed),
				FIsActionChecked::CreateSP(this, &ST1AssetView::IsIncludingClassNames)
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("IncludeAssetPathOption", "Search Asset Path"),
			LOCTEXT("IncludeAssetPathOptionTooltip", "Include entire asset path in search criteria?"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP(this, &ST1AssetView::ToggleIncludeAssetPaths),
				FCanExecuteAction::CreateSP(this, &ST1AssetView::IsToggleIncludeAssetPathsAllowed),
				FIsActionChecked::CreateSP(this, &ST1AssetView::IsIncludingAssetPaths)
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("IncludeCollectionNameOption", "Search Collection Names"),
			LOCTEXT("IncludeCollectionNameOptionTooltip", "Include Collection names in search criteria?"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP(this, &ST1AssetView::ToggleIncludeCollectionNames),
				FCanExecuteAction::CreateSP(this, &ST1AssetView::IsToggleIncludeCollectionNamesAllowed),
				FIsActionChecked::CreateSP(this, &ST1AssetView::IsIncludingCollectionNames)
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
		);
	}
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection("AssetThumbnails", LOCTEXT("ThumbnailsHeading", "Thumbnails"));
	{
		MenuBuilder.AddWidget(
			SNew(SSlider)
				.ToolTipText( LOCTEXT("ThumbnailScaleToolTip", "Adjust the size of thumbnails.") )
				.Value( this, &ST1AssetView::GetThumbnailScale )
				.OnValueChanged( this, &ST1AssetView::SetThumbnailScale )
				.Locked( this, &ST1AssetView::IsThumbnailScalingLocked ),
			LOCTEXT("ThumbnailScaleLabel", "Scale"),
			/*bNoIndent=*/true
			);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("ThumbnailEditModeOption", "Thumbnail Edit Mode"),
			LOCTEXT("ThumbnailEditModeOptionToolTip", "Toggle thumbnail editing mode. When in this mode you can rotate the camera on 3D thumbnails by dragging them."),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP( this, &ST1AssetView::ToggleThumbnailEditMode ),
				FCanExecuteAction::CreateSP( this, &ST1AssetView::IsThumbnailEditModeAllowed ),
				FIsActionChecked::CreateSP( this, &ST1AssetView::IsThumbnailEditMode )
				),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
			);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("RealTimeThumbnailsOption", "Real-Time Thumbnails"),
			LOCTEXT("RealTimeThumbnailsOptionToolTip", "Renders the assets thumbnails in real-time"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP( this, &ST1AssetView::ToggleRealTimeThumbnails ),
				FCanExecuteAction::CreateSP( this, &ST1AssetView::CanShowRealTimeThumbnails ),
				FIsActionChecked::CreateSP( this, &ST1AssetView::IsShowingRealTimeThumbnails )
			),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
			);
	}
	MenuBuilder.EndSection();

	if (GetColumnViewVisibility() == EVisibility::Visible)
	{
		MenuBuilder.BeginSection("AssetColumns", LOCTEXT("ToggleColumnsHeading", "Columns"));
		{
			MenuBuilder.AddSubMenu(
				LOCTEXT("ToggleColumnsMenu", "Toggle columns"),
				LOCTEXT("ToggleColumnsMenuTooltip", "Show or hide specific columns."),
				FNewMenuDelegate::CreateSP(this, &ST1AssetView::FillToggleColumnsMenu),
				false,
				FSlateIcon(),
				false
				);

			MenuBuilder.AddMenuEntry(
				LOCTEXT("ResetColumns", "Reset Columns"),
				LOCTEXT("ResetColumnsToolTip", "Reset all columns to be visible again."),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateSP(this, &ST1AssetView::ResetColumns)),
				NAME_None,
				EUserInterfaceActionType::Button
				);

			MenuBuilder.AddMenuEntry(
				LOCTEXT("ExportColumns", "Export to CSV"),
				LOCTEXT("ExportColumnsToolTip", "Export column data to CSV."),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateSP(this, &ST1AssetView::ExportColumns)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
		}
		MenuBuilder.EndSection();
	}

	return MenuBuilder.MakeWidget();
}

void ST1AssetView::ToggleShowFolders()
{
	check( IsToggleShowFoldersAllowed() );
	GetMutableDefault<UContentBrowserSettings>()->DisplayFolders = !GetDefault<UContentBrowserSettings>()->DisplayFolders;
	GetMutableDefault<UContentBrowserSettings>()->PostEditChange();
}

bool ST1AssetView::IsToggleShowFoldersAllowed() const
{
	return bCanShowFolders;
}

bool ST1AssetView::IsShowingFolders() const
{
	return IsToggleShowFoldersAllowed() && GetDefault<UContentBrowserSettings>()->DisplayFolders;
}

void ST1AssetView::ToggleShowEmptyFolders()
{
	check( IsToggleShowEmptyFoldersAllowed() );
	GetMutableDefault<UContentBrowserSettings>()->DisplayEmptyFolders = !GetDefault<UContentBrowserSettings>()->DisplayEmptyFolders;
	GetMutableDefault<UContentBrowserSettings>()->PostEditChange();
}

bool ST1AssetView::IsToggleShowEmptyFoldersAllowed() const
{
	return bCanShowFolders;
}

bool ST1AssetView::IsShowingEmptyFolders() const
{
	return IsToggleShowEmptyFoldersAllowed() && GetDefault<UContentBrowserSettings>()->DisplayEmptyFolders;
}

void ST1AssetView::ToggleRealTimeThumbnails()
{
	check( CanShowRealTimeThumbnails() );
	GetMutableDefault<UContentBrowserSettings>()->RealTimeThumbnails = !GetDefault<UContentBrowserSettings>()->RealTimeThumbnails;
	GetMutableDefault<UContentBrowserSettings>()->PostEditChange();
}

bool ST1AssetView::CanShowRealTimeThumbnails() const
{
	return bCanShowRealTimeThumbnails;
}

bool ST1AssetView::IsShowingRealTimeThumbnails() const
{
	return CanShowRealTimeThumbnails() && GetDefault<UContentBrowserSettings>()->RealTimeThumbnails;
}

void ST1AssetView::ToggleShowPluginContent()
{
	bool bDisplayPlugins = GetDefault<UContentBrowserSettings>()->GetDisplayPluginFolders();
	bool bRawDisplayPlugins = GetDefault<UContentBrowserSettings>()->GetDisplayPluginFolders( true );

	// Only if both these flags are false when toggling we want to enable the flag, otherwise we're toggling off
	if ( !bDisplayPlugins && !bRawDisplayPlugins )
	{
		GetMutableDefault<UContentBrowserSettings>()->SetDisplayPluginFolders( true );
	}
	else
	{
		GetMutableDefault<UContentBrowserSettings>()->SetDisplayPluginFolders( false );
		GetMutableDefault<UContentBrowserSettings>()->SetDisplayPluginFolders( false, true );
	}	
	GetMutableDefault<UContentBrowserSettings>()->PostEditChange();
}

bool ST1AssetView::IsShowingPluginContent() const
{
	return GetDefault<UContentBrowserSettings>()->GetDisplayPluginFolders();
}

void ST1AssetView::ToggleShowEngineContent()
{
	bool bDisplayEngine = GetDefault<UContentBrowserSettings>()->GetDisplayEngineFolder();
	bool bRawDisplayEngine = GetDefault<UContentBrowserSettings>()->GetDisplayEngineFolder( true );

	// Only if both these flags are false when toggling we want to enable the flag, otherwise we're toggling off
	if ( !bDisplayEngine && !bRawDisplayEngine )
	{
		GetMutableDefault<UContentBrowserSettings>()->SetDisplayEngineFolder( true );
	}
	else
	{
		GetMutableDefault<UContentBrowserSettings>()->SetDisplayEngineFolder( false );
		GetMutableDefault<UContentBrowserSettings>()->SetDisplayEngineFolder( false, true );
	}	
	GetMutableDefault<UContentBrowserSettings>()->PostEditChange();
}

bool ST1AssetView::IsShowingEngineContent() const
{
	return GetDefault<UContentBrowserSettings>()->GetDisplayEngineFolder();
}

void ST1AssetView::ToggleShowDevelopersContent()
{
	bool bDisplayDev = GetDefault<UContentBrowserSettings>()->GetDisplayDevelopersFolder();
	bool bRawDisplayDev = GetDefault<UContentBrowserSettings>()->GetDisplayDevelopersFolder( true );

	// Only if both these flags are false when toggling we want to enable the flag, otherwise we're toggling off
	if ( !bDisplayDev && !bRawDisplayDev )
	{
		GetMutableDefault<UContentBrowserSettings>()->SetDisplayDevelopersFolder( true );
	}
	else
	{
		GetMutableDefault<UContentBrowserSettings>()->SetDisplayDevelopersFolder( false );
		GetMutableDefault<UContentBrowserSettings>()->SetDisplayDevelopersFolder( false, true );
	}	
	GetMutableDefault<UContentBrowserSettings>()->PostEditChange();
}

bool ST1AssetView::IsToggleShowDevelopersContentAllowed() const
{
	return bCanShowDevelopersFolder;
}

bool ST1AssetView::IsShowingDevelopersContent() const
{
	return IsToggleShowDevelopersContentAllowed() && GetDefault<UContentBrowserSettings>()->GetDisplayDevelopersFolder();
}

void ST1AssetView::ToggleShowLocalizedContent()
{
	GetMutableDefault<UContentBrowserSettings>()->SetDisplayL10NFolder(!GetDefault<UContentBrowserSettings>()->GetDisplayL10NFolder());
	GetMutableDefault<UContentBrowserSettings>()->PostEditChange();
}

bool ST1AssetView::IsToggleShowLocalizedContentAllowed() const
{
	return true;
}

bool ST1AssetView::IsShowingLocalizedContent() const
{
	return IsToggleShowLocalizedContentAllowed() && GetDefault<UContentBrowserSettings>()->GetDisplayL10NFolder();
}

void ST1AssetView::ToggleShowCollections()
{
	const bool bDisplayCollections = GetDefault<UContentBrowserSettings>()->GetDisplayCollections();
	GetMutableDefault<UContentBrowserSettings>()->SetDisplayCollections( !bDisplayCollections );
	GetMutableDefault<UContentBrowserSettings>()->PostEditChange();
}

bool ST1AssetView::IsToggleShowCollectionsAllowed() const
{
	return bCanShowCollections;
}

bool ST1AssetView::IsShowingCollections() const
{
	return IsToggleShowCollectionsAllowed() && GetDefault<UContentBrowserSettings>()->GetDisplayCollections();
}


void ST1AssetView::ToggleShowFavorites()
{
	const bool bShowingFavorites = GetDefault<UContentBrowserSettings>()->GetDisplayFavorites();
	GetMutableDefault<UContentBrowserSettings>()->SetDisplayFavorites(!bShowingFavorites);
	GetMutableDefault<UContentBrowserSettings>()->PostEditChange();
}

bool ST1AssetView::IsToggleShowFavoritesAllowed() const
{
	return bCanShowFavorites;
}

bool ST1AssetView::IsShowingFavorites() const
{
	return IsToggleShowFavoritesAllowed() && GetDefault<UContentBrowserSettings>()->GetDisplayFavorites();
}

void ST1AssetView::ToggleShowCppContent()
{
	const bool bDisplayCppFolders = GetDefault<UContentBrowserSettings>()->GetDisplayCppFolders();
	GetMutableDefault<UContentBrowserSettings>()->SetDisplayCppFolders(!bDisplayCppFolders);
	GetMutableDefault<UContentBrowserSettings>()->PostEditChange();
}

bool ST1AssetView::IsToggleShowCppContentAllowed() const
{
	return bCanShowClasses;
}

bool ST1AssetView::IsShowingCppContent() const
{
	return IsToggleShowCppContentAllowed() && GetDefault<UContentBrowserSettings>()->GetDisplayCppFolders();
}

void ST1AssetView::ToggleIncludeClassNames()
{
	const bool bIncludeClassNames = GetDefault<UContentBrowserSettings>()->GetIncludeClassNames();
	GetMutableDefault<UContentBrowserSettings>()->SetIncludeClassNames(!bIncludeClassNames);
	GetMutableDefault<UContentBrowserSettings>()->PostEditChange();

	OnSearchOptionsChanged.ExecuteIfBound();
}

bool ST1AssetView::IsToggleIncludeClassNamesAllowed() const
{
	return true;
}

bool ST1AssetView::IsIncludingClassNames() const
{
	return IsToggleIncludeClassNamesAllowed() && GetDefault<UContentBrowserSettings>()->GetIncludeClassNames();
}

void ST1AssetView::ToggleIncludeAssetPaths()
{
	const bool bIncludeAssetPaths = GetDefault<UContentBrowserSettings>()->GetIncludeAssetPaths();
	GetMutableDefault<UContentBrowserSettings>()->SetIncludeAssetPaths(!bIncludeAssetPaths);
	GetMutableDefault<UContentBrowserSettings>()->PostEditChange();

	OnSearchOptionsChanged.ExecuteIfBound();
}

bool ST1AssetView::IsToggleIncludeAssetPathsAllowed() const
{
	return true;
}

bool ST1AssetView::IsIncludingAssetPaths() const
{
	return IsToggleIncludeAssetPathsAllowed() && GetDefault<UContentBrowserSettings>()->GetIncludeAssetPaths();
}

void ST1AssetView::ToggleIncludeCollectionNames()
{
	const bool bIncludeCollectionNames = GetDefault<UContentBrowserSettings>()->GetIncludeCollectionNames();
	GetMutableDefault<UContentBrowserSettings>()->SetIncludeCollectionNames(!bIncludeCollectionNames);
	GetMutableDefault<UContentBrowserSettings>()->PostEditChange();

	OnSearchOptionsChanged.ExecuteIfBound();
}

bool ST1AssetView::IsToggleIncludeCollectionNamesAllowed() const
{
	return true;
}

bool ST1AssetView::IsIncludingCollectionNames() const
{
	return IsToggleIncludeCollectionNamesAllowed() && GetDefault<UContentBrowserSettings>()->GetIncludeCollectionNames();
}


void ST1AssetView::SetCurrentViewType(ET1AssetViewType::Type NewType)
{
	if ( ensure(NewType != ET1AssetViewType::MAX) && NewType != CurrentViewType )
	{
		ResetQuickJump();

		CurrentViewType = NewType;
		CreateCurrentView();

		SyncToSelection();

		// Clear relevant thumbnails to render fresh ones in the new view if needed
		RelevantThumbnails.Empty();
		VisibleItems.Empty();

		if ( NewType == ET1AssetViewType::Tile )
		{
			CurrentThumbnailSize = TileViewThumbnailSize;
			bPendingUpdateThumbnails = true;
		}
		else if ( NewType == ET1AssetViewType::List )
		{
			CurrentThumbnailSize = ListViewThumbnailSize;
			bPendingUpdateThumbnails = true;
		}
		else if ( NewType == ET1AssetViewType::Column )
		{
			// No thumbnails, but we do need to refresh filtered items to determine a majority asset type
			MajorityAssetType = NAME_None;
			RefreshFilteredItems();
			RefreshFolders();
			SortList();
		}

		FSlateApplication::Get().DismissAllMenus();
	}
}

void ST1AssetView::CreateCurrentView()
{
	TileView.Reset();
	ListView.Reset();
	ColumnView.Reset();

	TSharedRef<SWidget> NewView = SNullWidget::NullWidget;
	switch (CurrentViewType)
	{
		case ET1AssetViewType::Tile:
			TileView = CreateTileView();
			NewView = CreateShadowOverlay(TileView.ToSharedRef());
			break;
		case ET1AssetViewType::List:
			ListView = CreateListView();
			NewView = CreateShadowOverlay(ListView.ToSharedRef());
			break;
		case ET1AssetViewType::Column:
			ColumnView = CreateColumnView();
			NewView = CreateShadowOverlay(ColumnView.ToSharedRef());
			break;
	}
	
	ViewContainer->SetContent( NewView );
}

TSharedRef<SWidget> ST1AssetView::CreateShadowOverlay( TSharedRef<STableViewBase> Table )
{
	return SNew(SScrollBorder, Table)
		[
			Table
		];
}

ET1AssetViewType::Type ST1AssetView::GetCurrentViewType() const
{
	return CurrentViewType;
}

bool ST1AssetView::IsCurrentViewType(ET1AssetViewType::Type ViewType) const
{
	return GetCurrentViewType() == ViewType;
}

void ST1AssetView::FocusList() const
{
	switch ( GetCurrentViewType() )
	{
		case ET1AssetViewType::List: FSlateApplication::Get().SetKeyboardFocus(ListView, EFocusCause::SetDirectly); break;
		case ET1AssetViewType::Tile: FSlateApplication::Get().SetKeyboardFocus(TileView, EFocusCause::SetDirectly); break;
		case ET1AssetViewType::Column: FSlateApplication::Get().SetKeyboardFocus(ColumnView, EFocusCause::SetDirectly); break;
	}
}

void ST1AssetView::RefreshList()
{
	switch ( GetCurrentViewType() )
	{
		case ET1AssetViewType::List: ListView->RequestListRefresh(); break;
		case ET1AssetViewType::Tile: TileView->RequestListRefresh(); break;
		case ET1AssetViewType::Column: ColumnView->RequestListRefresh(); break;
	}
}

void ST1AssetView::SetSelection(const TSharedPtr<FT1AssetViewItem>& Item)
{
	switch ( GetCurrentViewType() )
	{
		case ET1AssetViewType::List: ListView->SetSelection(Item); break;
		case ET1AssetViewType::Tile: TileView->SetSelection(Item); break;
		case ET1AssetViewType::Column: ColumnView->SetSelection(Item); break;
	}
}

void ST1AssetView::SetItemSelection(const TSharedPtr<FT1AssetViewItem>& Item, bool bSelected, const ESelectInfo::Type SelectInfo)
{
	switch ( GetCurrentViewType() )
	{
		case ET1AssetViewType::List: ListView->SetItemSelection(Item, bSelected, SelectInfo); break;
		case ET1AssetViewType::Tile: TileView->SetItemSelection(Item, bSelected, SelectInfo); break;
		case ET1AssetViewType::Column: ColumnView->SetItemSelection(Item, bSelected, SelectInfo); break;
	}
}

void ST1AssetView::RequestScrollIntoView(const TSharedPtr<FT1AssetViewItem>& Item)
{
	switch ( GetCurrentViewType() )
	{
		case ET1AssetViewType::List: ListView->RequestScrollIntoView(Item); break;
		case ET1AssetViewType::Tile: TileView->RequestScrollIntoView(Item); break;
		case ET1AssetViewType::Column: ColumnView->RequestScrollIntoView(Item); break;
	}
}

void ST1AssetView::OnOpenAssetsOrFolders()
{
	TArray<FAssetData> SelectedAssets = GetSelectedAssets();
	TArray<FString> SelectedFolders = GetSelectedFolders();
	if (SelectedAssets.Num() > 0 && SelectedFolders.Num() == 0)
	{
		OnAssetsActivated.ExecuteIfBound(SelectedAssets, EAssetTypeActivationMethod::Opened);
	}
	else if (SelectedAssets.Num() == 0 && SelectedFolders.Num() > 0)
	{
		OnPathSelected.ExecuteIfBound(SelectedFolders[0]);
	}
}

void ST1AssetView::OnPreviewAssets()
{
	OnAssetsActivated.ExecuteIfBound(GetSelectedAssets(), EAssetTypeActivationMethod::Previewed);
}

void ST1AssetView::ClearSelection(bool bForceSilent)
{
	const bool bTempBulkSelectingValue = bForceSilent ? true : bBulkSelecting;
	TGuardValue<bool>(bBulkSelecting, bTempBulkSelectingValue);
	switch ( GetCurrentViewType() )
	{
		case ET1AssetViewType::List: ListView->ClearSelection(); break;
		case ET1AssetViewType::Tile: TileView->ClearSelection(); break;
		case ET1AssetViewType::Column: ColumnView->ClearSelection(); break;
	}
}

TSharedRef<ITableRow> ST1AssetView::MakeListViewWidget(TSharedPtr<FT1AssetViewItem> AssetItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	if ( !ensure(AssetItem.IsValid()) )
	{
		return SNew( STableRow<TSharedPtr<FT1AssetViewAsset>>, OwnerTable );
	}

	VisibleItems.Add(AssetItem);
	bPendingUpdateThumbnails = true;

	if(AssetItem->GetType() == EAssetItemType::Folder)
	{
		TSharedPtr< STableRow<TSharedPtr<FT1AssetViewItem>> > TableRowWidget;
		SAssignNew( TableRowWidget, STableRow<TSharedPtr<FT1AssetViewItem>>, OwnerTable )
			.Style(FEditorStyle::Get(), "T1ContentBrowser.AssetListView.TableRow")
			.Cursor( bAllowDragging ? EMouseCursor::GrabHand : EMouseCursor::Default )
			.OnDragDetected( this, &ST1AssetView::OnDraggingAssetItem );

		TSharedRef<ST1AssetListItem> Item =
			SNew(ST1AssetListItem)
			.AssetItem(AssetItem)
			.ItemHeight(this, &ST1AssetView::GetListViewItemHeight)
			.OnRenameBegin(this, &ST1AssetView::AssetRenameBegin)
			.OnRenameCommit(this, &ST1AssetView::AssetRenameCommit)
			.OnVerifyRenameCommit(this, &ST1AssetView::AssetVerifyRenameCommit)
			.OnItemDestroyed(this, &ST1AssetView::AssetItemWidgetDestroyed)
			.ShouldAllowToolTip(this, &ST1AssetView::ShouldAllowToolTips)
			.HighlightText(HighlightedText)
			.IsSelected( FIsSelected::CreateSP(TableRowWidget.Get(), &STableRow<TSharedPtr<FT1AssetViewItem>>::IsSelectedExclusively) )
			.OnAssetsOrPathsDragDropped(this, &ST1AssetView::OnAssetsOrPathsDragDropped)
			.OnFilesDragDropped(this, &ST1AssetView::OnFilesDragDropped);

		TableRowWidget->SetContent(Item);

		return TableRowWidget.ToSharedRef();
	}
	else
	{
		TSharedPtr<FT1AssetViewAsset> AssetItemAsAsset = StaticCastSharedPtr<FT1AssetViewAsset>(AssetItem);

		TSharedPtr<FAssetThumbnail>* AssetThumbnailPtr = RelevantThumbnails.Find(AssetItemAsAsset);
		TSharedPtr<FAssetThumbnail> AssetThumbnail;
		if ( AssetThumbnailPtr )
		{
			AssetThumbnail = *AssetThumbnailPtr;
		}
		else
		{
			const float ThumbnailResolution = ListViewThumbnailResolution;
			AssetThumbnail = MakeShareable( new FAssetThumbnail( AssetItemAsAsset->Data, ThumbnailResolution, ThumbnailResolution, AssetThumbnailPool ) );
			RelevantThumbnails.Add( AssetItemAsAsset, AssetThumbnail );
			AssetThumbnail->GetViewportRenderTargetTexture(); // Access the texture once to trigger it to render
		}

		TSharedPtr< STableRow<TSharedPtr<FT1AssetViewItem>> > TableRowWidget;
		SAssignNew( TableRowWidget, STableRow<TSharedPtr<FT1AssetViewItem>>, OwnerTable )
		.Style(FEditorStyle::Get(), "ContentBrowser.AssetListView.TableRow")
		.Cursor( bAllowDragging ? EMouseCursor::GrabHand : EMouseCursor::Default )
		.OnDragDetected( this, &ST1AssetView::OnDraggingAssetItem );

		TSharedRef<ST1AssetListItem> Item =
			SNew(ST1AssetListItem)
			.AssetThumbnail(AssetThumbnail)
			.AssetItem(AssetItem)
			.ThumbnailPadding(ListViewThumbnailPadding)
			.ItemHeight(this, &ST1AssetView::GetListViewItemHeight)
			.OnRenameBegin(this, &ST1AssetView::AssetRenameBegin)
			.OnRenameCommit(this, &ST1AssetView::AssetRenameCommit)
			.OnVerifyRenameCommit(this, &ST1AssetView::AssetVerifyRenameCommit)
			.OnItemDestroyed(this, &ST1AssetView::AssetItemWidgetDestroyed)
			.ShouldAllowToolTip(this, &ST1AssetView::ShouldAllowToolTips)
			.HighlightText(HighlightedText)
			.ThumbnailEditMode(this, &ST1AssetView::IsThumbnailEditMode)
			.ThumbnailLabel( ThumbnailLabel )
			.ThumbnailHintColorAndOpacity( this, &ST1AssetView::GetThumbnailHintColorAndOpacity )
			.AllowThumbnailHintLabel( AllowThumbnailHintLabel )
			.IsSelected( FIsSelected::CreateSP(TableRowWidget.Get(), &STableRow<TSharedPtr<FT1AssetViewItem>>::IsSelectedExclusively) )
			.OnIsAssetValidForCustomToolTip(OnIsAssetValidForCustomToolTip)
			.OnGetCustomAssetToolTip(OnGetCustomAssetToolTip)
			.OnVisualizeAssetToolTip(OnVisualizeAssetToolTip)
			.OnAssetToolTipClosing(OnAssetToolTipClosing);

		TableRowWidget->SetContent(Item);

		return TableRowWidget.ToSharedRef();
	}
}

TSharedRef<ITableRow> ST1AssetView::MakeTileViewWidget(TSharedPtr<FT1AssetViewItem> AssetItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	if ( !ensure(AssetItem.IsValid()) )
	{
		return SNew( STableRow<TSharedPtr<FT1AssetViewAsset>>, OwnerTable );
	}

	VisibleItems.Add(AssetItem);
	bPendingUpdateThumbnails = true;

	if(AssetItem->GetType() == EAssetItemType::Folder)
	{
		TSharedPtr< STableRow<TSharedPtr<FT1AssetViewItem>> > TableRowWidget;
		SAssignNew( TableRowWidget, STableRow<TSharedPtr<FT1AssetViewItem>>, OwnerTable )
			.Style( FEditorStyle::Get(), "ContentBrowser.AssetListView.TableRow" )
			.Cursor( bAllowDragging ? EMouseCursor::GrabHand : EMouseCursor::Default )
			.OnDragDetected( this, &ST1AssetView::OnDraggingAssetItem );

		TSharedRef<ST1AssetTileItem> Item =
			SNew(ST1AssetTileItem)
			.AssetItem(AssetItem)
			.ItemWidth(this, &ST1AssetView::GetTileViewItemWidth)
			.OnRenameBegin(this, &ST1AssetView::AssetRenameBegin)
			.OnRenameCommit(this, &ST1AssetView::AssetRenameCommit)
			.OnVerifyRenameCommit(this, &ST1AssetView::AssetVerifyRenameCommit)
			.OnItemDestroyed(this, &ST1AssetView::AssetItemWidgetDestroyed)
			.ShouldAllowToolTip(this, &ST1AssetView::ShouldAllowToolTips)
			.HighlightText( HighlightedText )
			.IsSelected( FIsSelected::CreateSP(TableRowWidget.Get(), &STableRow<TSharedPtr<FT1AssetViewItem>>::IsSelectedExclusively) )
			.OnAssetsOrPathsDragDropped(this, &ST1AssetView::OnAssetsOrPathsDragDropped)
			.OnFilesDragDropped(this, &ST1AssetView::OnFilesDragDropped);

		TableRowWidget->SetContent(Item);

		return TableRowWidget.ToSharedRef();
	}
	else
	{
		TSharedPtr<FT1AssetViewAsset> AssetItemAsAsset = StaticCastSharedPtr<FT1AssetViewAsset>(AssetItem);

		TSharedPtr<FAssetThumbnail>* AssetThumbnailPtr = RelevantThumbnails.Find(AssetItemAsAsset);
		TSharedPtr<FAssetThumbnail> AssetThumbnail;
		if ( AssetThumbnailPtr )
		{
			AssetThumbnail = *AssetThumbnailPtr;
		}
		else
		{
			const float ThumbnailResolution = TileViewThumbnailResolution;
			AssetThumbnail = MakeShareable( new FAssetThumbnail( AssetItemAsAsset->Data, ThumbnailResolution, ThumbnailResolution, AssetThumbnailPool ) );
			RelevantThumbnails.Add( AssetItemAsAsset, AssetThumbnail );
			AssetThumbnail->GetViewportRenderTargetTexture(); // Access the texture once to trigger it to render
		}

		TSharedPtr< STableRow<TSharedPtr<FT1AssetViewItem>> > TableRowWidget;
		SAssignNew( TableRowWidget, STableRow<TSharedPtr<FT1AssetViewItem>>, OwnerTable )
		.Style(FEditorStyle::Get(), "ContentBrowser.AssetListView.TableRow")
		.Cursor( bAllowDragging ? EMouseCursor::GrabHand : EMouseCursor::Default )
		.OnDragDetected( this, &ST1AssetView::OnDraggingAssetItem );

		TSharedRef<ST1AssetTileItem> Item =
			SNew(ST1AssetTileItem)
			.AssetThumbnail(AssetThumbnail)
			.AssetItem(AssetItem)
			.ThumbnailPadding(TileViewThumbnailPadding)
			.ItemWidth(this, &ST1AssetView::GetTileViewItemWidth)
			.OnRenameBegin(this, &ST1AssetView::AssetRenameBegin)
			.OnRenameCommit(this, &ST1AssetView::AssetRenameCommit)
			.OnVerifyRenameCommit(this, &ST1AssetView::AssetVerifyRenameCommit)
			.OnItemDestroyed(this, &ST1AssetView::AssetItemWidgetDestroyed)
			.ShouldAllowToolTip(this, &ST1AssetView::ShouldAllowToolTips)
			.HighlightText( HighlightedText )
			.ThumbnailEditMode(this, &ST1AssetView::IsThumbnailEditMode)
			.ThumbnailLabel( ThumbnailLabel )
			.ThumbnailHintColorAndOpacity( this, &ST1AssetView::GetThumbnailHintColorAndOpacity )
			.AllowThumbnailHintLabel( AllowThumbnailHintLabel )
			.IsSelected( FIsSelected::CreateSP(TableRowWidget.Get(), &STableRow<TSharedPtr<FT1AssetViewItem>>::IsSelectedExclusively) )
			.OnIsAssetValidForCustomToolTip(OnIsAssetValidForCustomToolTip)
			.OnGetCustomAssetToolTip(OnGetCustomAssetToolTip)
			.OnVisualizeAssetToolTip( OnVisualizeAssetToolTip )
			.OnAssetToolTipClosing( OnAssetToolTipClosing );

		TableRowWidget->SetContent(Item);

		return TableRowWidget.ToSharedRef();
	}
}

TSharedRef<ITableRow> ST1AssetView::MakeColumnViewWidget(TSharedPtr<FT1AssetViewItem> AssetItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	if ( !ensure(AssetItem.IsValid()) )
	{
		return SNew( STableRow<TSharedPtr<FT1AssetViewItem>>, OwnerTable )
			.Style(FEditorStyle::Get(), "ContentBrowser.AssetListView.TableRow");
	}

	// Update the cached custom data
	AssetItem->CacheCustomColumns(CustomColumns, false, true, false);
	
	return
		SNew( ST1AssetColumnViewRow, OwnerTable )
		.OnDragDetected( this, &ST1AssetView::OnDraggingAssetItem )
		.Cursor( bAllowDragging ? EMouseCursor::GrabHand : EMouseCursor::Default )
		.AssetColumnItem(
			SNew(ST1AssetColumnItem)
				.AssetItem(AssetItem)
				.OnRenameBegin(this, &ST1AssetView::AssetRenameBegin)
				.OnRenameCommit(this, &ST1AssetView::AssetRenameCommit)
				.OnVerifyRenameCommit(this, &ST1AssetView::AssetVerifyRenameCommit)
				.OnItemDestroyed(this, &ST1AssetView::AssetItemWidgetDestroyed)
				.HighlightText( HighlightedText )
				.OnAssetsOrPathsDragDropped(this, &ST1AssetView::OnAssetsOrPathsDragDropped)
				.OnFilesDragDropped(this, &ST1AssetView::OnFilesDragDropped)
				.OnIsAssetValidForCustomToolTip(OnIsAssetValidForCustomToolTip)
				.OnGetCustomAssetToolTip(OnGetCustomAssetToolTip)
				.OnVisualizeAssetToolTip( OnVisualizeAssetToolTip )
				.OnAssetToolTipClosing( OnAssetToolTipClosing )
		);
}

UObject* ST1AssetView::CreateAssetFromTemporary(FString InName, const TSharedPtr<FT1AssetViewAsset>& InItem, FText& OutErrorText)
{
	UObject* Asset = NULL;

	const EAssetItemType::Type ItemType = InItem->GetType();
	if ( ItemType == EAssetItemType::Creation )
	{
		// Committed creation
		TSharedPtr<FAssetViewCreation> CreationItem = StaticCastSharedPtr<FAssetViewCreation>(InItem);
		UFactory* Factory = CreationItem->Factory;
		UClass* AssetClass = CreationItem->AssetClass;
		FString PackagePath = CreationItem->Data.PackagePath.ToString();

		// Remove the temporary item before we do any work to ensure the new item creation is not prevented.
		FilteredAssetItems.Remove(InItem);
		RefreshList();

		if ( AssetClass || Factory )
		{
			FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
			Asset = AssetToolsModule.Get().CreateAsset(InName, PackagePath, AssetClass, Factory, FName("ContentBrowserNewAsset"));
		}

		if ( Asset == NULL )
		{
			OutErrorText = LOCTEXT("AssetCreationFailed", "Failed to create asset.");
		}
	}
	else if ( ItemType == EAssetItemType::Duplication )
	{
		// Committed duplication
		TSharedPtr<FT1AssetViewDuplication> DuplicationItem = StaticCastSharedPtr<FT1AssetViewDuplication>(InItem);
		UObject* SourceObject = DuplicationItem->SourceObject.Get();
		FString PackagePath = DuplicationItem->Data.PackagePath.ToString();

		// Remove the temporary item before we do any work to ensure the new item creation is not prevented.
		FilteredAssetItems.Remove(InItem);
		RefreshList();

		if ( SourceObject )
		{
			FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
			Asset = AssetToolsModule.Get().DuplicateAsset(InName, PackagePath, SourceObject);
		}

		if ( Asset == NULL )
		{
			OutErrorText = LOCTEXT("AssetCreationFailed", "Failed to create asset.");
		}
	}

	return Asset;
}

void ST1AssetView::AssetItemWidgetDestroyed(const TSharedPtr<FT1AssetViewItem>& Item)
{
	if(RenamingAsset.Pin().Get() == Item.Get())
	{
		/* Check if the item is in a temp state and if it is, commit using the default name so that it does not entirely vanish on the user.
		   This keeps the functionality consistent for content to never be in a temporary state */
		if ( Item.IsValid() && Item->IsTemporaryItem() && Item->GetType() != EAssetItemType::Folder )
		{
			FText OutErrorText;
			const TSharedPtr<FT1AssetViewAsset>& ItemAsAsset = StaticCastSharedPtr<FT1AssetViewAsset>(Item);
			CreateAssetFromTemporary(ItemAsAsset->Data.AssetName.ToString(), ItemAsAsset, OutErrorText);

			// Remove the temporary item.
			FilteredAssetItems.Remove(Item);
			RefreshList();
		}

		RenamingAsset.Reset();
	}

	if ( VisibleItems.Remove(Item) != INDEX_NONE )
	{
		bPendingUpdateThumbnails = true;
	}
}

void ST1AssetView::UpdateThumbnails()
{
	int32 MinItemIdx = INDEX_NONE;
	int32 MaxItemIdx = INDEX_NONE;
	int32 MinVisibleItemIdx = INDEX_NONE;
	int32 MaxVisibleItemIdx = INDEX_NONE;

	const int32 HalfNumOffscreenThumbnails = NumOffscreenThumbnails * 0.5;
	for ( auto ItemIt = VisibleItems.CreateConstIterator(); ItemIt; ++ItemIt )
	{
		int32 ItemIdx = FilteredAssetItems.Find(*ItemIt);
		if ( ItemIdx != INDEX_NONE )
		{
			const int32 ItemIdxLow = FMath::Max<int32>(0, ItemIdx - HalfNumOffscreenThumbnails);
			const int32 ItemIdxHigh = FMath::Min<int32>(FilteredAssetItems.Num() - 1, ItemIdx + HalfNumOffscreenThumbnails);
			if ( MinItemIdx == INDEX_NONE || ItemIdxLow < MinItemIdx )
			{
				MinItemIdx = ItemIdxLow;
			}
			if ( MaxItemIdx == INDEX_NONE || ItemIdxHigh > MaxItemIdx )
			{
				MaxItemIdx = ItemIdxHigh;
			}
			if ( MinVisibleItemIdx == INDEX_NONE || ItemIdx < MinVisibleItemIdx )
			{
				MinVisibleItemIdx = ItemIdx;
			}
			if ( MaxVisibleItemIdx == INDEX_NONE || ItemIdx > MaxVisibleItemIdx )
			{
				MaxVisibleItemIdx = ItemIdx;
			}
		}
	}

	if ( MinItemIdx != INDEX_NONE && MaxItemIdx != INDEX_NONE && MinVisibleItemIdx != INDEX_NONE && MaxVisibleItemIdx != INDEX_NONE )
	{
		// We have a new min and a new max, compare it to the old min and max so we can create new thumbnails
		// when appropriate and remove old thumbnails that are far away from the view area.
		TMap< TSharedPtr<FT1AssetViewAsset>, TSharedPtr<FAssetThumbnail> > NewRelevantThumbnails;

		// Operate on offscreen items that are furthest away from the visible items first since the thumbnail pool processes render requests in a LIFO order.
		while (MinItemIdx < MinVisibleItemIdx || MaxItemIdx > MaxVisibleItemIdx)
		{
			const int32 LowEndDistance = MinVisibleItemIdx - MinItemIdx;
			const int32 HighEndDistance = MaxItemIdx - MaxVisibleItemIdx;

			if ( HighEndDistance > LowEndDistance )
			{
				if(FilteredAssetItems.IsValidIndex(MaxItemIdx) && FilteredAssetItems[MaxItemIdx]->GetType() != EAssetItemType::Folder)
				{
					AddItemToNewThumbnailRelevancyMap( StaticCastSharedPtr<FT1AssetViewAsset>(FilteredAssetItems[MaxItemIdx]), NewRelevantThumbnails );
				}
				MaxItemIdx--;
			}
			else
			{
				if(FilteredAssetItems.IsValidIndex(MinItemIdx) && FilteredAssetItems[MinItemIdx]->GetType() != EAssetItemType::Folder)
				{
					AddItemToNewThumbnailRelevancyMap( StaticCastSharedPtr<FT1AssetViewAsset>(FilteredAssetItems[MinItemIdx]), NewRelevantThumbnails );
				}
				MinItemIdx++;
			}
		}

		// Now operate on VISIBLE items then prioritize them so they are rendered first
		TArray< TSharedPtr<FAssetThumbnail> > ThumbnailsToPrioritize;
		for ( int32 ItemIdx = MinVisibleItemIdx; ItemIdx <= MaxVisibleItemIdx; ++ItemIdx )
		{
			if(FilteredAssetItems.IsValidIndex(ItemIdx) && FilteredAssetItems[ItemIdx]->GetType() != EAssetItemType::Folder)
			{
				TSharedPtr<FAssetThumbnail> Thumbnail = AddItemToNewThumbnailRelevancyMap( StaticCastSharedPtr<FT1AssetViewAsset>(FilteredAssetItems[ItemIdx]), NewRelevantThumbnails );
				if ( Thumbnail.IsValid() )
				{
					ThumbnailsToPrioritize.Add(Thumbnail);
				}
			}
		}

		// Now prioritize all thumbnails there were in the visible range
		if ( ThumbnailsToPrioritize.Num() > 0 )
		{
			AssetThumbnailPool->PrioritizeThumbnails(ThumbnailsToPrioritize, CurrentThumbnailSize, CurrentThumbnailSize);
		}

		// Assign the new map of relevant thumbnails. This will remove any entries that were no longer relevant.
		RelevantThumbnails = NewRelevantThumbnails;
	}
}

TSharedPtr<FAssetThumbnail> ST1AssetView::AddItemToNewThumbnailRelevancyMap(const TSharedPtr<FT1AssetViewAsset>& Item, TMap< TSharedPtr<FT1AssetViewAsset>, TSharedPtr<FAssetThumbnail> >& NewRelevantThumbnails)
{
	const TSharedPtr<FAssetThumbnail>* Thumbnail = RelevantThumbnails.Find(Item);
	if ( Thumbnail )
	{
		// The thumbnail is still relevant, add it to the new list
		NewRelevantThumbnails.Add(Item, *Thumbnail);

		return *Thumbnail;
	}
	else
	{
		if ( !ensure(CurrentThumbnailSize > 0 && CurrentThumbnailSize <= MAX_THUMBNAIL_SIZE) )
		{
			// Thumbnail size must be in a sane range
			CurrentThumbnailSize = 64;
		}

		// The thumbnail newly relevant, create a new thumbnail
		const float ThumbnailResolution = CurrentThumbnailSize * MaxThumbnailScale;
		TSharedPtr<FAssetThumbnail> NewThumbnail = MakeShareable( new FAssetThumbnail( Item->Data, ThumbnailResolution, ThumbnailResolution, AssetThumbnailPool ) );
		NewRelevantThumbnails.Add( Item, NewThumbnail );
		NewThumbnail->GetViewportRenderTargetTexture(); // Access the texture once to trigger it to render

		return NewThumbnail;
	}
}

void ST1AssetView::AssetSelectionChanged( TSharedPtr< struct FT1AssetViewItem > AssetItem, ESelectInfo::Type SelectInfo )
{
	if ( !bBulkSelecting )
	{
		if ( AssetItem.IsValid() && AssetItem->GetType() != EAssetItemType::Folder )
		{
			OnAssetSelected.ExecuteIfBound(StaticCastSharedPtr<FT1AssetViewAsset>(AssetItem)->Data);
			OnAssetSelectionChanged.ExecuteIfBound(StaticCastSharedPtr<FT1AssetViewAsset>(AssetItem)->Data, SelectInfo);
		}
		else
		{
			OnAssetSelected.ExecuteIfBound(FAssetData());
			OnAssetSelectionChanged.ExecuteIfBound(FAssetData(), SelectInfo);
		}
	}
}

void ST1AssetView::ItemScrolledIntoView(TSharedPtr<struct FT1AssetViewItem> AssetItem, const TSharedPtr<ITableRow>& Widget )
{
	if ( AssetItem->bRenameWhenScrolledIntoview )
	{
		// Make sure we have window focus to avoid the inline text editor from canceling itself if we try to click on it
		// This can happen if creating an asset opens an intermediary window which steals our focus, 
		// eg, the blueprint and slate widget style class windows (TTP# 314240)
		TSharedPtr<SWindow> OwnerWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
		if(OwnerWindow.IsValid())
		{
			OwnerWindow->BringToFront();
		}

		AwaitingRename = AssetItem;
	}
}

TSharedPtr<SWidget> ST1AssetView::OnGetContextMenuContent()
{
	if ( CanOpenContextMenu() )
	{
		const TArray<FString> SelectedFolders = GetSelectedFolders();
		if(SelectedFolders.Num() > 0)
		{
			return OnGetFolderContextMenu.Execute(SelectedFolders, OnGetPathContextMenuExtender, FT1OnCreateNewFolder::CreateSP(this, &ST1AssetView::OnCreateNewFolder));
		}
		else
		{
			return OnGetAssetContextMenu.Execute(GetSelectedAssets());
		}
	}
	
	return NULL;
}

bool ST1AssetView::CanOpenContextMenu() const
{
	if ( !OnGetAssetContextMenu.IsBound() )
	{
		// You can only a summon a context menu if one is set up
		return false;
	}

	if ( IsThumbnailEditMode() )
	{
		// You can not summon a context menu for assets when in thumbnail edit mode because right clicking may happen inadvertently while adjusting thumbnails.
		return false;
	}

	TArray<FAssetData> SelectedAssets = GetSelectedAssets();

	// Detect if at least one temporary item was selected. If there were no valid assets selected and a temporary one was, then deny the context menu.
	TArray<TSharedPtr<FT1AssetViewItem>> SelectedItems = GetSelectedItems();
	bool bAtLeastOneTemporaryItemFound = false;
	for ( auto ItemIt = SelectedItems.CreateConstIterator(); ItemIt; ++ItemIt )
	{
		const TSharedPtr<FT1AssetViewItem>& Item = *ItemIt;
		if ( Item->IsTemporaryItem() )
		{
			bAtLeastOneTemporaryItemFound = true;
		}
	}

	// If there were no valid assets found, but some invalid assets were found, deny the context menu
	if ( SelectedAssets.Num() == 0 && bAtLeastOneTemporaryItemFound )
	{
		return false;
	}

	if ( SelectedAssets.Num() == 0 && T1SourcesData.HasCollections() )
	{
		// Don't allow a context menu when we're viewing a collection and have no assets selected
		return false;
	}

	// Build a list of selected object paths
	TArray<FString> ObjectPaths;
	for(auto AssetIt = SelectedAssets.CreateConstIterator(); AssetIt; ++AssetIt)
	{
		ObjectPaths.Add( AssetIt->ObjectPath.ToString() );
	}

	bool bLoadSuccessful = true;

	if ( bPreloadAssetsForContextMenu )
	{
		TArray<UObject*> LoadedObjects;
		const bool bAllowedToPrompt = false;
		bLoadSuccessful = T1ContentBrowserUtils::LoadAssetsIfNeeded(ObjectPaths, LoadedObjects, bAllowedToPrompt);
	}

	// Do not show the context menu if the load failed
	return bLoadSuccessful;
}

void ST1AssetView::OnListMouseButtonDoubleClick(TSharedPtr<FT1AssetViewItem> AssetItem)
{
	if ( !ensure(AssetItem.IsValid()) )
	{
		return;
	}

	if ( IsThumbnailEditMode() )
	{
		// You can not activate assets when in thumbnail edit mode because double clicking may happen inadvertently while adjusting thumbnails.
		return;
	}

	if ( AssetItem->GetType() == EAssetItemType::Folder )
	{
		OnPathSelected.ExecuteIfBound(StaticCastSharedPtr<FT1AssetViewFolder>(AssetItem)->FolderPath);
		return;
	}

	if ( AssetItem->IsTemporaryItem() )
	{
		// You may not activate temporary items, they are just for display.
		return;
	}

	TArray<FAssetData> ActivatedAssets;
	ActivatedAssets.Add(StaticCastSharedPtr<FT1AssetViewAsset>(AssetItem)->Data);
	OnAssetsActivated.ExecuteIfBound( ActivatedAssets, EAssetTypeActivationMethod::DoubleClicked );
}

FReply ST1AssetView::OnDraggingAssetItem( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent )
{
	if (bAllowDragging)
	{
		TArray<FAssetData> DraggedAssets;
		TArray<FString> DraggedAssetPaths;

		// Work out which assets to drag
		{
			TArray<FAssetData> AssetDataList = GetSelectedAssets();
			for (const FAssetData& AssetData : AssetDataList)
			{
				// Skip invalid assets and redirectors
				if (AssetData.IsValid() && AssetData.AssetClass != UObjectRedirector::StaticClass()->GetFName())
				{
					DraggedAssets.Add(AssetData);
				}
			}
		}

		// Work out which asset paths to drag
		{
			TArray<FString> SelectedFolders = GetSelectedFolders();
			if (SelectedFolders.Num() > 0 && !T1SourcesData.HasCollections())
			{
				DraggedAssetPaths = MoveTemp(SelectedFolders);
			}
		}

		// Use the custom drag handler?
		if (DraggedAssets.Num() > 0 && FEditorDelegates::OnAssetDragStarted.IsBound())
		{
			FEditorDelegates::OnAssetDragStarted.Broadcast(DraggedAssets, nullptr);
			return FReply::Handled();
		}
		
		// Use the standard drag handler?
		if ((DraggedAssets.Num() > 0 || DraggedAssetPaths.Num() > 0) && MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
		{
			return FReply::Handled().BeginDragDrop(FAssetDragDropOp::New(MoveTemp(DraggedAssets), MoveTemp(DraggedAssetPaths)));
		}
	}

	return FReply::Unhandled();
}

bool ST1AssetView::AssetVerifyRenameCommit(const TSharedPtr<FT1AssetViewItem>& Item, const FText& NewName, const FSlateRect& MessageAnchor, FText& OutErrorMessage)
{
	// Everything other than a folder is considered an asset, including "Creation" and "Duplication"
	// See FAssetViewCreation and FT1AssetViewDuplication
	const bool bIsAssetType = Item->GetType() != EAssetItemType::Folder;

	FString NewNameString = NewName.ToString();
	if ( bIsAssetType )
	{
		const TSharedPtr<FT1AssetViewAsset>& ItemAsAsset = StaticCastSharedPtr<FT1AssetViewAsset>(Item);
		if ( !Item->IsTemporaryItem() && NewNameString == ItemAsAsset->Data.AssetName.ToString() )
		{
			return true;
		}
	}
	else
	{
		const TSharedPtr<FT1AssetViewFolder>& ItemAsFolder = StaticCastSharedPtr<FT1AssetViewFolder>(Item);
		if (NewNameString == ItemAsFolder->FolderName.ToString())
		{
			return true;
		}
	}

	if ( bIsAssetType )
	{
		const TSharedPtr<FT1AssetViewAsset>& ItemAsAsset = StaticCastSharedPtr<FT1AssetViewAsset>(Item);
		const FString NewObjectPath = ItemAsAsset->Data.PackagePath.ToString() / NewNameString + TEXT(".") + NewNameString;
		return T1ContentBrowserUtils::IsValidObjectPathForCreate(NewObjectPath, OutErrorMessage);
	}
	else
	{
		const TSharedPtr<FT1AssetViewFolder>& ItemAsFolder = StaticCastSharedPtr<FT1AssetViewFolder>(Item);
		const FString FolderPath = FPaths::GetPath(ItemAsFolder->FolderPath);
		return T1ContentBrowserUtils::IsValidFolderPathForCreate(FolderPath, NewNameString, OutErrorMessage);
	}

	return true;
}

void ST1AssetView::AssetRenameBegin(const TSharedPtr<FT1AssetViewItem>& Item, const FString& NewName, const FSlateRect& MessageAnchor)
{
	check(!RenamingAsset.IsValid());
	RenamingAsset = Item;
}

void ST1AssetView::AssetRenameCommit(const TSharedPtr<FT1AssetViewItem>& Item, const FString& NewName, const FSlateRect& MessageAnchor, const ETextCommit::Type CommitType)
{
	const EAssetItemType::Type ItemType = Item->GetType();

	// If the item had a factory, create a new object, otherwise rename
	bool bSuccess = false;
	UObject* Asset = NULL;
	FText ErrorMessage;
	if ( ItemType == EAssetItemType::Normal )
	{
		const TSharedPtr<FT1AssetViewAsset>& ItemAsAsset = StaticCastSharedPtr<FT1AssetViewAsset>(Item);

		// Check if the name is different
		if( NewName.Equals(ItemAsAsset->Data.AssetName.ToString(), ESearchCase::CaseSensitive) )
		{
			RenamingAsset.Reset();
			return;
		}

		// Committed rename
		Asset = ItemAsAsset->Data.GetAsset();
		if(Asset == NULL)
		{
			//put back the original name
			RenamingAsset.Reset();
			
			//Notify the user rename fail and link the output log
			FNotificationInfo Info(LOCTEXT("RenameAssetsFailed", "Failed to rename assets"));
			Info.ExpireDuration = 5.0f;
			Info.Hyperlink = FSimpleDelegate::CreateStatic([](){ FGlobalTabmanager::Get()->InvokeTab(FName("OutputLog")); });
			Info.HyperlinkText = LOCTEXT("ShowOutputLogHyperlink", "Show Output Log");
			FSlateNotificationManager::Get().AddNotification(Info);
			
			//Set the content browser error message
			ErrorMessage = LOCTEXT("RenameAssetsFailed", "Failed to rename assets");
		}
		else
		{
			T1ContentBrowserUtils::RenameAsset(Asset, NewName, ErrorMessage);
			bSuccess = true;
		}
	}
	else if ( ItemType == EAssetItemType::Creation || ItemType == EAssetItemType::Duplication )
	{
		if (CommitType == ETextCommit::OnCleared)
		{
			// Clearing the rename box on a newly created asset cancels the entire creation process
			FilteredAssetItems.Remove(Item);
			RefreshList();
		}
		else
		{
			Asset = CreateAssetFromTemporary(NewName, StaticCastSharedPtr<FT1AssetViewAsset>(Item), ErrorMessage);
			bSuccess = Asset != NULL;
		}
	}
	else if( ItemType == EAssetItemType::Folder )
	{
		TArray<FT1MovedContentFolder> MovedFolders;
		const TSharedPtr<FT1AssetViewFolder>& ItemAsFolder = StaticCastSharedPtr<FT1AssetViewFolder>(Item);
		if(ItemAsFolder->bNewFolder)
		{
			ItemAsFolder->bNewFolder = false;

			if (CommitType == ETextCommit::OnCleared)
			{
				// Clearing the rename box on a newly created folder cancels the entire creation process
				FilteredAssetItems.Remove(Item);
				RefreshList();
			}
			else
			{
				const FString NewPath = FPaths::GetPath(ItemAsFolder->FolderPath) / NewName;
				FText ErrorText;
				if( T1ContentBrowserUtils::IsValidFolderName(NewName, ErrorText) &&
					!T1ContentBrowserUtils::DoesFolderExist(NewPath))
				{
					// ensure the folder exists on disk
					FString NewPathOnDisk;
					bSuccess = FPackageName::TryConvertLongPackageNameToFilename(NewPath, NewPathOnDisk) && IFileManager::Get().MakeDirectory(*NewPathOnDisk, true);

					if (bSuccess)
					{
						TSharedRef<FT1EmptyFolderVisibilityManager> EmptyFolderVisibilityManager = FT1ContentBrowserSingleton::Get().GetEmptyFolderVisibilityManager();
						EmptyFolderVisibilityManager->SetAlwaysShowPath(NewPath);

						FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
						bSuccess = AssetRegistryModule.Get().AddPath(NewPath);
					}
				}

				// remove this temp item - a new one will have been added by the asset registry callback
				FilteredAssetItems.Remove(Item);
				RefreshList();

				if(!bSuccess)
				{
					ErrorMessage = LOCTEXT("CreateFolderFailed", "Failed to create folder.");
				}
			}
		}
		else if(NewName != ItemAsFolder->FolderName.ToString())
		{
			FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

			// first create the new folder
			const FString NewPath = FPaths::GetPath(ItemAsFolder->FolderPath) / NewName;
			FText ErrorText;
			if( T1ContentBrowserUtils::IsValidFolderName(NewName, ErrorText) &&
				!T1ContentBrowserUtils::DoesFolderExist(NewPath))
			{
				// ensure the folder exists on disk
				FString NewPathOnDisk;
				bSuccess = FPackageName::TryConvertLongPackageNameToFilename(NewPath, NewPathOnDisk) && IFileManager::Get().MakeDirectory(*NewPathOnDisk, true);

				if (bSuccess)
				{
					bSuccess = AssetRegistryModule.Get().AddPath(NewPath);
				}
			}

			if(bSuccess)
			{
				MovedFolders.Add(FT1MovedContentFolder(ItemAsFolder->FolderPath, NewPath));
				// move any assets in our folder
				TArray<FAssetData> AssetsInFolder;
				AssetRegistryModule.Get().GetAssetsByPath(*ItemAsFolder->FolderPath, AssetsInFolder, true);
				TArray<UObject*> ObjectsInFolder;
				T1ContentBrowserUtils::GetObjectsInAssetData(AssetsInFolder, ObjectsInFolder);
				T1ContentBrowserUtils::MoveAssets(ObjectsInFolder, NewPath, ItemAsFolder->FolderPath);

				// Now check to see if the original folder is empty, if so we can delete it
				TArray<FAssetData> AssetsInOriginalFolder;
				AssetRegistryModule.Get().GetAssetsByPath(*ItemAsFolder->FolderPath, AssetsInOriginalFolder, true);
				if(AssetsInOriginalFolder.Num() == 0)
				{
					TArray<FString> FoldersToDelete;
					FoldersToDelete.Add(ItemAsFolder->FolderPath);
					T1ContentBrowserUtils::DeleteFolders(FoldersToDelete);
				}
			}
			OnFolderPathChanged.ExecuteIfBound(MovedFolders);
			RequestQuickFrontendListRefresh();
		}		
	}
	else
	{
		// Unknown AssetItemType
		ensure(0);
	}

	if ( bSuccess )
	{
		// Sort in the new item
		bPendingSortFilteredItems = true;
		RequestQuickFrontendListRefresh();

		if ( ItemType == EAssetItemType::Folder )
		{
			const TSharedPtr<FT1AssetViewFolder>& ItemAsFolder = StaticCastSharedPtr<FT1AssetViewFolder>(Item);
			const FString NewPath = FPaths::GetPath(ItemAsFolder->FolderPath) / NewName;

			// Sync the view to the new folder
			TArray<FString> FolderList;
			FolderList.Add(NewPath);
			SyncToFolders(FolderList);
		}
		else
		{
			if ( ensure(Asset != NULL) )
			{
				// Refresh the thumbnail
				const TSharedPtr<FAssetThumbnail>* AssetThumbnail = RelevantThumbnails.Find(StaticCastSharedPtr<FT1AssetViewAsset>(Item));
				if ( AssetThumbnail )
				{
					AssetThumbnailPool->RefreshThumbnail(*AssetThumbnail);
				}

				// Sync to its location
				TArray<FAssetData> AssetDataList;
				new(AssetDataList) FAssetData(Asset);

				if ( OnAssetRenameCommitted.IsBound() && !bUserSearching)
				{
					// If our parent wants to potentially handle the sync, let it, but only if we're not currently searching (or it would cancel the search)
					OnAssetRenameCommitted.Execute(AssetDataList); 
				}
				else
				{
					// Otherwise, sync just the view
					SyncToAssets(AssetDataList);
				}
			}
		}
	}
	else if ( !ErrorMessage.IsEmpty() )
	{
		// Prompt the user with the reason the rename/creation failed
		T1ContentBrowserUtils::DisplayMessage(ErrorMessage, MessageAnchor, SharedThis(this));
	}

	RenamingAsset.Reset();
}

bool ST1AssetView::IsRenamingAsset() const
{
	return RenamingAsset.IsValid();
}

bool ST1AssetView::ShouldAllowToolTips() const
{
	bool bIsRightClickScrolling = false;
	switch( CurrentViewType )
	{
		case ET1AssetViewType::List:
			bIsRightClickScrolling = ListView->IsRightClickScrolling();
			break;

		case ET1AssetViewType::Tile:
			bIsRightClickScrolling = TileView->IsRightClickScrolling();
			break;

		case ET1AssetViewType::Column:
			bIsRightClickScrolling = ColumnView->IsRightClickScrolling();
			break;

		default:
			bIsRightClickScrolling = false;
			break;
	}

	return !bIsRightClickScrolling && !IsThumbnailEditMode() && !IsRenamingAsset();
}

bool ST1AssetView::IsThumbnailEditMode() const
{
	return IsThumbnailEditModeAllowed() && bThumbnailEditMode;
}

bool ST1AssetView::IsThumbnailEditModeAllowed() const
{
	return bAllowThumbnailEditMode && GetCurrentViewType() != ET1AssetViewType::Column;
}

FReply ST1AssetView::EndThumbnailEditModeClicked()
{
	bThumbnailEditMode = false;

	return FReply::Handled();
}

FText ST1AssetView::GetAssetCountText() const
{
	const int32 NumAssets = FilteredAssetItems.Num();
	const int32 NumSelectedAssets = GetSelectedItems().Num();

	FText AssetCount = FText::GetEmpty();
	if ( NumSelectedAssets == 0 )
	{
		if ( NumAssets == 1 )
		{
			AssetCount = LOCTEXT("AssetCountLabelSingular", "1 item");
		}
		else
		{
			AssetCount = FText::Format( LOCTEXT("AssetCountLabelPlural", "{0} items"), FText::AsNumber(NumAssets) );
		}
	}
	else
	{
		if ( NumAssets == 1 )
		{
			AssetCount = FText::Format( LOCTEXT("AssetCountLabelSingularPlusSelection", "1 item ({0} selected)"), FText::AsNumber(NumSelectedAssets) );
		}
		else
		{
			AssetCount = FText::Format( LOCTEXT("AssetCountLabelPluralPlusSelection", "{0} items ({1} selected)"), FText::AsNumber(NumAssets), FText::AsNumber(NumSelectedAssets) );
		}
	}

	return AssetCount;
}

EVisibility ST1AssetView::GetEditModeLabelVisibility() const
{
	return IsThumbnailEditMode() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility ST1AssetView::GetListViewVisibility() const
{
	return GetCurrentViewType() == ET1AssetViewType::List ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility ST1AssetView::GetTileViewVisibility() const
{
	return GetCurrentViewType() == ET1AssetViewType::Tile ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility ST1AssetView::GetColumnViewVisibility() const
{
	return GetCurrentViewType() == ET1AssetViewType::Column ? EVisibility::Visible : EVisibility::Collapsed;
}

void ST1AssetView::ToggleThumbnailEditMode()
{
	bThumbnailEditMode = !bThumbnailEditMode;
}

float ST1AssetView::GetThumbnailScale() const
{
	return ThumbnailScaleSliderValue.Get();
}

void ST1AssetView::SetThumbnailScale( float NewValue )
{
	ThumbnailScaleSliderValue = NewValue;
	RefreshList();
}

bool ST1AssetView::IsThumbnailScalingLocked() const
{
	return GetCurrentViewType() == ET1AssetViewType::Column;
}

float ST1AssetView::GetListViewItemHeight() const
{
	return (ListViewThumbnailSize + ListViewThumbnailPadding * 2) * FMath::Lerp(MinThumbnailScale, MaxThumbnailScale, GetThumbnailScale());
}

float ST1AssetView::GetTileViewItemHeight() const
{
	return TileViewNameHeight + GetTileViewItemBaseHeight() * FillScale;
}

float ST1AssetView::GetTileViewItemBaseHeight() const
{
	return (TileViewThumbnailSize + TileViewThumbnailPadding * 2) * FMath::Lerp(MinThumbnailScale, MaxThumbnailScale, GetThumbnailScale());
}

float ST1AssetView::GetTileViewItemWidth() const
{
	return GetTileViewItemBaseWidth() * FillScale;
}

float ST1AssetView::GetTileViewItemBaseWidth() const //-V524
{
	return ( TileViewThumbnailSize + TileViewThumbnailPadding * 2 ) * FMath::Lerp( MinThumbnailScale, MaxThumbnailScale, GetThumbnailScale() );
}

EColumnSortMode::Type ST1AssetView::GetColumnSortMode(const FName ColumnId) const
{
	for (int32 PriorityIdx = 0; PriorityIdx < EColumnSortPriority::Max; PriorityIdx++)
	{
		const EColumnSortPriority::Type SortPriority = static_cast<EColumnSortPriority::Type>(PriorityIdx);
		if (ColumnId == SortManager.GetSortColumnId(SortPriority))
		{
			return SortManager.GetSortMode(SortPriority);
		}
	}
	return EColumnSortMode::None;
}

EColumnSortPriority::Type ST1AssetView::GetColumnSortPriority(const FName ColumnId) const
{
	for (int32 PriorityIdx = 0; PriorityIdx < EColumnSortPriority::Max; PriorityIdx++)
	{
		const EColumnSortPriority::Type SortPriority = static_cast<EColumnSortPriority::Type>(PriorityIdx);
		if (ColumnId == SortManager.GetSortColumnId(SortPriority))
		{
			return SortPriority;
		}
	}
	return EColumnSortPriority::Primary;
}

void ST1AssetView::OnSortColumnHeader(const EColumnSortPriority::Type SortPriority, const FName& ColumnId, const EColumnSortMode::Type NewSortMode)
{
	SortManager.SetSortColumnId(SortPriority, ColumnId);
	SortManager.SetSortMode(SortPriority, NewSortMode);
	SortList();
}

EVisibility ST1AssetView::IsAssetShowWarningTextVisible() const
{
	return FilteredAssetItems.Num() > 0 ? EVisibility::Collapsed : EVisibility::HitTestInvisible;
}

FText ST1AssetView::GetAssetShowWarningText() const
{
	if (AssetShowWarningText.IsSet())
	{
		return AssetShowWarningText.Get();
	}
	
	FText NothingToShowText, DropText;
	if (ShouldFilterRecursively())
	{
		NothingToShowText = LOCTEXT( "NothingToShowCheckFilter", "No results, check your filter." );
	}

	if ( T1SourcesData.HasCollections() && !T1SourcesData.IsDynamicCollection() )
	{
		DropText = LOCTEXT( "DragAssetsHere", "Drag and drop assets here to add them to the collection." );
	}
	else if ( OnGetAssetContextMenu.IsBound() )
	{
		DropText = LOCTEXT( "DropFilesOrRightClick", "Drop files here or right click to create content." );
	}
	
	return NothingToShowText.IsEmpty() ? DropText : FText::Format(LOCTEXT("NothingToShowPattern", "{0}\n\n{1}"), NothingToShowText, DropText);
}

bool ST1AssetView::HasSingleCollectionSource() const
{
	return ( T1SourcesData.Collections.Num() == 1 && T1SourcesData.PackagePaths.Num() == 0 );
}

void ST1AssetView::OnAssetsOrPathsDragDropped(const TArray<FAssetData>& AssetList, const TArray<FString>& AssetPaths, const FString& DestinationPath)
{
	T1DragDropHandler::HandleDropOnAssetFolder(
		SharedThis(this), 
		AssetList, 
		AssetPaths, 
		DestinationPath, 
		FText::FromString(FPaths::GetCleanFilename(DestinationPath)), 
		T1DragDropHandler::FExecuteCopyOrMove::CreateSP(this, &ST1AssetView::ExecuteDropCopy),
		T1DragDropHandler::FExecuteCopyOrMove::CreateSP(this, &ST1AssetView::ExecuteDropMove)
		);
}

void ST1AssetView::OnFilesDragDropped(const TArray<FString>& AssetList, const FString& DestinationPath)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
	AssetToolsModule.Get().ImportAssets( AssetList, DestinationPath );
}

void ST1AssetView::ExecuteDropCopy(TArray<FAssetData> AssetList, TArray<FString> AssetPaths, FString DestinationPath)
{
	int32 NumItemsCopied = 0;

	if (AssetList.Num() > 0)
	{
		TArray<UObject*> DroppedObjects;
		T1ContentBrowserUtils::GetObjectsInAssetData(AssetList, DroppedObjects);

		TArray<UObject*> NewObjects;
		ObjectTools::DuplicateObjects(DroppedObjects, TEXT(""), DestinationPath, /*bOpenDialog=*/false, &NewObjects);

		NumItemsCopied += NewObjects.Num();
	}

	if (AssetPaths.Num() > 0)
	{
		if (T1ContentBrowserUtils::CopyFolders(AssetPaths, DestinationPath))
		{
			NumItemsCopied += AssetPaths.Num();
		}
	}

	// If any items were duplicated, report the success
	if (NumItemsCopied > 0)
	{
		const FText Message = FText::Format(LOCTEXT("AssetItemsDroppedCopy", "{0} {0}|plural(one=item,other=items) copied"), NumItemsCopied);
		const FVector2D& CursorPos = FSlateApplication::Get().GetCursorPos();
		FSlateRect MessageAnchor(CursorPos.X, CursorPos.Y, CursorPos.X, CursorPos.Y);
		T1ContentBrowserUtils::DisplayMessage(Message, MessageAnchor, SharedThis(this));
	}
}

void ST1AssetView::ExecuteDropMove(TArray<FAssetData> AssetList, TArray<FString> AssetPaths, FString DestinationPath)
{
	if (AssetList.Num() > 0)
	{
		TArray<UObject*> DroppedObjects;
		T1ContentBrowserUtils::GetObjectsInAssetData(AssetList, DroppedObjects);

		T1ContentBrowserUtils::MoveAssets(DroppedObjects, DestinationPath);
	}

	// Prepare to fixup any asset paths that are favorites
	TArray<FT1MovedContentFolder> MovedFolders;
	for (const FString& OldPath : AssetPaths)
	{
		const FString SubFolderName = FPackageName::GetLongPackageAssetName(OldPath);
		const FString NewPath = DestinationPath + TEXT("/") + SubFolderName;
		MovedFolders.Add(FT1MovedContentFolder(OldPath, NewPath));
	}

	if (AssetPaths.Num() > 0)
	{
		T1ContentBrowserUtils::MoveFolders(AssetPaths, DestinationPath);
	}

	OnFolderPathChanged.ExecuteIfBound(MovedFolders);
}

void ST1AssetView::SetUserSearching(bool bInSearching)
{
	if(bUserSearching != bInSearching)
	{
		RequestSlowFullListRefresh();
	}
	bUserSearching = bInSearching;
}

void ST1AssetView::HandleSettingChanged(FName PropertyName)
{
	if ((PropertyName == GET_MEMBER_NAME_CHECKED(UContentBrowserSettings, DisplayFolders)) ||
		(PropertyName == GET_MEMBER_NAME_CHECKED(UContentBrowserSettings, DisplayEmptyFolders)) ||
		(PropertyName == "DisplayDevelopersFolder") ||
		(PropertyName == "DisplayEngineFolder") ||
		(PropertyName == NAME_None))	// @todo: Needed if PostEditChange was called manually, for now
	{
		RequestSlowFullListRefresh();
	}
}

FText ST1AssetView::GetQuickJumpTerm() const
{
	return FText::FromString(QuickJumpData.JumpTerm);
}

EVisibility ST1AssetView::IsQuickJumpVisible() const
{
	return (QuickJumpData.JumpTerm.IsEmpty()) ? EVisibility::Collapsed : EVisibility::HitTestInvisible;
}

FSlateColor ST1AssetView::GetQuickJumpColor() const
{
	return FEditorStyle::GetColor((QuickJumpData.bHasValidMatch) ? "InfoReporting.BackgroundColor" : "ErrorReporting.BackgroundColor");
}

void ST1AssetView::ResetQuickJump()
{
	QuickJumpData.JumpTerm.Empty();
	QuickJumpData.bIsJumping = false;
	QuickJumpData.bHasChangedSinceLastTick = false;
	QuickJumpData.bHasValidMatch = false;
}

FReply ST1AssetView::HandleQuickJumpKeyDown(const TCHAR InCharacter, const bool bIsControlDown, const bool bIsAltDown, const bool bTestOnly)
{
	// Check for special characters
	if(bIsControlDown || bIsAltDown)
	{
		return FReply::Unhandled();
	}

	// Check for invalid characters
	for(int InvalidCharIndex = 0; InvalidCharIndex < ARRAY_COUNT(INVALID_OBJECTNAME_CHARACTERS) - 1; ++InvalidCharIndex)
	{
		if(InCharacter == INVALID_OBJECTNAME_CHARACTERS[InvalidCharIndex])
		{
			return FReply::Unhandled();
		}
	}

	switch(InCharacter)
	{
	// Ignore some other special characters that we don't want to be entered into the buffer
	case 0:		// Any non-character key press, e.g. f1-f12, Delete, Pause/Break, etc.
				// These should be explicitly not handled so that their input bindings are handled higher up the chain.

	case 8:		// Backspace
	case 13:	// Enter
	case 27:	// Esc
		return FReply::Unhandled();

	default:
		break;
	}

	// Any other character!
	if(!bTestOnly)
	{
		QuickJumpData.JumpTerm.AppendChar(InCharacter);
		QuickJumpData.bHasChangedSinceLastTick = true;
	}

	return FReply::Handled();
}

bool ST1AssetView::PerformQuickJump(const bool bWasJumping)
{
	auto GetAssetViewItemName = [](const TSharedPtr<FT1AssetViewItem> &Item) -> FString
	{
		switch(Item->GetType())
		{
		case EAssetItemType::Normal:
			{
				const TSharedPtr<FT1AssetViewAsset>& ItemAsAsset = StaticCastSharedPtr<FT1AssetViewAsset>(Item);
				return ItemAsAsset->Data.AssetName.ToString();
			}

		case EAssetItemType::Folder:
			{
				const TSharedPtr<FT1AssetViewFolder>& ItemAsFolder = StaticCastSharedPtr<FT1AssetViewFolder>(Item);
				return ItemAsFolder->FolderName.ToString();
			}

		default:
			return FString();
		}
	};

	auto JumpToNextMatch = [this, &GetAssetViewItemName](const int StartIndex, const int EndIndex) -> bool
	{
		check(StartIndex >= 0);
		check(EndIndex <= FilteredAssetItems.Num());

		for(int NewSelectedItemIndex = StartIndex; NewSelectedItemIndex < EndIndex; ++NewSelectedItemIndex)
		{
			TSharedPtr<FT1AssetViewItem>& NewSelectedItem = FilteredAssetItems[NewSelectedItemIndex];
			const FString NewSelectedItemName = GetAssetViewItemName(NewSelectedItem);
			if(NewSelectedItemName.StartsWith(QuickJumpData.JumpTerm, ESearchCase::IgnoreCase))
			{
				SetSelection(NewSelectedItem);
				RequestScrollIntoView(NewSelectedItem);
				return true;
			}
		}

		return false;
	};

	TArray<TSharedPtr<FT1AssetViewItem>> SelectedItems = GetSelectedItems();
	TSharedPtr<FT1AssetViewItem> SelectedItem = (SelectedItems.Num()) ? SelectedItems[0] : nullptr;

	// If we have a selection, and we were already jumping, first check to see whether 
	// the current selection still matches the quick-jump term; if it does, we do nothing
	if(bWasJumping && SelectedItem.IsValid())
	{
		const FString SelectedItemName = GetAssetViewItemName(SelectedItem);
		if(SelectedItemName.StartsWith(QuickJumpData.JumpTerm, ESearchCase::IgnoreCase))
		{
			return true;
		}
	}

	// We need to move on to the next match in FilteredAssetItems that starts with the given quick-jump term
	const int SelectedItemIndex = (SelectedItem.IsValid()) ? FilteredAssetItems.Find(SelectedItem) : INDEX_NONE;
	const int StartIndex = (SelectedItemIndex == INDEX_NONE) ? 0 : SelectedItemIndex + 1;
	
	bool ValidMatch = JumpToNextMatch(StartIndex, FilteredAssetItems.Num());
	if(!ValidMatch && StartIndex > 0)
	{
		// If we didn't find a match, we need to loop around and look again from the start (assuming we weren't already)
		return JumpToNextMatch(0, StartIndex);
	}

	return ValidMatch;
}

void ST1AssetView::FillToggleColumnsMenu(FMenuBuilder& MenuBuilder)
{
	// Column view may not be valid if we toggled off columns view while the columns menu was open
	if(ColumnView.IsValid())
	{
		const TIndirectArray<SHeaderRow::FColumn> Columns = ColumnView->GetHeaderRow()->GetColumns();

		for (int32 ColumnIndex = 0; ColumnIndex < Columns.Num(); ++ColumnIndex)
		{
			const FString ColumnName = Columns[ColumnIndex].ColumnId.ToString();

			MenuBuilder.AddMenuEntry(
				Columns[ColumnIndex].DefaultText,
				LOCTEXT("ShowHideColumnTooltip", "Show or hide column"),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateSP(this, &ST1AssetView::ToggleColumn, ColumnName),
					FCanExecuteAction::CreateSP(this, &ST1AssetView::CanToggleColumn, ColumnName),
					FIsActionChecked::CreateSP(this, &ST1AssetView::IsColumnVisible, ColumnName),
					EUIActionRepeatMode::RepeatEnabled
				),
				NAME_None,
				EUserInterfaceActionType::Check
			);
		}
	}
}

void ST1AssetView::ResetColumns()
{
	HiddenColumnNames.Empty();
	NumVisibleColumns = ColumnView->GetHeaderRow()->GetColumns().Num();
	ColumnView->GetHeaderRow()->RefreshColumns();
	ColumnView->RebuildList();
}

void ST1AssetView::ExportColumns()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	const void* ParentWindowWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

	const FText Title = LOCTEXT("ExportToCSV", "Export columns as CSV...");
	const FString FileTypes = TEXT("Data Table CSV (*.csv)|*.csv");

	TArray<FString> OutFilenames;
	DesktopPlatform->SaveFileDialog(
		ParentWindowWindowHandle,
		Title.ToString(),
		TEXT(""),
		TEXT("Report.csv"),
		FileTypes,
		EFileDialogFlags::None,
		OutFilenames
	);

	if (OutFilenames.Num() > 0)
	{
		const TIndirectArray<SHeaderRow::FColumn>& Columns = ColumnView->GetHeaderRow()->GetColumns();

		TArray<FName> ColumnNames;
		for (const SHeaderRow::FColumn& Column : Columns)
		{
			ColumnNames.Add(Column.ColumnId);
		}

		FString SaveString;
		SortManager.ExportColumnsToCSV(FilteredAssetItems, ColumnNames, CustomColumns, SaveString);

		FFileHelper::SaveStringToFile(SaveString, *OutFilenames[0]);
	}
}

void ST1AssetView::ToggleColumn(const FString ColumnName)
{
	SetColumnVisibility(ColumnName, HiddenColumnNames.Contains(ColumnName));
}

void ST1AssetView::SetColumnVisibility(const FString ColumnName, const bool bShow)
{
	if (!bShow)
	{
		--NumVisibleColumns;
		HiddenColumnNames.Add(ColumnName);
	}
	else
	{
		++NumVisibleColumns;
		check(HiddenColumnNames.Contains(ColumnName));
		HiddenColumnNames.Remove(ColumnName);
	}

	ColumnView->GetHeaderRow()->RefreshColumns();
	ColumnView->RebuildList();
}

bool ST1AssetView::CanToggleColumn(const FString ColumnName) const
{
	return (HiddenColumnNames.Contains(ColumnName) || NumVisibleColumns > 1);
}

bool ST1AssetView::IsColumnVisible(const FString ColumnName) const
{
	return !HiddenColumnNames.Contains(ColumnName);
}

bool ST1AssetView::ShouldColumnGenerateWidget(const FString ColumnName) const
{
	return !HiddenColumnNames.Contains(ColumnName);
}

TSharedRef<SWidget> ST1AssetView::CreateRowHeaderMenuContent(const FString ColumnName)
{
	FMenuBuilder MenuBuilder(true, NULL);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("HideColumn", "Hide Column"),
		LOCTEXT("HideColumnToolTip", "Hides this column."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateSP(this, &ST1AssetView::SetColumnVisibility, ColumnName, false), FCanExecuteAction::CreateSP(this, &ST1AssetView::CanToggleColumn, ColumnName)),
		NAME_None,
		EUserInterfaceActionType::Button);

	return MenuBuilder.MakeWidget();
}

void ST1AssetView::ForceShowPluginFolder(bool bEnginePlugin)
{
	if (bEnginePlugin && !IsShowingEngineContent())
	{
		ToggleShowEngineContent();
	}

	if (!IsShowingPluginContent())
	{
		ToggleShowPluginContent();
	}
}

#undef LOCTEXT_NAMESPACE
