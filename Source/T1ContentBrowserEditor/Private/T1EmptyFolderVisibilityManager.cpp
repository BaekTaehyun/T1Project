// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "T1EmptyFolderVisibilityManager.h"
#include "AssetRegistryModule.h"
#include "T1ContentBrowserUtils.h"
#include "Settings/ContentBrowserSettings.h"
#include "Misc/Paths.h"

FT1EmptyFolderVisibilityManager::FT1EmptyFolderVisibilityManager()
{
	// Load the asset registry module to listen for updates
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	AssetRegistryModule.Get().OnPathRemoved().AddRaw(this, &FT1EmptyFolderVisibilityManager::OnAssetRegistryPathRemoved);
	AssetRegistryModule.Get().OnAssetAdded().AddRaw(this, &FT1EmptyFolderVisibilityManager::OnAssetRegistryAssetAdded);

	// Query all paths currently gathered from the asset registry
	TArray<FString> PathList;
	AssetRegistryModule.Get().GetAllCachedPaths(PathList);
	for (const FString& Path : PathList)
	{
		const bool bPathIsEmpty = T1ContentBrowserUtils::IsEmptyFolder(Path, true);
		if (!bPathIsEmpty)
		{
			PathsToAlwaysShow.Add(Path);
		}
	}
}

FT1EmptyFolderVisibilityManager::~FT1EmptyFolderVisibilityManager()
{
	if (FModuleManager::Get().IsModuleLoaded(TEXT("AssetRegistry")))
	{
		// Get the asset registry module to stop listening for updates
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		AssetRegistryModule.Get().OnPathRemoved().RemoveAll(this);
		AssetRegistryModule.Get().OnAssetAdded().RemoveAll(this);
	}
}

bool FT1EmptyFolderVisibilityManager::ShouldShowPath(const FString& InPath) const
{
	const bool bDisplayEmpty = GetDefault<UContentBrowserSettings>()->DisplayEmptyFolders;
	if (bDisplayEmpty)
	{
		return true;
	}

	const bool bPathIsEmpty = T1ContentBrowserUtils::IsEmptyFolder(InPath, true);
	return !bPathIsEmpty || PathsToAlwaysShow.Contains(InPath);
}

void FT1EmptyFolderVisibilityManager::SetAlwaysShowPath(const FString& InPath)
{
	if (InPath.IsEmpty())
	{
		return;
	}

	FString PathToAdd = InPath;

	bool bWasAlreadyShown = false;
	if (PathToAdd[PathToAdd.Len() - 1] == TEXT('/'))
	{
		PathToAdd = PathToAdd.Mid(0, PathToAdd.Len() - 1);
	}
	PathsToAlwaysShow.Add(PathToAdd, &bWasAlreadyShown);

	if (!bWasAlreadyShown)
	{
		OnFolderPopulatedDelegate.Broadcast(PathToAdd);

		// We also need to make sure the parents of this path are on the visible list too
		SetAlwaysShowPath(FPaths::GetPath(PathToAdd));
	}
}

void FT1EmptyFolderVisibilityManager::OnAssetRegistryPathRemoved(const FString& InPath)
{
	PathsToAlwaysShow.Remove(InPath);
}

void FT1EmptyFolderVisibilityManager::OnAssetRegistryAssetAdded(const FAssetData& InAssetData)
{
	SetAlwaysShowPath(InAssetData.PackagePath.ToString());
}
