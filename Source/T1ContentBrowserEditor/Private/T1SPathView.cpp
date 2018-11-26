// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "T1SPathView.h"
#include "HAL/FileManager.h"
#include "Misc/ConfigCacheIni.h"
#include "Layout/WidgetPath.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SSeparator.h"
#include "EditorStyleSet.h"
#include "Settings/ContentBrowserSettings.h"
#include "AssetRegistryModule.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "T1ContentBrowserSingleton.h"
#include "T1ContentBrowserUtils.h"
#include "T1HistoryManager.h"

#include "DragAndDrop/AssetDragDropOp.h"
#include "T1DragDropHandler.h"

#include "T1PathViewTypes.h"
#include "T1SourcesViewWidgets.h"
#include "Widgets/Input/SSearchBox.h"
#include "T1NativeClassHierarchy.h"
#include "T1EmptyFolderVisibilityManager.h"

#define LOCTEXT_NAMESPACE "T1ContentBrowser"

ST1PathView::~ST1PathView()
{
	// Unsubscribe from content path events
	FPackageName::OnContentPathMounted().RemoveAll( this );
	FPackageName::OnContentPathDismounted().RemoveAll( this );

	// Unsubscribe from class events
	if ( bAllowClassesFolder )
	{
		TSharedRef<FT1NativeClassHierarchy> NativeClassHierarchy = FT1ContentBrowserSingleton::Get().GetNativeClassHierarchy();
		NativeClassHierarchy->OnClassHierarchyUpdated().RemoveAll( this );
	}

	// Unsubscribe from folder population events
	{
		TSharedRef<FT1EmptyFolderVisibilityManager> EmptyFolderVisibilityManager = FT1ContentBrowserSingleton::Get().GetEmptyFolderVisibilityManager();
		EmptyFolderVisibilityManager->OnFolderPopulated().RemoveAll(this);
	}

	// Load the asset registry module to stop listening for updates
	FAssetRegistryModule* AssetRegistryModule = FModuleManager::GetModulePtr<FAssetRegistryModule>(TEXT("AssetRegistry"));
	if(AssetRegistryModule)
	{
		AssetRegistryModule->Get().OnPathAdded().RemoveAll(this);
		AssetRegistryModule->Get().OnPathRemoved().RemoveAll(this);
		AssetRegistryModule->Get().OnFilesLoaded().RemoveAll(this);
	}

	SearchBoxFolderFilter->OnChanged().RemoveAll( this );
}

void ST1PathView::Construct( const FArguments& InArgs )
{
	OnPathSelected = InArgs._OnPathSelected;
	bAllowContextMenu = InArgs._AllowContextMenu;
	OnGetFolderContextMenu = InArgs._OnGetFolderContextMenu;
	OnGetPathContextMenuExtender = InArgs._OnGetPathContextMenuExtender;
	bAllowClassesFolder = InArgs._AllowClassesFolder;
	PreventTreeItemChangedDelegateCount = 0;
	TreeTitle = LOCTEXT("AssetTreeTitle", "Asset Tree");
	if ( InArgs._FocusSearchBoxWhenOpened )
	{
		RegisterActiveTimer( 0.f, FWidgetActiveTimerDelegate::CreateSP( this, &ST1PathView::SetFocusPostConstruct ) );
	}

	// Listen for when view settings are changed
	UContentBrowserSettings::OnSettingChanged().AddSP(this, &ST1PathView::HandleSettingChanged);

	//Setup the SearchBox filter
	SearchBoxFolderFilter = MakeShareable( new FolderTextFilter( FolderTextFilter::FItemToStringArray::CreateSP( this, &ST1PathView::PopulateFolderSearchStrings ) ) );
	SearchBoxFolderFilter->OnChanged().AddSP( this, &ST1PathView::FilterUpdated );

	// Listen to find out when new game content paths are mounted or dismounted, so that we can refresh our root set of paths
	FPackageName::OnContentPathMounted().AddSP( this, &ST1PathView::OnContentPathMountedOrDismounted );
	FPackageName::OnContentPathDismounted().AddSP( this, &ST1PathView::OnContentPathMountedOrDismounted );

	// Listen to find out when the available classes are changed, so that we can refresh our paths
	if ( bAllowClassesFolder )
	{
		TSharedRef<FT1NativeClassHierarchy> NativeClassHierarchy = FT1ContentBrowserSingleton::Get().GetNativeClassHierarchy();
		NativeClassHierarchy->OnClassHierarchyUpdated().AddSP( this, &ST1PathView::OnClassHierarchyUpdated );
	}

	// Listen to find out when previously empty paths are populated with content
	{
		TSharedRef<FT1EmptyFolderVisibilityManager> EmptyFolderVisibilityManager = FT1ContentBrowserSingleton::Get().GetEmptyFolderVisibilityManager();
		EmptyFolderVisibilityManager->OnFolderPopulated().AddSP(this, &ST1PathView::OnFolderPopulated);
	}

	if (!TreeViewPtr.IsValid())
	{
		SAssignNew(TreeViewPtr, STreeView< TSharedPtr<FT1TreeItem> >)
			.TreeItemsSource(&TreeRootItems)
			.OnGenerateRow(this, &ST1PathView::GenerateTreeRow)
			.OnItemScrolledIntoView(this, &ST1PathView::TreeItemScrolledIntoView)
			.ItemHeight(18)
			.SelectionMode(InArgs._SelectionMode)
			.OnSelectionChanged(this, &ST1PathView::TreeSelectionChanged)
			.OnExpansionChanged(this, &ST1PathView::TreeExpansionChanged)
			.OnGetChildren(this, &ST1PathView::GetChildrenForTree)
			.OnSetExpansionRecursive(this, &ST1PathView::SetTreeItemExpansionRecursive)
			.OnContextMenuOpening(this, &ST1PathView::MakePathViewContextMenu)
			.ClearSelectionOnClick(false);
	}

	ChildSlot
	[
		SNew(SVerticalBox)

		// Search
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0, 1, 0, 3)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				InArgs._SearchContent.Widget
			]

			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SAssignNew(SearchBoxPtr, SSearchBox)
				.Visibility(InArgs._SearchBarVisibility)
				.HintText( LOCTEXT( "AssetTreeSearchBoxHint", "Search Folders" ) )
				.OnTextChanged( this, &ST1PathView::OnAssetTreeSearchBoxChanged )
				.OnTextCommitted( this, &ST1PathView::OnAssetTreeSearchBoxCommitted )
			]
		]

		// Tree title
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Font( FEditorStyle::GetFontStyle("T1ContentBrowser.SourceTitleFont") )
			.Text(this, &ST1PathView::GetTreeTitle)
			.Visibility(InArgs._ShowTreeTitle ? EVisibility::Visible : EVisibility::Collapsed)
		]

		// Separator
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0, 0, 0, 1)
		[
			SNew(SSeparator)
			.Visibility( ( InArgs._ShowSeparator) ? EVisibility::Visible : EVisibility::Collapsed )
		]
			
		// Tree
		+SVerticalBox::Slot()
		.FillHeight(1.f)
		[
			TreeViewPtr.ToSharedRef()
		]
	];

	// Load the asset registry module to listen for updates
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	AssetRegistryModule.Get().OnPathAdded().AddSP( this, &ST1PathView::OnAssetRegistryPathAdded );
	AssetRegistryModule.Get().OnPathRemoved().AddSP( this, &ST1PathView::OnAssetRegistryPathRemoved );
	AssetRegistryModule.Get().OnFilesLoaded().AddSP( this, &ST1PathView::OnAssetRegistrySearchCompleted );

	// Add all paths currently gathered from the asset registry
	Populate();

	// Always expand the game root initially
	static const FString GameRootName = TEXT("Game");
	for ( auto RootIt = TreeRootItems.CreateConstIterator(); RootIt; ++RootIt )
	{
		if ( (*RootIt)->FolderName == GameRootName )
		{
			TreeViewPtr->SetItemExpansion(*RootIt, true);
		}
	}
}

