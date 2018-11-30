// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.


#include "T1SPathPicker.h"
#include "Framework/Commands/UIAction.h"
#include "Textures/SlateIcon.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "EditorStyleSet.h"
#include "T1ContentBrowserUtils.h"
#include "T1SPathView.h"

#define LOCTEXT_NAMESPACE "T1ContentBrowserEditor"


void ST1PathPicker::Construct( const FArguments& InArgs )
{
	for (auto DelegateIt = InArgs._PathPickerConfig.SetPathsDelegates.CreateConstIterator(); DelegateIt; ++DelegateIt)
	{
		if ((*DelegateIt) != NULL)
		{
			(**DelegateIt) = FT1SetPathPickerPathsDelegate::CreateSP(this, &ST1PathPicker::SetPaths);
		}
	}

	FT1OnGetFolderContextMenu OnGetFolderContextMenuDelegate = InArgs._PathPickerConfig.OnGetFolderContextMenu.IsBound() ? InArgs._PathPickerConfig.OnGetFolderContextMenu : FT1OnGetFolderContextMenu::CreateSP(this, &ST1PathPicker::GetFolderContextMenu);

	ChildSlot
	[
		SAssignNew(PathViewPtr, ST1PathView)
		.OnPathSelected(InArgs._PathPickerConfig.OnPathSelected)
		.OnGetFolderContextMenu(OnGetFolderContextMenuDelegate)
		.OnGetPathContextMenuExtender(InArgs._PathPickerConfig.OnGetPathContextMenuExtender)
		.FocusSearchBoxWhenOpened(InArgs._PathPickerConfig.bFocusSearchBoxWhenOpened)
		.AllowContextMenu(InArgs._PathPickerConfig.bAllowContextMenu)
		.AllowClassesFolder(InArgs._PathPickerConfig.bAllowClassesFolder)
		.SelectionMode(ESelectionMode::Single)
	];

	const FString& DefaultPath = InArgs._PathPickerConfig.DefaultPath;
	if ( !DefaultPath.IsEmpty() )
	{
		if (InArgs._PathPickerConfig.bAddDefaultPath)
		{
			PathViewPtr->AddPath(DefaultPath, false);
		}

		TArray<FString> SelectedPaths;
		SelectedPaths.Add(DefaultPath);
		PathViewPtr->SetSelectedPaths(SelectedPaths);
	}
}

TSharedPtr<SWidget> ST1PathPicker::GetFolderContextMenu(const TArray<FString>& SelectedPaths, FT1ContentBrowserMenuExtender_SelectedPaths InMenuExtender, FT1OnCreateNewFolder InOnCreateNewFolder)
{
	TSharedPtr<FExtender> Extender;
	if (InMenuExtender.IsBound())
	{
		Extender = InMenuExtender.Execute(SelectedPaths);
	}

	const bool bInShouldCloseWindowAfterSelection = true;
	const bool bCloseSelfOnly = true;
	FMenuBuilder MenuBuilder(bInShouldCloseWindowAfterSelection, nullptr, Extender, bCloseSelfOnly);

	// We can only create folders when we have a single path selected
	const bool bCanCreateNewFolder = SelectedPaths.Num() == 1 && T1ContentBrowserUtils::IsValidPathToCreateNewFolder(SelectedPaths[0]);

	FText NewFolderToolTip;
	if(SelectedPaths.Num() == 1)
	{
		if(bCanCreateNewFolder)
		{
			NewFolderToolTip = FText::Format(LOCTEXT("NewFolderTooltip_CreateIn", "Create a new folder in {0}."), FText::FromString(SelectedPaths[0]));
		}
		else
		{
			NewFolderToolTip = FText::Format(LOCTEXT("NewFolderTooltip_InvalidPath", "Cannot create new folders in {0}."), FText::FromString(SelectedPaths[0]));
		}
	}
	else
	{
		NewFolderToolTip = LOCTEXT("NewFolderTooltip_InvalidNumberOfPaths", "Can only create folders when there is a single path selected.");
	}

	// New Folder
	MenuBuilder.AddMenuEntry(
		LOCTEXT("NewFolder", "New Folder"),
		NewFolderToolTip,
		FSlateIcon(FEditorStyle::GetStyleSetName(), "T1ContentBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateSP(this, &ST1PathPicker::CreateNewFolder, SelectedPaths.Num() > 0 ? SelectedPaths[0] : FString(), InOnCreateNewFolder),
			FCanExecuteAction::CreateLambda( [bCanCreateNewFolder] { return bCanCreateNewFolder; } )
			),
		"NewFolder"
		);

	return MenuBuilder.MakeWidget();
}

void ST1PathPicker::CreateNewFolder(FString FolderPath, FT1OnCreateNewFolder InOnCreateNewFolder)
{
	// Create a valid base name for this folder
	FText DefaultFolderBaseName = LOCTEXT("DefaultFolderName", "NewFolder");
	FText DefaultFolderName = DefaultFolderBaseName;
	int32 NewFolderPostfix = 1;
	while (T1ContentBrowserUtils::DoesFolderExist(FolderPath / DefaultFolderName.ToString()))
	{
		DefaultFolderName = FText::Format(LOCTEXT("DefaultFolderNamePattern", "{0}{1}"), DefaultFolderBaseName, FText::AsNumber(NewFolderPostfix));
		NewFolderPostfix++;
	}

	InOnCreateNewFolder.ExecuteIfBound(DefaultFolderName.ToString(), FolderPath);
}

void ST1PathPicker::SetPaths(const TArray<FString>& NewPaths)
{
	PathViewPtr->SetSelectedPaths(NewPaths);
}

TArray<FString> ST1PathPicker::GetPaths() const
{
	return PathViewPtr->GetSelectedPaths();
}

const TSharedPtr<ST1PathView>& ST1PathPicker::GetPathView() const
{
	return PathViewPtr;
}

#undef LOCTEXT_NAMESPACE
