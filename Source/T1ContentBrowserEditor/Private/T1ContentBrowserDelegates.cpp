// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "T1ContentBrowserEditor/Public/T1ContentBrowserDelegates.h"
#include "T1ContentBrowserUtils.h"

FT1MovedContentFolder::FT1MovedContentFolder(const FString& InOldPath, const FString& InNewPath)
	: OldPath(InOldPath)
	, NewPath(InNewPath)
	, Flags(T1ContentBrowserUtils::IsFavoriteFolder(OldPath) ? ET1MovedContentFolderFlags::Favorite : ET1MovedContentFolderFlags::None)
{
	OldPath = InOldPath;
	NewPath = InNewPath;
}