void ST1PathView::SetSelectedPaths(const TArray<FString>& Paths)
{
	if ( !ensure(TreeViewPtr.IsValid()) )
	{
		return;
	}

	if ( !SearchBoxPtr->GetText().IsEmpty() )
	{
		// Clear the search box so the selected paths will be visible
		SearchBoxPtr->SetText( FText::GetEmpty() );
	}

	// Prevent the selection changed delegate since the invoking code requested it
	FScopedPreventTreeItemChangedDelegate DelegatePrevention( SharedThis(this) );

	// If the selection was changed before all pending initial paths were found, stop attempting to select them
	PendingInitialPaths.Empty();

	// Clear the selection to start, then add the selected paths as they are found
	TreeViewPtr->ClearSelection();

	for (int32 PathIdx = 0; PathIdx < Paths.Num(); ++PathIdx)
	{
		const FString& Path = Paths[PathIdx];

		TArray<FString> PathItemList;
		Path.ParseIntoArray(PathItemList, TEXT("/"), /*InCullEmpty=*/true);

		if ( PathItemList.Num() )
		{
			// There is at least one element in the path
			TArray<TSharedPtr<FT1TreeItem>> TreeItems;

			// Find the first item in the root items list
			for ( int32 RootItemIdx = 0; RootItemIdx < TreeRootItems.Num(); ++RootItemIdx )
			{
				if ( TreeRootItems[RootItemIdx]->FolderName == PathItemList[0] )
				{
					// Found the first item in the path
					TreeItems.Add(TreeRootItems[RootItemIdx]);
					break;
				}
			}

			// If found in the root items list, try to find the childmost item matching the path
			if ( TreeItems.Num() > 0 )
			{
				for ( int32 PathItemIdx = 1; PathItemIdx < PathItemList.Num(); ++PathItemIdx )
				{
					const FString& PathItemName = PathItemList[PathItemIdx];
					const TSharedPtr<FT1TreeItem> ChildItem = TreeItems.Last()->GetChild(PathItemName);

					if ( ChildItem.IsValid() )
					{
						// Update tree items list
						TreeItems.Add(ChildItem);
					}
					else
					{
						// Could not find the child item
						break;
					}
				}

				// Expand all the tree folders up to but not including the last one.
				for (int32 ItemIdx = 0; ItemIdx < TreeItems.Num() - 1; ++ItemIdx)
				{
					TreeViewPtr->SetItemExpansion(TreeItems[ItemIdx], true);
				}

				// Set the selection to the closest found folder and scroll it into view
				TreeViewPtr->SetItemSelection(TreeItems.Last(), true);
				TreeViewPtr->RequestScrollIntoView(TreeItems.Last());
			}
			else
			{
				// Could not even find the root path... skip
			}
		}
		else
		{
			// No path items... skip
		}
	}
}

void ST1PathView::ClearSelection()
{
	// Prevent the selection changed delegate since the invoking code requested it
	FScopedPreventTreeItemChangedDelegate DelegatePrevention( SharedThis(this) );

	// If the selection was changed before all pending initial paths were found, stop attempting to select them
	PendingInitialPaths.Empty();

	// Clear the selection to start, then add the selected paths as they are found
	TreeViewPtr->ClearSelection();
}

FString ST1PathView::GetSelectedPath() const
{
	TArray<TSharedPtr<FT1TreeItem>> Items = TreeViewPtr->GetSelectedItems();
	if ( Items.Num() > 0 )
	{
		return Items[0]->FolderPath;
	}

	return FString();
}

TArray<FString> ST1PathView::GetSelectedPaths() const
{
	TArray<FString> RetArray;

	TArray<TSharedPtr<FT1TreeItem>> Items = TreeViewPtr->GetSelectedItems();
	for ( int32 ItemIdx = 0; ItemIdx < Items.Num(); ++ItemIdx )
	{
		RetArray.Add(Items[ItemIdx]->FolderPath);
	}
	return RetArray;
}

TSharedPtr<FT1TreeItem> ST1PathView::AddPath(const FString& Path, bool bUserNamed)
{
	if ( !ensure(TreeViewPtr.IsValid()) )
	{
		// No tree view for some reason
		return TSharedPtr<FT1TreeItem>();
	}

	TArray<FString> PathItemList;
	Path.ParseIntoArray(PathItemList, TEXT("/"), /*InCullEmpty=*/true);

	if ( PathItemList.Num() )
	{
		// There is at least one element in the path
		TSharedPtr<FT1TreeItem> CurrentItem;

		// Find the first item in the root items list
		for ( int32 RootItemIdx = 0; RootItemIdx < TreeRootItems.Num(); ++RootItemIdx )
		{
			if ( TreeRootItems[RootItemIdx]->FolderName == PathItemList[0] )
			{
				// Found the first item in the path
				CurrentItem = TreeRootItems[RootItemIdx];
				break;
			}
		}

		// Roots may or may not exist, add the root here if it doesn't
		if ( !CurrentItem.IsValid() )
		{
			CurrentItem = AddRootItem(PathItemList[0]);
		}

		// Found or added the root item?
		if ( CurrentItem.IsValid() )
		{
			TSharedRef<FT1EmptyFolderVisibilityManager> EmptyFolderVisibilityManager = FT1ContentBrowserSingleton::Get().GetEmptyFolderVisibilityManager();

			// Now add children as necessary
			const bool bDisplayEmpty = GetDefault<UContentBrowserSettings>()->DisplayEmptyFolders;
			const bool bDisplayDev = GetDefault<UContentBrowserSettings>()->GetDisplayDevelopersFolder();
			const bool bDisplayL10N = GetDefault<UContentBrowserSettings>()->GetDisplayL10NFolder();
			for ( int32 PathItemIdx = 1; PathItemIdx < PathItemList.Num(); ++PathItemIdx )
			{
				const FString& PathItemName = PathItemList[PathItemIdx];
				TSharedPtr<FT1TreeItem> ChildItem = CurrentItem->GetChild(PathItemName);

				// If it does not exist, Create the child item
				if ( !ChildItem.IsValid() )
				{
					const FString FolderName = PathItemName;
					const FString FolderPath = CurrentItem->FolderPath + "/" + PathItemName;

					if (!bUserNamed)
					{
						// If this folder shouldn't be shown, break out here
						if ( !bDisplayEmpty && !EmptyFolderVisibilityManager->ShouldShowPath(FolderPath) )
						{
							break;
						}

						// If this is a developer folder, and we don't want to show them break out here
						if ( !bDisplayDev && T1ContentBrowserUtils::IsDevelopersFolder(FolderPath) )
						{
							break;
						}

						// If this is a localized folder, and we don't want to show them break out here
						if ( !bDisplayL10N && T1ContentBrowserUtils::IsLocalizationFolder(FolderPath) )
						{
							break;
						}
					}

					ChildItem = MakeShareable( new FT1TreeItem(FText::FromString(FolderName), FolderName, FolderPath, CurrentItem, bUserNamed) );
					CurrentItem->Children.Add(ChildItem);
					CurrentItem->RequestSortChildren();
					TreeViewPtr->RequestTreeRefresh();

					// If we have pending initial paths, and this path added the path, we should select it now
					if ( PendingInitialPaths.Num() > 0 && PendingInitialPaths.Contains(FolderPath) )
					{
						RecursiveExpandParents(ChildItem);
						TreeViewPtr->SetItemSelection(ChildItem, true);
						TreeViewPtr->RequestScrollIntoView(ChildItem);
					}
				}
				else
				{
					//If the child item does exist, ensure its folder path is correct (may differ when renaming parent folder)
					ChildItem->FolderPath = CurrentItem->FolderPath + "/" + PathItemName;
				}

				CurrentItem = ChildItem;
			}

			if ( bUserNamed && CurrentItem->Parent.IsValid() )
			{
				// If we were creating a new item, select it, scroll it into view, expand the parent
				RecursiveExpandParents(CurrentItem);
				TreeViewPtr->RequestScrollIntoView(CurrentItem);
				TreeViewPtr->SetSelection(CurrentItem);
			}
			else
			{
				CurrentItem->bNamingFolder = false;
			}
		}

		return CurrentItem;
	}

	return TSharedPtr<FT1TreeItem>();
}

bool ST1PathView::RemovePath(const FString& Path)
{
	if ( !ensure(TreeViewPtr.IsValid()) )
	{
		// No tree view for some reason
		return false;
	}

	if ( Path.IsEmpty() )
	{
		// There were no elements in the path, cannot remove nothing
		return false;
	}

	// Find the folder in the tree
	TSharedPtr<FT1TreeItem> ItemToRemove = FindItemRecursive(Path);

	if ( ItemToRemove.IsValid() )
	{
		// Found the folder to remove. Remove it.
		if ( ItemToRemove->Parent.IsValid() )
		{
			// Remove the folder from its parent's list
			ItemToRemove->Parent.Pin()->Children.Remove(ItemToRemove);
		}
		else
		{
			// This is a root item. Remove the folder from the root items list.
			TreeRootItems.Remove(ItemToRemove);
		}

		// Refresh the tree
		TreeViewPtr->RequestTreeRefresh();

		return true;
	}
	else
	{
		// Did not find the folder to remove
		return false;
	}
}

void ST1PathView::RenameFolder(const FString& FolderToRename)
{
	TArray<TSharedPtr<FT1TreeItem>> Items = TreeViewPtr->GetSelectedItems();
	for (int32 ItemIdx = 0; ItemIdx < Items.Num(); ++ItemIdx)
	{
		TSharedPtr<FT1TreeItem>& Item = Items[ItemIdx];
		if (Item.IsValid())
		{
			if (Item->FolderPath == FolderToRename)
			{
				Item->bNamingFolder = true;
				TreeViewPtr->SetSelection(Item);
				TreeViewPtr->RequestScrollIntoView(Item);
				break;
			}
		}
	}
}

void ST1PathView::SyncToAssets( const TArray<FAssetData>& AssetDataList, const bool bAllowImplicitSync )
{
	SyncToInternal(AssetDataList, TArray<FString>(), bAllowImplicitSync);
}

void ST1PathView::SyncToFolders( const TArray<FString>& FolderList, const bool bAllowImplicitSync )
{
	SyncToInternal(TArray<FAssetData>(), FolderList, bAllowImplicitSync);
}

void ST1PathView::SyncTo( const FT1ContentBrowserSelection& ItemSelection, const bool bAllowImplicitSync )
{
	SyncToInternal(ItemSelection.SelectedAssets, ItemSelection.SelectedFolders, bAllowImplicitSync);
}

