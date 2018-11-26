// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "Framework/Commands/Commands.h"

class FT1ContentBrowserCommands
	: public TCommands<FT1ContentBrowserCommands>
{
public:

	/** Default constructor. */
	FT1ContentBrowserCommands()
		: TCommands<FT1ContentBrowserCommands>(TEXT("T1ContentBrowser"), NSLOCTEXT( "T1ContentBrowser", "T1ContentBrowser", "Content Browser" ), NAME_None, FEditorStyle::GetStyleSetName() )
	{ }

public:

	//~ TCommands interface

	virtual void RegisterCommands() override;

public:

	TSharedPtr<FUICommandInfo> CreateNewFolder;
	TSharedPtr<FUICommandInfo> DirectoryUp;
	TSharedPtr<FUICommandInfo> OpenAssetsOrFolders;
	TSharedPtr<FUICommandInfo> PreviewAssets;
	TSharedPtr<FUICommandInfo> SaveSelectedAsset;
	TSharedPtr<FUICommandInfo> SaveAllCurrentFolder;
	TSharedPtr<FUICommandInfo> ResaveAllCurrentFolder;
};
