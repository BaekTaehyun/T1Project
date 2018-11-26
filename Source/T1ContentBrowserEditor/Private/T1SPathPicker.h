// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "T1ContentBrowserEditor/Public/IT1ContentBrowserSingleton.h"

class ST1PathView;

/**
 * A sources view designed for path picking
 */
class ST1PathPicker : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS( ST1PathPicker ){}

		/** A struct containing details about how the path picker should behave */
		SLATE_ARGUMENT(FT1PathPickerConfig, PathPickerConfig)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct( const FArguments& InArgs );

	/** Sets the selected paths in this picker */
	void SetPaths(const TArray<FString>& NewPaths);

	/** Return the selected paths in this picker */
	TArray<FString> GetPaths() const;

	/** Return the associated ST1PathView */
	const TSharedPtr<ST1PathView>& GetPathView() const;

	/** Handler for creating a new folder in the path picker */
	void CreateNewFolder(FString FolderPath, FT1OnCreateNewFolder InOnCreateNewFolder);

private:

	/** Handler for the context menu for folder items */
	TSharedPtr<SWidget> GetFolderContextMenu(const TArray<FString>& SelectedPaths, FT1ContentBrowserMenuExtender_SelectedPaths InMenuExtender, FT1OnCreateNewFolder InOnCreateNewFolder);

private:

	/** The path view in this picker */
	TSharedPtr<ST1PathView> PathViewPtr;
};