void ST1PathView::SyncToInternal( const TArray<FAssetData>& AssetDataList, const TArray<FString>& FolderPaths, const bool bAllowImplicitSync )
{
	TArray<TSharedPtr<FT1TreeItem>> SyncTreeItems;

	// Clear the filter
	SearchBoxPtr->SetText(FText::GetEmpty());

	TSet<FString> PackagePaths = TSet<FString>(FolderPaths);
	for (const FAssetData& AssetData : AssetDataList)
	{
		FString PackagePath;
		if (AssetData.AssetClass == NAME_Class)
		{
			// Classes are found in the /Classes_ roots
			TSharedRef<FT1NativeClassHierarchy> NativeClassHierarchy = FT1ContentBrowserSingleton::Get().GetNativeClassHierarchy();
			NativeClassHierarchy->GetClassPath(Cast<UClass>(AssetData.GetAsset()), PackagePath, false/*bIncludeClassName*/);
		}
		else
		{
			// All other assets are found by their package path
			PackagePath = AssetData.PackagePath.ToString();
		}

		PackagePaths.Add(PackagePath);
	}

	for (const FString& PackagePath : PackagePaths)
	{
		if ( !PackagePath.IsEmpty() )
		{
			TSharedPtr<FT1TreeItem> Item = FindItemRecursive(PackagePath);
			if ( Item.IsValid() )
			{
				SyncTreeItems.Add(Item);
			}
		}
	}

	if ( SyncTreeItems.Num() > 0 )
	{
		if (bAllowImplicitSync)
		{
			// Prune the current selection so that we don't unnecessarily change the path which might disorientate the user.
			// If a parent tree item is currently selected we don't need to clear it and select the child
			auto SelectedTreeItems = TreeViewPtr->GetSelectedItems();

			for (int32 Index = 0; Index < SelectedTreeItems.Num(); ++Index)
			{
				// For each item already selected in the tree
				auto AlreadySelectedTreeItem = SelectedTreeItems[Index];
				if (!AlreadySelectedTreeItem.IsValid())
				{
					continue;
				}

				// Check to see if any of the items to sync are already synced
				for (int32 ToSyncIndex = SyncTreeItems.Num()-1; ToSyncIndex >= 0; --ToSyncIndex)
				{
					auto ToSyncItem = SyncTreeItems[ToSyncIndex];
					if (ToSyncItem == AlreadySelectedTreeItem || ToSyncItem->IsChildOf(*AlreadySelectedTreeItem.Get()))
					{
						// A parent is already selected
						SyncTreeItems.Pop();
					}
					else if (ToSyncIndex == 0)
					{
						// AlreadySelectedTreeItem is not required for SyncTreeItems, so deselect it
						TreeViewPtr->SetItemSelection(AlreadySelectedTreeItem, false);
					}
				}
			}
		}
		else
		{
			// Explicit sync so just clear the selection
			TreeViewPtr->ClearSelection();
		}

		// SyncTreeItems should now only contain items which aren't already shown explicitly or implicitly (as a child)
		for ( auto ItemIt = SyncTreeItems.CreateConstIterator(); ItemIt; ++ItemIt )
		{
			RecursiveExpandParents(*ItemIt);
			TreeViewPtr->SetItemSelection(*ItemIt, true);
		}

		// > 0 as some may have been popped off in the code above
		if (SyncTreeItems.Num() > 0)
		{
			// Scroll the first item into view if applicable
			TreeViewPtr->RequestScrollIntoView(SyncTreeItems[0]);
		}
	}
}

TSharedPtr<FT1TreeItem> ST1PathView::FindItemRecursive(const FString& Path) const
{
	for (auto TreeItemIt = TreeRootItems.CreateConstIterator(); TreeItemIt; ++TreeItemIt)
	{
		if ( (*TreeItemIt)->FolderPath == Path)
		{
			// This root item is the path
			return *TreeItemIt;
		}

		// Try to find the item under this root
		TSharedPtr<FT1TreeItem> Item = (*TreeItemIt)->FindItemRecursive(Path);
		if ( Item.IsValid() )
		{
			// The item was found under this root
			return Item;
		}
	}

	return TSharedPtr<FT1TreeItem>();
}

void ST1PathView::ApplyHistoryData ( const FT1HistoryData& History )
{
	// Prevent the selection changed delegate because it would add more history when we are just setting a state
	FScopedPreventTreeItemChangedDelegate DelegatePrevention( SharedThis(this) );

	// Update paths
	TArray<FString> SelectedPaths;
	for (const FName& HistoryPath : History.T1SourcesData.PackagePaths)
	{
		SelectedPaths.Add(HistoryPath.ToString());
	}
	SetSelectedPaths(SelectedPaths);
}

void ST1PathView::SaveSettings(const FString& IniFilename, const FString& IniSection, const FString& SettingsString) const
{
	FString SelectedPathsString;
	TArray< TSharedPtr<FT1TreeItem> > PathItems = TreeViewPtr->GetSelectedItems();
	for ( auto PathIt = PathItems.CreateConstIterator(); PathIt; ++PathIt )
	{
		if ( SelectedPathsString.Len() > 0 )
		{
			SelectedPathsString += TEXT(",");
		}

		SelectedPathsString += (*PathIt)->FolderPath;
	}

	GConfig->SetString(*IniSection, *(SettingsString + TEXT(".SelectedPaths")), *SelectedPathsString, IniFilename);
}

void ST1PathView::LoadSettings(const FString& IniFilename, const FString& IniSection, const FString& SettingsString)
{
	// Selected Paths
	FString SelectedPathsString;
	TArray<FString> NewSelectedPaths;
	if ( GConfig->GetString(*IniSection, *(SettingsString + TEXT(".SelectedPaths")), SelectedPathsString, IniFilename) )
	{
		SelectedPathsString.ParseIntoArray(NewSelectedPaths, TEXT(","), /*bCullEmpty*/true);
	}

	if ( NewSelectedPaths.Num() > 0 )
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		const bool bDiscoveringAssets = AssetRegistryModule.Get().IsLoadingAssets();

		if ( bDiscoveringAssets )
		{
			// Keep track if we changed at least one source so we know to fire the bulk selection changed delegate later
			bool bSelectedAtLeastOnePath = false;

			{
				// Prevent the selection changed delegate since we are selecting one path at a time. A bulk event will be fired later if needed.
				FScopedPreventTreeItemChangedDelegate DelegatePrevention( SharedThis(this) );				

				// Clear any previously selected paths
				TreeViewPtr->ClearSelection();

				// If the selected paths is empty, the path was "All assets"
				// This should handle that case properly
				for (int32 PathIdx = 0; PathIdx < NewSelectedPaths.Num(); ++PathIdx)
				{
					const FString& Path = NewSelectedPaths[PathIdx];
					if ( ExplicitlyAddPathToSelection(Path) )
					{
						bSelectedAtLeastOnePath = true;
					}
					else
					{
						// If we could not initially select these paths, but are still discovering assets, add them to a pending list to select them later
						PendingInitialPaths.Add(Path);
					}
				}
			}

			if ( bSelectedAtLeastOnePath )
			{
				// Send the first selected item with the notification
				const TArray<TSharedPtr<FT1TreeItem>> SelectedItems = TreeViewPtr->GetSelectedItems();
				check(SelectedItems.Num() > 0);

				// Signal a single selection changed event to let any listeners know that paths have changed
				TreeSelectionChanged( SelectedItems[0], ESelectInfo::Direct );
			}
		}
		else
		{
			// If all assets are already discovered, just select paths the best we can
			SetSelectedPaths(NewSelectedPaths);

			// Send the first selected item with the notification
			const TArray<TSharedPtr<FT1TreeItem>> SelectedItems = TreeViewPtr->GetSelectedItems();
			if (SelectedItems.Num() > 0)
			{
				// Signal a single selection changed event to let any listeners know that paths have changed
				TreeSelectionChanged( SelectedItems[0], ESelectInfo::Direct );
			}
		}
	}
}

EActiveTimerReturnType ST1PathView::SetFocusPostConstruct( double InCurrentTime, float InDeltaTime )
{
	FWidgetPath WidgetToFocusPath;
	FSlateApplication::Get().GeneratePathToWidgetUnchecked( SearchBoxPtr.ToSharedRef(), WidgetToFocusPath );
	FSlateApplication::Get().SetKeyboardFocus( WidgetToFocusPath, EFocusCause::SetDirectly );

	return EActiveTimerReturnType::Stop;
}

EActiveTimerReturnType ST1PathView::TriggerRepopulate(double InCurrentTime, float InDeltaTime)
{
	Populate();
	return EActiveTimerReturnType::Stop;
}

TSharedPtr<SWidget> ST1PathView::MakePathViewContextMenu()
{
	if ( TreeViewPtr->GetSelectedItems().Num() <= 0 || !bAllowContextMenu )
	{
		return NULL;
	}

	if(!OnGetFolderContextMenu.IsBound())
	{
		return NULL;
	}

	const TArray<FString> SelectedPaths = GetSelectedPaths();
	return OnGetFolderContextMenu.Execute(SelectedPaths, OnGetPathContextMenuExtender, FT1OnCreateNewFolder::CreateSP(this, &ST1PathView::OnCreateNewFolder));
}

void ST1PathView::OnCreateNewFolder(const FString& FolderName, const FString& FolderPath)
{
	AddPath(FolderPath / FolderName, /*bUserNamed=*/true);
}

