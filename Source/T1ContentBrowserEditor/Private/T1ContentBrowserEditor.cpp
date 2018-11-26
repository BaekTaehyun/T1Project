// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.


#include "T1ContentBrowserEditor/Public/T1ContentBrowserEditor.h"
#include "T1ContentBrowserLog.h"
#include "T1ContentBrowserSingleton.h"
#include "MRUFavoritesList.h"
#include "Settings/ContentBrowserSettings.h"

IMPLEMENT_MODULE(T1ContentBrowserEditor, T1ContentBrowser );
DEFINE_LOG_CATEGORY(LogContentBrowser);
const FName T1ContentBrowserEditor::NumberOfRecentAssetsName(TEXT("NumObjectsInRecentList"));

void T1ContentBrowserEditor::StartupModule()
{
	ContentBrowserSingleton = new FT1ContentBrowserSingleton();
	
	RecentlyOpenedAssets = MakeUnique<FMainMRUFavoritesList>(TEXT("T1ContentBrowserRecent"), GetDefault<UContentBrowserSettings>()->NumObjectsInRecentList);
	RecentlyOpenedAssets->ReadFromINI();

	UContentBrowserSettings::OnSettingChanged().AddRaw(this, &T1ContentBrowserEditor::ResizeRecentAssetList);
}

void T1ContentBrowserEditor::ShutdownModule()
{	
	if ( ContentBrowserSingleton )
	{
		delete ContentBrowserSingleton;
		ContentBrowserSingleton = NULL;
	}
	UContentBrowserSettings::OnSettingChanged().RemoveAll(this);
	RecentlyOpenedAssets.Reset();
}

IT1ContentBrowserSingleton& T1ContentBrowserEditor::Get() const
{
	check(ContentBrowserSingleton);
	return *ContentBrowserSingleton;
}

void T1ContentBrowserEditor::ResizeRecentAssetList(FName InName)
{
	if (InName == NumberOfRecentAssetsName)
	{
		RecentlyOpenedAssets->WriteToINI();
		RecentlyOpenedAssets = MakeUnique<FMainMRUFavoritesList>(TEXT("T1ContentBrowserRecent"), GetDefault<UContentBrowserSettings>()->NumObjectsInRecentList);
		RecentlyOpenedAssets->ReadFromINI();
	}
}