// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "AssetData.h"
#include "Modules/ModuleInterface.h"
#include "T1ContentBrowserDelegates.h"

class IT1ContentBrowserSingleton;
struct FARFilter;
class FMainMRUFavoritesList;

/**
 * Content browser module
 */
class T1ContentBrowserEditor : public IModuleInterface
{

public:

	/**  */
	DECLARE_MULTICAST_DELEGATE_TwoParams( FT1OnFilterChanged, const FARFilter& /*NewFilter*/, bool /*bIsPrimaryBrowser*/ );
	/** */
	DECLARE_MULTICAST_DELEGATE_TwoParams( FT1OnSearchBoxChanged, const FText& /*InSearchText*/, bool /*bIsPrimaryBrowser*/ );
	/** */
	DECLARE_MULTICAST_DELEGATE_TwoParams( FT1OnAssetSelectionChanged, const TArray<FAssetData>& /*NewSelectedAssets*/, bool /*bIsPrimaryBrowser*/ );
	/** */
	DECLARE_MULTICAST_DELEGATE_OneParam( FT1OnSourcesViewChanged, bool /*bExpanded*/ );
	/** */
	DECLARE_MULTICAST_DELEGATE_OneParam( FT1OnAssetPathChanged, const FString& /*NewPath*/ );

	/**
	 * Called right after the plugin DLL has been loaded and the plugin object has been created
	 */
	virtual void StartupModule();

	/**
	 * Called before the plugin is unloaded, right before the plugin object is destroyed.
	 */
	virtual void ShutdownModule();

	/** Gets the content browser singleton */
	virtual IT1ContentBrowserSingleton& Get() const;

	/** Delegates to be called to extend the content browser menus */
	virtual TArray<FT1ContentBrowserMenuExtender_SelectedPaths>& GetAllAssetContextMenuExtenders() {return AssetContextMenuExtenders;}
	virtual TArray<FT1ContentBrowserMenuExtender_SelectedPaths>& GetAllPathViewContextMenuExtenders() {return PathViewContextMenuExtenders;}
	virtual TArray<FT1ContentBrowserMenuExtender>& GetAllCollectionListContextMenuExtenders() {return CollectionListContextMenuExtenders;}
	virtual TArray<FT1ContentBrowserMenuExtender>& GetAllCollectionViewContextMenuExtenders() {return CollectionViewContextMenuExtenders;}
	virtual TArray<FT1ContentBrowserMenuExtender_SelectedAssets>& GetAllAssetViewContextMenuExtenders() {return AssetViewContextMenuExtenders;}
	virtual TArray<FT1ContentBrowserMenuExtender>& GetAllAssetViewViewMenuExtenders() {return AssetViewViewMenuExtenders;}

	/** Delegates to call to extend the command/keybinds for content browser */
	virtual TArray<FT1ContentBrowserCommandExtender>& GetAllContentBrowserCommandExtenders() { return ContentBrowserCommandExtenders; }

	/** Delegates to be called to extend the drag-and-drop support of the asset view */
	virtual TArray<FT1AssetViewDragAndDropExtender>& GetAssetViewDragAndDropExtenders() { return AssetViewDragAndDropExtenders; }

	/** Delegate accessors */
	FT1OnFilterChanged& GetOnFilterChanged() { return OnFilterChanged; } 
	FT1OnSearchBoxChanged& GetOnSearchBoxChanged() { return OnSearchBoxChanged; } 
	FT1OnAssetSelectionChanged& GetOnAssetSelectionChanged() { return OnAssetSelectionChanged; } 
	FT1OnSourcesViewChanged& GetOnSourcesViewChanged() { return OnSourcesViewChanged; }
	FT1OnAssetPathChanged& GetOnAssetPathChanged() { return OnAssetPathChanged; }

	FMainMRUFavoritesList* GetRecentlyOpenedAssets() const
	{
		return RecentlyOpenedAssets.Get();
	};

	static const FName NumberOfRecentAssetsName;

private:
	/** Resize the recently opened asset list */
	void ResizeRecentAssetList(FName InName);

private:
	IT1ContentBrowserSingleton* ContentBrowserSingleton;
	TSharedPtr<class FContentBrowserSpawner> ContentBrowserSpawner;

	/** All extender delegates for the content browser menus */
	TArray<FT1ContentBrowserMenuExtender_SelectedPaths> AssetContextMenuExtenders;
	TArray<FT1ContentBrowserMenuExtender_SelectedPaths> PathViewContextMenuExtenders;
	TArray<FT1ContentBrowserMenuExtender> CollectionListContextMenuExtenders;
	TArray<FT1ContentBrowserMenuExtender> CollectionViewContextMenuExtenders;
	TArray<FT1ContentBrowserMenuExtender_SelectedAssets> AssetViewContextMenuExtenders;
	TArray<FT1ContentBrowserMenuExtender> AssetViewViewMenuExtenders;
	TArray<FT1ContentBrowserCommandExtender> ContentBrowserCommandExtenders;

	/** All extender delegates for the drag-and-drop support of the asset view */
	TArray<FT1AssetViewDragAndDropExtender> AssetViewDragAndDropExtenders;

	TUniquePtr<FMainMRUFavoritesList> RecentlyOpenedAssets;

	FT1OnFilterChanged OnFilterChanged;
	FT1OnSearchBoxChanged OnSearchBoxChanged;
	FT1OnAssetSelectionChanged OnAssetSelectionChanged;
	FT1OnSourcesViewChanged OnSourcesViewChanged;
	FT1OnAssetPathChanged OnAssetPathChanged;
};