bool ST1PathView::ExplicitlyAddPathToSelection(const FString& Path)
{
	if ( !ensure(TreeViewPtr.IsValid()) )
	{
		return false;
	}

	TArray<FString> PathItemList;
	Path.ParseIntoArray(PathItemList, TEXT("/"), /*InCullEmpty=*/true);

	if ( PathItemList.Num() )
	{
		// There is at least one element in the path
		TSharedPtr<FT1TreeItem> RootItem;

		// Find the first item in the root items list
		for ( int32 RootItemIdx = 0; RootItemIdx < TreeRootItems.Num(); ++RootItemIdx )
		{
			if ( TreeRootItems[RootItemIdx]->FolderName == PathItemList[0] )
			{
				// Found the first item in the path
				RootItem = TreeRootItems[RootItemIdx];
				break;
			}
		}

		// If found in the root items list, try to find the item matching the path
		if ( RootItem.IsValid() )
		{
			TSharedPtr<FT1TreeItem> FoundItem = RootItem->FindItemRecursive(Path);

			if ( FoundItem.IsValid() )
			{
				// Set the selection to the closest found folder and scroll it into view
				RecursiveExpandParents(FoundItem);
				TreeViewPtr->SetItemSelection(FoundItem, true);
				TreeViewPtr->RequestScrollIntoView(FoundItem);

				return true;
			}
		}
	}

	return false;
}

bool ST1PathView::ShouldAllowTreeItemChangedDelegate() const
{
	return PreventTreeItemChangedDelegateCount == 0;
}

void ST1PathView::RecursiveExpandParents(const TSharedPtr<FT1TreeItem>& Item)
{
	if ( Item->Parent.IsValid() )
	{
		RecursiveExpandParents(Item->Parent.Pin());
		TreeViewPtr->SetItemExpansion(Item->Parent.Pin(), true);
	}
}

TSharedPtr<struct FT1TreeItem> ST1PathView::AddRootItem( const FString& InFolderName )
{
	// Make sure the item is not already in the list
	for ( int32 RootItemIdx = 0; RootItemIdx < TreeRootItems.Num(); ++RootItemIdx )
	{
		if ( TreeRootItems[RootItemIdx]->FolderName == InFolderName )
		{
			// The root to add was already in the list return it here
			return TreeRootItems[RootItemIdx];
		}
	}

	TSharedPtr<struct FT1TreeItem> NewItem = nullptr;

	// If this isn't an engine folder or we want to show them, add
	const bool bDisplayEngine = GetDefault<UContentBrowserSettings>()->GetDisplayEngineFolder();
	const bool bDisplayPlugins = GetDefault<UContentBrowserSettings>()->GetDisplayPluginFolders();
	const bool bDisplayCpp = GetDefault<UContentBrowserSettings>()->GetDisplayCppFolders();

	// Filter out classes folders if we're not showing them.
	if ( !bDisplayCpp && T1ContentBrowserUtils::IsClassesFolder(InFolderName) )
	{
		return nullptr;
	}

	// Filter out plugin folders
	bool bIsPlugin = false;
	EPluginLoadedFrom PluginSource = EPluginLoadedFrom::Engine; // init to avoid warning
	if (!bDisplayEngine || !bDisplayPlugins)
	{
		bIsPlugin = T1ContentBrowserUtils::IsPluginFolder(InFolderName, &PluginSource);
	}

	if ((bDisplayEngine || !T1ContentBrowserUtils::IsEngineFolder(InFolderName)) &&
 		 ((bDisplayEngine && bDisplayPlugins) || !(bIsPlugin && PluginSource == EPluginLoadedFrom::Engine)) &&
		 (bDisplayPlugins || !(bIsPlugin && PluginSource == EPluginLoadedFrom::Project)))
		{
			const FText DisplayName = T1ContentBrowserUtils::GetRootDirDisplayName(InFolderName);
			NewItem = MakeShareable( new FT1TreeItem(DisplayName, InFolderName, FString(TEXT("/")) + InFolderName, TSharedPtr<FT1TreeItem>()));
			TreeRootItems.Add( NewItem );
			TreeViewPtr->RequestTreeRefresh();
		}

	return NewItem;
}

TSharedRef<ITableRow> ST1PathView::GenerateTreeRow( TSharedPtr<FT1TreeItem> TreeItem, const TSharedRef<STableViewBase>& OwnerTable )
{
	check(TreeItem.IsValid());

	return
		SNew( STableRow< TSharedPtr<FT1TreeItem> >, OwnerTable )
		.OnDragDetected( this, &ST1PathView::OnFolderDragDetected )
		[
			SNew(T1SAssetTreeItem)
			.TreeItem(TreeItem)
			.OnNameChanged(this, &ST1PathView::FolderNameChanged)
			.OnVerifyNameChanged(this, &ST1PathView::VerifyFolderNameChanged)
			.OnAssetsOrPathsDragDropped(this, &ST1PathView::TreeAssetsOrPathsDropped)
			.OnFilesDragDropped(this, &ST1PathView::TreeFilesDropped)
			.IsItemExpanded(this, &ST1PathView::IsTreeItemExpanded, TreeItem)
			.HighlightText(this, &ST1PathView::GetHighlightText)
			.IsSelected(this, &ST1PathView::IsTreeItemSelected, TreeItem)
		];
}

void ST1PathView::TreeItemScrolledIntoView( TSharedPtr<FT1TreeItem> TreeItem, const TSharedPtr<ITableRow>& Widget )
{
	if ( TreeItem->bNamingFolder && Widget.IsValid() && Widget->GetContent().IsValid() )
	{
		TreeItem->OnRenamedRequestEvent.Broadcast();
	}
}

void ST1PathView::GetChildrenForTree( TSharedPtr< FT1TreeItem > TreeItem, TArray< TSharedPtr<FT1TreeItem> >& OutChildren )
{
	TreeItem->SortChildrenIfNeeded();
	OutChildren = TreeItem->Children;
}

void ST1PathView::SetTreeItemExpansionRecursive( TSharedPtr< FT1TreeItem > TreeItem, bool bInExpansionState )
{
	TreeViewPtr->SetItemExpansion(TreeItem, bInExpansionState);

	// Recursively go through the children.
	for(auto It = TreeItem->Children.CreateIterator(); It; ++It)
	{
		SetTreeItemExpansionRecursive( *It, bInExpansionState );
	}
}

void ST1PathView::TreeSelectionChanged( TSharedPtr< FT1TreeItem > TreeItem, ESelectInfo::Type /*SelectInfo*/ )
{
	if ( ShouldAllowTreeItemChangedDelegate() )
	{
		const TArray<TSharedPtr<FT1TreeItem>> SelectedItems = TreeViewPtr->GetSelectedItems();

		LastSelectedPaths.Empty();
		for (int32 ItemIdx = 0; ItemIdx < SelectedItems.Num(); ++ItemIdx)
		{
			const TSharedPtr<FT1TreeItem> Item = SelectedItems[ItemIdx];
			if ( !ensure(Item.IsValid()) )
			{
				// All items must exist
				continue;
			}

			// Keep track of the last paths that we broadcasted for selection reasons when filtering
			LastSelectedPaths.Add(Item->FolderPath);
		}

		if ( OnPathSelected.IsBound() )
		{
			if ( TreeItem.IsValid() )
			{
				OnPathSelected.Execute(TreeItem->FolderPath);
			}
			else
			{
				OnPathSelected.Execute(TEXT(""));
			}
		}
	}

	if (TreeItem.IsValid())
	{
		// Prioritize the asset registry scan for the selected path
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		AssetRegistryModule.Get().PrioritizeSearchPath(TreeItem->FolderPath / TEXT(""));
	}
}

void ST1PathView::TreeExpansionChanged( TSharedPtr< FT1TreeItem > TreeItem, bool bIsExpanded )
{
	if ( ShouldAllowTreeItemChangedDelegate() )
	{
		TSet<TSharedPtr<FT1TreeItem>> ExpandedItemSet;
		TreeViewPtr->GetExpandedItems(ExpandedItemSet);
		const TArray<TSharedPtr<FT1TreeItem>> ExpandedItems = ExpandedItemSet.Array();

		LastExpandedPaths.Empty();
		for (int32 ItemIdx = 0; ItemIdx < ExpandedItems.Num(); ++ItemIdx)
		{
			const TSharedPtr<FT1TreeItem> Item = ExpandedItems[ItemIdx];
			if ( !ensure(Item.IsValid()) )
			{
				// All items must exist
				continue;
			}

			// Keep track of the last paths that we broadcasted for expansion reasons when filtering
			LastExpandedPaths.Add(Item->FolderPath);
		}
	}
}

void ST1PathView::OnAssetTreeSearchBoxChanged( const FText& InSearchText )
{
	SearchBoxFolderFilter->SetRawFilterText( InSearchText );
	SearchBoxPtr->SetError( SearchBoxFolderFilter->GetFilterErrorText() );
}

void ST1PathView::OnAssetTreeSearchBoxCommitted(const FText& InSearchText, ETextCommit::Type InCommitType)
{
	if (InCommitType == ETextCommit::OnCleared)
	{
		// Clear the search box and the filters
		SearchBoxPtr->SetText(FText::GetEmpty());
		OnAssetTreeSearchBoxChanged(FText::GetEmpty());
		FSlateApplication::Get().ClearKeyboardFocus(EFocusCause::Cleared);
	}
}

void ST1PathView::FilterUpdated()
{
	Populate();
}

FText ST1PathView::GetHighlightText() const
{
	return SearchBoxFolderFilter->GetRawFilterText();
}

void ST1PathView::Populate()
{
	// Don't allow the selection changed delegate to be fired here
	FScopedPreventTreeItemChangedDelegate DelegatePrevention( SharedThis(this) );

	// Clear all root items and clear selection
	TreeRootItems.Empty();
	TreeViewPtr->ClearSelection();

	// Load the native class hierarchy to listen for updates
	TSharedRef<FT1NativeClassHierarchy> NativeClassHierarchy = FT1ContentBrowserSingleton::Get().GetNativeClassHierarchy();

	const bool bFilteringByText = !SearchBoxFolderFilter->GetRawFilterText().IsEmpty();
	
	const bool bDisplayEngine = GetDefault<UContentBrowserSettings>()->GetDisplayEngineFolder();
	const bool bDisplayPlugins = GetDefault<UContentBrowserSettings>()->GetDisplayPluginFolders();

	TArray<FName> ClassRoots;
	TArray<FString> ClassFolders;
	if ( bAllowClassesFolder )
	{
		NativeClassHierarchy->GetClassFolders(ClassRoots, ClassFolders, bDisplayEngine, bDisplayPlugins);
	}

	if ( !bFilteringByText )
	{
		// If we aren't filtering, add default folders to the asset tree

		for(const FName& ClassRoot : ClassRoots)
		{
			AddRootItem(ClassRoot.ToString());
		}

		// Add all of the content paths we know about.  Note that this can change on the fly (if say, a plugin
		// with content becomes loaded), so this ST1PathView would need to be refreshed if that happened.
		TArray<FString> RootContentPaths;
		FPackageName::QueryRootContentPaths( RootContentPaths );
		for( TArray<FString>::TConstIterator RootPathIt( RootContentPaths ); RootPathIt; ++RootPathIt )
		{
			// Strip off any leading or trailing forward slashes.  We just want a root path name that
			// we can display, and we'll add the path separators back later on
			FString CleanRootPathName = *RootPathIt;
			while( CleanRootPathName.StartsWith( TEXT( "/" ) ) )
			{
				CleanRootPathName = CleanRootPathName.Mid( 1 );
			}
			while( CleanRootPathName.EndsWith( TEXT( "/" ) ) )
			{
				CleanRootPathName = CleanRootPathName.Mid( 0, CleanRootPathName.Len() - 1 );
			}

			// Templates can mount "root" items which are actually sub-items under a root (see FUnrealEdMisc::MountTemplateSharedPaths)
			// We can use AddPath here (rather than AddRootItem), as this will ensure that both the root and any sub-path items are added correctly
			AddPath(CleanRootPathName);
		}
	}
	
	// Load the asset registry module to listen for updates
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	// Add all paths currently gathered from the asset registry
	TArray<FString> PathList;
	AssetRegistryModule.Get().GetAllCachedPaths(PathList);

	// Add any class paths we discovered
	PathList.Append(ClassFolders);

	// Add the user developer folder
	const FString UserDeveloperFolder = FPackageName::FilenameToLongPackageName(FPaths::GameUserDeveloperDir().LeftChop(1));
	PathList.Add(UserDeveloperFolder);

	// Remove paths of localized assets, if not displaying localized assets.
	if (!(GetDefault<UContentBrowserSettings>()->GetDisplayL10NFolder()))
	{
		PathList.RemoveAll([](const FString& Path) -> bool
		{
			return T1ContentBrowserUtils::IsLocalizationFolder(Path);
		});
	}

	// we have a text filter, expand all parents of matching folders
	for ( int32 PathIdx = 0; PathIdx < PathList.Num(); ++PathIdx)
	{
		const FString& Path = PathList[PathIdx];

		// by sending the whole path we deliberately include any children
		// of successful hits in the filtered list. 
		if ( SearchBoxFolderFilter->PassesFilter( Path ) )
		{
			TSharedPtr<FT1TreeItem> Item = AddPath(Path);
			if ( Item.IsValid() )
			{
				const bool bSelectedItem = LastSelectedPaths.Contains(Item->FolderPath);
				const bool bExpandedItem = LastExpandedPaths.Contains(Item->FolderPath);

				if ( bFilteringByText || bSelectedItem )
				{
					RecursiveExpandParents(Item);
				}

				if ( bSelectedItem )
				{
					// Tree items that match the last broadcasted paths should be re-selected them after they are added
					if ( !TreeViewPtr->IsItemSelected(Item) )
					{
						TreeViewPtr->SetItemSelection(Item, true);
					}
					TreeViewPtr->RequestScrollIntoView(Item);
				}

				if ( bExpandedItem )
				{
					// Tree items that were previously expanded should be re-expanded when repopulating
					if ( !TreeViewPtr->IsItemExpanded(Item) )
					{
						TreeViewPtr->SetItemExpansion(Item, true);
					}
				}
			}
		}
	}

	SortRootItems();
}

void ST1PathView::SortRootItems()
{
	// First sort the root items by their display name, but also making sure that content to appears before classes
	TreeRootItems.Sort([](const TSharedPtr<FT1TreeItem>& One, const TSharedPtr<FT1TreeItem>& Two) -> bool
	{
		static const FString ClassesPrefix = TEXT("Classes_");

		FString OneModuleName = One->FolderName;
		const bool bOneIsClass = OneModuleName.StartsWith(ClassesPrefix);
		if(bOneIsClass)
		{
			OneModuleName = OneModuleName.Mid(ClassesPrefix.Len());
		}

		FString TwoModuleName = Two->FolderName;
		const bool bTwoIsClass = TwoModuleName.StartsWith(ClassesPrefix);
		if(bTwoIsClass)
		{
			TwoModuleName = TwoModuleName.Mid(ClassesPrefix.Len());
		}

		// We want to sort content before classes if both items belong to the same module
		if(OneModuleName == TwoModuleName)
		{
			if(!bOneIsClass && bTwoIsClass)
			{
				return true;
			}
			return false;
		}

		return One->DisplayName.ToString() < Two->DisplayName.ToString();
	});

	// We have some manual sorting requirements that game must come before engine, and engine before everything else - we do that here after sorting everything by name
	// The array below is in the inverse order as we iterate through and move each match to the beginning of the root items array
	static const FString InverseSortOrder[] = {
		TEXT("Classes_Engine"),
		TEXT("Engine"),
		TEXT("Classes_Game"),
		TEXT("Game"),
	};
	for(const FString& SortItem : InverseSortOrder)
	{
		const int32 FoundItemIndex = TreeRootItems.IndexOfByPredicate([&SortItem](const TSharedPtr<FT1TreeItem>& TreeItem) -> bool
		{
			return TreeItem->FolderName == SortItem;
		});
		if(FoundItemIndex != INDEX_NONE)
		{
			TSharedPtr<FT1TreeItem> ItemToMove = TreeRootItems[FoundItemIndex];
			TreeRootItems.RemoveAt(FoundItemIndex);
			TreeRootItems.Insert(ItemToMove, 0);
		}
	}

	TreeViewPtr->RequestTreeRefresh();
}

void ST1PathView::PopulateFolderSearchStrings( const FString& FolderName, OUT TArray< FString >& OutSearchStrings ) const
{
	OutSearchStrings.Add( FolderName );
}

FReply ST1PathView::OnFolderDragDetected(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if ( MouseEvent.IsMouseButtonDown( EKeys::LeftMouseButton ) )
	{
		TArray<TSharedPtr<FT1TreeItem>> SelectedItems = TreeViewPtr->GetSelectedItems();
		if (SelectedItems.Num())
		{
			TArray<FString> PathNames;

			for ( auto ItemIt = SelectedItems.CreateConstIterator(); ItemIt; ++ItemIt )
			{
				PathNames.Add((*ItemIt)->FolderPath);
			}

			return FReply::Handled().BeginDragDrop(FAssetDragDropOp::New(PathNames));
		}
	}

	return FReply::Unhandled();
}

bool ST1PathView::VerifyFolderNameChanged(const FString& InName, FText& OutErrorMessage, const FString& InFolderPath) const
{	
	return T1ContentBrowserUtils::IsValidFolderPathForCreate(FPaths::GetPath(InFolderPath), InName, OutErrorMessage);
}

void ST1PathView::FolderNameChanged( const TSharedPtr< FT1TreeItem >& TreeItem, const FString& OldPath, const FVector2D& MessageLocation, const ETextCommit::Type CommitType )
{
	TArray<FT1MovedContentFolder> MovedFolders;
	// Verify the name of the folder
	FText Reason;
	if ( T1ContentBrowserUtils::IsValidFolderName(TreeItem->FolderName, Reason) )
	{
		if (CommitType == ETextCommit::OnCleared)
		{
			FString NewPathOnDisk;
			if (FPackageName::TryConvertLongPackageNameToFilename(TreeItem->FolderPath, NewPathOnDisk) && !IFileManager::Get().DirectoryExists(*NewPathOnDisk))
			{
				// Clearing the rename box on a newly created folder cancels the entire creation process
				RemoveFolderItem(TreeItem);
				return;
			}
		}

		TSharedPtr< FT1TreeItem > ExistingItem;
		if ( FolderAlreadyExists(TreeItem, ExistingItem) )
		{
			// The folder already exists, remove it so selection is simple
			RemoveFolderItem(ExistingItem);
		}
		
		// The folder did not already exist
		bool bWasItemSelected = TreeViewPtr->IsItemSelected(TreeItem);

		// Reselect the path to notify that selection has changed
		if ( bWasItemSelected )
		{
			FScopedPreventTreeItemChangedDelegate DelegatePrevention( SharedThis(this) );
			TreeViewPtr->SetItemSelection(TreeItem, false);
		}

		// If we weren't a root node, make sure our parent is sorted
		if ( TreeItem->Parent.IsValid() )
		{
			TreeItem->Parent.Pin()->RequestSortChildren();
			TreeViewPtr->RequestTreeRefresh();
		}

		if ( bWasItemSelected )
		{
			// Set the selection again
			TreeViewPtr->SetItemSelection(TreeItem, true);

			// Scroll back into view if position has changed
			TreeViewPtr->RequestScrollIntoView(TreeItem);
		}

		// Update either the asset registry or the native class hierarchy so this folder will persist
		/*
		if (T1ContentBrowserUtils::IsClassPath(TreeItem->FolderPath))
		{
			// todo: jdale - CLASS - This will need updating to support renaming of class folders (SAssetView has similar logic - needs abstracting)
			TSharedRef<FT1NativeClassHierarchy> NativeClassHierarchy = FT1ContentBrowserSingleton::Get().GetNativeClassHierarchy();
			NativeClassHierarchy->AddFolder(TreeItem->FolderPath);
		}
		else
		*/
		{
			// ensure the folder exists on disk
			FString NewPathOnDisk;
			if (FPackageName::TryConvertLongPackageNameToFilename(TreeItem->FolderPath, NewPathOnDisk) && IFileManager::Get().MakeDirectory(*NewPathOnDisk, true))
			{
				TSharedRef<FT1EmptyFolderVisibilityManager> EmptyFolderVisibilityManager = FT1ContentBrowserSingleton::Get().GetEmptyFolderVisibilityManager();
				EmptyFolderVisibilityManager->SetAlwaysShowPath(TreeItem->FolderPath);

				FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
				if (AssetRegistryModule.Get().AddPath(TreeItem->FolderPath) && TreeItem->FolderPath != OldPath)
				{
					MovedFolders.Add(FT1MovedContentFolder(OldPath, TreeItem->FolderPath));
					// move any assets in our folder
					TArray<FAssetData> AssetsInFolder;
					AssetRegistryModule.Get().GetAssetsByPath(*OldPath, AssetsInFolder, true);
					TArray<UObject*> ObjectsInFolder;
					T1ContentBrowserUtils::GetObjectsInAssetData(AssetsInFolder, ObjectsInFolder);
					T1ContentBrowserUtils::MoveAssets(ObjectsInFolder, TreeItem->FolderPath, OldPath);

					// Now check to see if the original folder is empty, if so we can delete it
					TArray<FAssetData> AssetsInOriginalFolder;
					AssetRegistryModule.Get().GetAssetsByPath(*OldPath, AssetsInOriginalFolder, true);
					if (AssetsInOriginalFolder.Num() == 0)
					{
						TArray<FString> FoldersToDelete;
						FoldersToDelete.Add(OldPath);
						T1ContentBrowserUtils::DeleteFolders(FoldersToDelete);
					}
					OnFolderPathChanged.ExecuteIfBound(MovedFolders);
				}
			}
		}
	}
	else
	{
		// Remove the item
		RemoveFolderItem(TreeItem);

		// Display the reason why the folder was invalid
		FSlateRect MessageAnchor(MessageLocation.X, MessageLocation.Y, MessageLocation.X, MessageLocation.Y);
		T1ContentBrowserUtils::DisplayMessage(Reason, MessageAnchor, SharedThis(this));
	}
}



bool ST1PathView::FolderAlreadyExists(const TSharedPtr< FT1TreeItem >& TreeItem, TSharedPtr< FT1TreeItem >& ExistingItem)
{
	ExistingItem.Reset();

	if ( TreeItem.IsValid() )
	{
		if ( TreeItem->Parent.IsValid() )
		{
			// This item has a parent, try to find it in its parent's children
			TSharedPtr<FT1TreeItem> ParentItem = TreeItem->Parent.Pin();

			for ( auto ChildIt = ParentItem->Children.CreateConstIterator(); ChildIt; ++ChildIt )
			{
				const TSharedPtr<FT1TreeItem>& Child = *ChildIt;
				if ( Child != TreeItem && Child->FolderName == TreeItem->FolderName )
				{
					// The item is in its parent already
					ExistingItem = Child;
					break;
				}
			}
		}
		else
		{
			// This item is part of the root set
			for ( auto RootIt = TreeRootItems.CreateConstIterator(); RootIt; ++RootIt )
			{
				const TSharedPtr<FT1TreeItem>& Root = *RootIt;
				if ( Root != TreeItem && Root->FolderName == TreeItem->FolderName )
				{
					// The item is part of the root set already
					ExistingItem = Root;
					break;
				}
			}
		}
	}

	return ExistingItem.IsValid();
}

void ST1PathView::RemoveFolderItem(const TSharedPtr< FT1TreeItem >& TreeItem)
{
	if ( TreeItem.IsValid() )
	{
		if ( TreeItem->Parent.IsValid() )
		{
			// Remove this item from it's parent's list
			TreeItem->Parent.Pin()->Children.Remove(TreeItem);
		}
		else
		{
			// This was a root node, remove from the root list
			TreeRootItems.Remove(TreeItem);
		}

		TreeViewPtr->RequestTreeRefresh();
	}
}

void ST1PathView::TreeAssetsOrPathsDropped(const TArray<FAssetData>& AssetList, const TArray<FString>& AssetPaths, const TSharedPtr<FT1TreeItem>& TreeItem)
{
	T1DragDropHandler::HandleDropOnAssetFolder(
		SharedThis(this), 
		AssetList, 
		AssetPaths, 
		TreeItem->FolderPath, 
		TreeItem->DisplayName, 
		T1DragDropHandler::FExecuteCopyOrMove::CreateSP(this, &ST1PathView::ExecuteTreeDropCopy),
		T1DragDropHandler::FExecuteCopyOrMove::CreateSP(this, &ST1PathView::ExecuteTreeDropMove)
		);
}

void ST1PathView::TreeFilesDropped(const TArray<FString>& FileNames, const TSharedPtr<FT1TreeItem>& TreeItem)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
	AssetToolsModule.Get().ImportAssets( FileNames, TreeItem->FolderPath );
}

bool ST1PathView::IsTreeItemExpanded(TSharedPtr<FT1TreeItem> TreeItem) const
{
	return TreeViewPtr->IsItemExpanded(TreeItem);
}

bool ST1PathView::IsTreeItemSelected(TSharedPtr<FT1TreeItem> TreeItem) const
{
	return TreeViewPtr->IsItemSelected(TreeItem);
}

void ST1PathView::ExecuteTreeDropCopy(TArray<FAssetData> AssetList, TArray<FString> AssetPaths, FString DestinationPath)
{
	if (AssetList.Num() > 0)
	{
		TArray<UObject*> DroppedObjects;
		T1ContentBrowserUtils::GetObjectsInAssetData(AssetList, DroppedObjects);

		T1ContentBrowserUtils::CopyAssets(DroppedObjects, DestinationPath);
	}

	if (AssetPaths.Num() > 0 && T1ContentBrowserUtils::CopyFolders(AssetPaths, DestinationPath))
	{
		TSharedPtr<FT1TreeItem> RootItem = FindItemRecursive(DestinationPath);
		if (RootItem.IsValid())
		{
			TreeViewPtr->SetItemExpansion(RootItem, true);

			// Select all the new folders
			TreeViewPtr->ClearSelection();
			for (const FString& AssetPath : AssetPaths)
			{
				const FString SubFolderName = FPackageName::GetLongPackageAssetName(AssetPath);
				const FString NewPath = DestinationPath + TEXT("/") + SubFolderName;

				TSharedPtr<FT1TreeItem> Item = FindItemRecursive(NewPath);
				if (Item.IsValid())
				{
					TreeViewPtr->SetItemSelection(Item, true);
					TreeViewPtr->RequestScrollIntoView(Item);
				}
			}
		}
	}
}

void ST1PathView::ExecuteTreeDropMove(TArray<FAssetData> AssetList, TArray<FString> AssetPaths, FString DestinationPath)
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

	if (AssetPaths.Num() > 0 && T1ContentBrowserUtils::MoveFolders(AssetPaths, DestinationPath))
	{
		TSharedPtr<FT1TreeItem> RootItem = FindItemRecursive(DestinationPath);
		if (RootItem.IsValid())
		{
			TreeViewPtr->SetItemExpansion(RootItem, true);

			// Select all the new folders
			TreeViewPtr->ClearSelection();
			for (const FString& AssetPath : AssetPaths)
			{
				const FString SubFolderName = FPackageName::GetLongPackageAssetName(AssetPath);
				const FString NewPath = DestinationPath + TEXT("/") + SubFolderName;
				TSharedPtr<FT1TreeItem> Item = FindItemRecursive(NewPath);
				if (Item.IsValid())
				{
					TreeViewPtr->SetItemSelection(Item, true);
					TreeViewPtr->RequestScrollIntoView(Item);
				}
			}
		}

		OnFolderPathChanged.ExecuteIfBound(MovedFolders);
	}
}

void ST1PathView::OnAssetRegistryPathAdded(const FString& Path)
{
	// by sending the whole path we deliberately include any children
	// of successful hits in the filtered list. 
	if ( SearchBoxFolderFilter->PassesFilter( Path ) )
	{
		AddPath(Path);
	}
}

void ST1PathView::OnAssetRegistryPathRemoved(const FString& Path)
{
	// by sending the whole path we deliberately include any children
	// of successful hits in the filtered list. 
	if ( SearchBoxFolderFilter->PassesFilter( Path ) )
	{
		RemovePath(Path);
	}
}

void ST1PathView::OnAssetRegistrySearchCompleted()
{
	// If there were any more initial paths, they no longer exist so clear them now.
	PendingInitialPaths.Empty();
}

void ST1PathView::OnFolderPopulated(const FString& Path)
{
	OnAssetRegistryPathAdded(Path);
}

void ST1PathView::OnContentPathMountedOrDismounted( const FString& AssetPath, const FString& FilesystemPath )
{
	// A new content path has appeared, so we should refresh out root set of paths
	RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &ST1PathView::TriggerRepopulate));
}

void ST1PathView::OnClassHierarchyUpdated()
{
	// The class hierarchy has changed in some way, so we need to refresh our set of paths
	RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &ST1PathView::TriggerRepopulate));
}

void ST1PathView::HandleSettingChanged(FName PropertyName)
{
	if ((PropertyName == GET_MEMBER_NAME_CHECKED(UContentBrowserSettings, DisplayEmptyFolders)) ||
		(PropertyName == "DisplayDevelopersFolder") ||
		(PropertyName == "DisplayEngineFolder") ||
		(PropertyName == "DisplayPluginFolders") ||
		(PropertyName == "DisplayL10NFolder") ||
		(PropertyName == NAME_None))	// @todo: Needed if PostEditChange was called manually, for now
	{
		TSharedRef<FT1EmptyFolderVisibilityManager> EmptyFolderVisibilityManager = FT1ContentBrowserSingleton::Get().GetEmptyFolderVisibilityManager();

		// If the dev or engine folder is no longer visible but we're inside it...
		const bool bDisplayEmpty = GetDefault<UContentBrowserSettings>()->DisplayEmptyFolders;
		const bool bDisplayDev = GetDefault<UContentBrowserSettings>()->GetDisplayDevelopersFolder();
		const bool bDisplayEngine = GetDefault<UContentBrowserSettings>()->GetDisplayEngineFolder();
		const bool bDisplayPlugins = GetDefault<UContentBrowserSettings>()->GetDisplayPluginFolders();
		const bool bDisplayL10N = GetDefault<UContentBrowserSettings>()->GetDisplayL10NFolder();
		if (!bDisplayEmpty || !bDisplayDev || !bDisplayEngine || !bDisplayPlugins || !bDisplayL10N)
		{
			const FString OldSelectedPath = GetSelectedPath();
			const T1ContentBrowserUtils::ECBFolderCategory OldFolderCategory = T1ContentBrowserUtils::GetFolderCategory(OldSelectedPath);

			if ((!bDisplayEmpty && !EmptyFolderVisibilityManager->ShouldShowPath(OldSelectedPath)) || 
				(!bDisplayDev && OldFolderCategory == T1ContentBrowserUtils::ECBFolderCategory::DeveloperContent) || 
				(!bDisplayEngine && (OldFolderCategory == T1ContentBrowserUtils::ECBFolderCategory::EngineContent || OldFolderCategory == T1ContentBrowserUtils::ECBFolderCategory::EngineClasses)) || 
				(!bDisplayPlugins && (OldFolderCategory == T1ContentBrowserUtils::ECBFolderCategory::PluginContent || OldFolderCategory == T1ContentBrowserUtils::ECBFolderCategory::PluginClasses)) ||
				(!bDisplayL10N && T1ContentBrowserUtils::IsLocalizationFolder(OldSelectedPath)))
			{
				// Set the folder back to the root, and refresh the contents
				TSharedPtr<FT1TreeItem> GameRoot = FindItemRecursive(TEXT("/Game"));
				if ( GameRoot.IsValid() )
				{
					TreeViewPtr->SetSelection(GameRoot);
				}
				else
				{
					TreeViewPtr->ClearSelection();
				}
			}
		}

		// Update our path view so that it can include/exclude the dev folder
		Populate();

		// If the dev or engine folder has become visible and we're inside it...
		if (bDisplayDev || bDisplayEngine || bDisplayPlugins || bDisplayL10N)
		{
			const FString NewSelectedPath = GetSelectedPath();
			const T1ContentBrowserUtils::ECBFolderCategory NewFolderCategory = T1ContentBrowserUtils::GetFolderCategory(NewSelectedPath);

			if ((bDisplayDev && NewFolderCategory == T1ContentBrowserUtils::ECBFolderCategory::DeveloperContent) || 
				(bDisplayEngine && (NewFolderCategory == T1ContentBrowserUtils::ECBFolderCategory::EngineContent || NewFolderCategory == T1ContentBrowserUtils::ECBFolderCategory::EngineClasses)) || 
				(bDisplayPlugins && (NewFolderCategory == T1ContentBrowserUtils::ECBFolderCategory::PluginContent || NewFolderCategory == T1ContentBrowserUtils::ECBFolderCategory::PluginClasses)) ||
				(bDisplayL10N && T1ContentBrowserUtils::IsLocalizationFolder(NewSelectedPath)))
			{
				// Refresh the contents
				OnPathSelected.ExecuteIfBound(NewSelectedPath);
			}
		}
	}
}




void SFavoritePathView::Construct(const FArguments& InArgs)
{
	SAssignNew(TreeViewPtr, STreeView< TSharedPtr<FT1TreeItem> >)
		.TreeItemsSource(&TreeRootItems)
		.OnGetChildren(this, &SFavoritePathView::GetChildrenForTree)
		.OnGenerateRow(this, &SFavoritePathView::GenerateTreeRow)
		.OnItemScrolledIntoView(this, &SFavoritePathView::TreeItemScrolledIntoView)
		.ItemHeight(18)
		.SelectionMode(InArgs._SelectionMode)
		.OnSelectionChanged(this, &SFavoritePathView::TreeSelectionChanged)
		.OnContextMenuOpening(this, &SFavoritePathView::MakePathViewContextMenu)
		.ClearSelectionOnClick(false);

	ST1PathView::Construct(InArgs);
}

void SFavoritePathView::Populate()
{
	// Don't allow the selection changed delegate to be fired here
	FScopedPreventTreeItemChangedDelegate DelegatePrevention(SharedThis(this));

	// Clear all root items and clear selection
	TreeRootItems.Empty();
	TreeViewPtr->ClearSelection();
	const TArray<FString> FavoritePaths = T1ContentBrowserUtils::GetFavoriteFolders();
	// we have a text filter, expand all parents of matching folders
	for (const FString& Path : FavoritePaths)
	{
		// by sending the whole path we deliberately include any children
		// of successful hits in the filtered list. 
		if (SearchBoxFolderFilter->PassesFilter(Path))
		{
			TSharedPtr<FT1TreeItem> Item = AddPath(Path);
			if (Item.IsValid())
			{
				const bool bSelectedItem = LastSelectedPaths.Contains(Item->FolderPath);

				if (bSelectedItem)
				{
					// Tree items that match the last broadcasted paths should be re-selected them after they are added
					TreeViewPtr->SetItemSelection(Item, true);
					TreeViewPtr->RequestScrollIntoView(Item);
				}
			}
		}
	}

	SortRootItems();
}



void SFavoritePathView::SaveSettings(const FString& IniFilename, const FString& IniSection, const FString& SettingsString) const
{
	ST1PathView::SaveSettings(IniFilename, IniSection, SettingsString);

	FString FavoritePathsString;
	const TArray<FString> FavoritePaths = T1ContentBrowserUtils::GetFavoriteFolders();
	for (const FString& PathIt : FavoritePaths)
	{
		if (FavoritePathsString.Len() > 0)
		{
			FavoritePathsString += TEXT(",");
		}

		FavoritePathsString += PathIt;
	}

	GConfig->SetString(*IniSection, TEXT("FavoritePaths"), *FavoritePathsString, IniFilename);
}

void SFavoritePathView::LoadSettings(const FString& IniFilename, const FString& IniSection, const FString& SettingsString)
{
	ST1PathView::LoadSettings(IniFilename, IniSection, SettingsString);

	// Selected Paths
	FString SelectedPathsString;
	TArray<FString> NewFavoritePaths;
	if (GConfig->GetString(*IniSection, TEXT("FavoritePaths"), SelectedPathsString, IniFilename))
	{
		SelectedPathsString.ParseIntoArray(NewFavoritePaths, TEXT(","), /*bCullEmpty*/true);
	}

	if (NewFavoritePaths.Num() > 0)
	{
		// Keep track if we changed at least one source so we know to fire the bulk selection changed delegate later
		bool bAddedAtLeastOnePath = false;
		{
			// If the selected paths is empty, the path was "All assets"
			// This should handle that case properly
			for (int32 PathIdx = 0; PathIdx < NewFavoritePaths.Num(); ++PathIdx)
			{
				const FString& Path = NewFavoritePaths[PathIdx];
				T1ContentBrowserUtils::AddFavoriteFolder(Path, false);
				bAddedAtLeastOnePath = true;
			}
		}

		if (bAddedAtLeastOnePath)
		{
			Populate();
		}
	}
}

void SFavoritePathView::OnAssetTreeSearchBoxChanged(const FText& InSearchText)
{
	ST1PathView::OnAssetTreeSearchBoxChanged(InSearchText);
	OnFavoriteSearchChanged.ExecuteIfBound(InSearchText);
}

void SFavoritePathView::OnAssetTreeSearchBoxCommitted(const FText& InSearchText, ETextCommit::Type InCommitType)
{
	ST1PathView::OnAssetTreeSearchBoxCommitted(InSearchText, InCommitType);
	OnFavoriteSearchCommitted.ExecuteIfBound(InSearchText, InCommitType);
}

void SFavoritePathView::SetSelectedPaths(const TArray<FString>& Paths)
{
	if (!ensure(TreeViewPtr.IsValid()))
	{
		return;
	}

	if (!SearchBoxPtr->GetText().IsEmpty())
	{
		// Clear the search box so the selected paths will be visible
		SearchBoxPtr->SetText(FText::GetEmpty());
	}

	// Prevent the selection changed delegate since the invoking code requested it
	FScopedPreventTreeItemChangedDelegate DelegatePrevention(SharedThis(this));

	// If the selection was changed before all pending initial paths were found, stop attempting to select them
	PendingInitialPaths.Empty();

	// Clear the selection to start, then add the selected paths as they are found
	TreeViewPtr->ClearSelection();

	for (int32 PathIdx = 0; PathIdx < Paths.Num(); ++PathIdx)
	{
		const FString& Path = Paths[PathIdx];

		TArray<FString> PathItemList;
		Path.ParseIntoArray(PathItemList, TEXT("/"), /*InCullEmpty=*/true);

		if (PathItemList.Num())
		{
			// There is at least one element in the path
			TArray<TSharedPtr<FT1TreeItem>> TreeItems;

			// Find the first item in the root items list
			for (int32 RootItemIdx = 0; RootItemIdx < TreeRootItems.Num(); ++RootItemIdx)
			{
				if (TreeRootItems[RootItemIdx]->FolderName == PathItemList[0])
				{
					// Found the first item in the path
					TreeItems.Add(TreeRootItems[RootItemIdx]);
					break;
				}
			}

			// If found in the root items list, try to find the childmost item matching the path
			if (TreeItems.Num() > 0)
			{
				for (int32 PathItemIdx = 1; PathItemIdx < PathItemList.Num(); ++PathItemIdx)
				{
					const FString& PathItemName = PathItemList[PathItemIdx];
					const TSharedPtr<FT1TreeItem> ChildItem = TreeItems.Last()->GetChild(PathItemName);

					if (ChildItem.IsValid())
					{
						// Update tree items list
						TreeItems.Add(ChildItem);
					}
					else
					{
						// Could not find the child item
						break;
					}
				}

				// Set the selection to the closest found folder and scroll it into view
				TreeViewPtr->SetItemSelection(TreeItems.Last(), true);
				TreeViewPtr->RequestScrollIntoView(TreeItems.Last());
			}
			else
			{
				// Could not even find the root path... skip
			}
		}
		else
		{
			// No path items... skip
		}
	}
}

TSharedPtr<FT1TreeItem> SFavoritePathView::AddPath(const FString& Path, bool bUserNamed /*= false*/)
{
	if (!ensure(TreeViewPtr.IsValid()))
	{
		// No tree view for some reason
		return TSharedPtr<FT1TreeItem>();
	}

	TArray<FString> PathItemList;
	Path.ParseIntoArray(PathItemList, TEXT("/"), /*InCullEmpty=*/true);

	if (PathItemList.Num())
	{
		// There is at least one element in the path
		const FString FolderName = PathItemList.Last();
		const FString FolderPath = Path;

		// Make sure the item is not already in the list
		for (int32 RootItemIdx = 0; RootItemIdx < TreeRootItems.Num(); ++RootItemIdx)
		{
			if (TreeRootItems[RootItemIdx]->FolderPath == FolderPath)
			{
				// The root to add was already in the list return it here
				return TreeRootItems[RootItemIdx];
			}
		}

		TSharedPtr<struct FT1TreeItem> NewItem = nullptr;

		// If this isn't an engine folder or we want to show them, add
		const bool bDisplayEngine = GetDefault<UContentBrowserSettings>()->GetDisplayEngineFolder();
		const bool bDisplayPlugins = GetDefault<UContentBrowserSettings>()->GetDisplayPluginFolders();
		const bool bDisplayCpp = GetDefault<UContentBrowserSettings>()->GetDisplayCppFolders();
		const bool bDisplayLoc = GetDefault<UContentBrowserSettings>()->GetDisplayL10NFolder();
		// Filter out classes folders if we're not showing them.
		if (!bDisplayCpp && T1ContentBrowserUtils::IsClassesFolder(FolderName))
		{
			return nullptr;
		}

		// Filter out plugin folders
		bool bIsPlugin = false;
		EPluginLoadedFrom PluginSource = EPluginLoadedFrom::Engine; // init to avoid warning
		if (!bDisplayEngine || !bDisplayPlugins)
		{
			bIsPlugin = T1ContentBrowserUtils::IsPluginFolder(FolderName, &PluginSource);
		}

		if ((bDisplayEngine || !T1ContentBrowserUtils::IsEngineFolder(FolderName)) &&
			((bDisplayEngine && bDisplayPlugins) || !(bIsPlugin && PluginSource == EPluginLoadedFrom::Engine)) &&
			(bDisplayPlugins || !(bIsPlugin && PluginSource == EPluginLoadedFrom::Project)) &&
			(bDisplayLoc || !T1ContentBrowserUtils::IsLocalizationFolder(FolderName)))
		{
			const FText DisplayName = T1ContentBrowserUtils::GetRootDirDisplayName(FolderName);
			NewItem = MakeShareable(new FT1TreeItem(FText::FromString(FolderName), FolderName, FolderPath, TSharedPtr<FT1TreeItem>()));
			TreeRootItems.Add(NewItem);
			TreeViewPtr->RequestTreeRefresh();
			TreeViewPtr->SetSelection(NewItem);
		}

		return NewItem;
	}
	return TSharedPtr<FT1TreeItem>();
}

TSharedRef<ITableRow> SFavoritePathView::GenerateTreeRow(TSharedPtr<FT1TreeItem> TreeItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	check(TreeItem.IsValid());

	return
		SNew( STableRow< TSharedPtr<FT1TreeItem> >, OwnerTable )
		.OnDragDetected( this, &SFavoritePathView::OnFolderDragDetected )
		[
			SNew(T1SAssetTreeItem)
			.TreeItem(TreeItem)
			.OnNameChanged(this, &SFavoritePathView::FolderNameChanged)
			.OnVerifyNameChanged(this, &SFavoritePathView::VerifyFolderNameChanged)
			.OnAssetsOrPathsDragDropped(this, &SFavoritePathView::TreeAssetsOrPathsDropped)
			.OnFilesDragDropped(this, &SFavoritePathView::TreeFilesDropped)
			.IsItemExpanded(false)
			.HighlightText(this, &SFavoritePathView::GetHighlightText)
			.IsSelected(this, &SFavoritePathView::IsTreeItemSelected, TreeItem)
			.FontOverride(FEditorStyle::GetFontStyle("T1ContentBrowser.SourceTreeItemFont"))
		];
}


void SFavoritePathView::OnAssetRegistryPathAdded(const FString& Path)
{
}

void SFavoritePathView::OnAssetRegistryPathRemoved(const FString& Path)
{
	T1ContentBrowserUtils::RemoveFavoriteFolder(Path);
	Populate();

}

void SFavoritePathView::ExecuteTreeDropMove(TArray<FAssetData> AssetList, TArray<FString> AssetPaths, FString DestinationPath)
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

	FixupFavoritesFromExternalChange(MovedFolders);
	T1ContentBrowserUtils::MoveFolders(AssetPaths, DestinationPath);
}

void SFavoritePathView::FixupFavoritesFromExternalChange(const TArray<FT1MovedContentFolder>& MovedFolders)
{
	for (FT1MovedContentFolder MovedFolder : MovedFolders)
	{
		if (EnumHasAnyFlags(MovedFolder.Flags, ET1MovedContentFolderFlags::Favorite))
		{
			// Remove the original path
			T1ContentBrowserUtils::RemoveFavoriteFolder(MovedFolder.OldPath, false);

			// Add the new path to favorites instead
			const FString NewPath = MovedFolder.NewPath;
			T1ContentBrowserUtils::AddFavoriteFolder(NewPath, false);
			TSharedPtr<FT1TreeItem> Item = FindItemRecursive(NewPath);
			if (Item.IsValid())
			{
				TreeViewPtr->SetItemSelection(Item, true);
				TreeViewPtr->RequestScrollIntoView(Item);
			}
		}
	}
	Populate();
}

#undef LOCTEXT_NAMESPACE