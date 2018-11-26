// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "T1SAssetDialog.h"
#include "Misc/MessageDialog.h"
#include "Widgets/SBoxPanel.h"
#include "Layout/WidgetPath.h"
#include "SlateOptMacros.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SSplitter.h"
#include "EditorStyleSet.h"
#include "AssetRegistryModule.h"
#include "T1ContentBrowserSingleton.h"
#include "T1ContentBrowserUtils.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "T1ContentBrowserCommands.h"
#include "Framework/Commands/GenericCommands.h"
#include "T1SPathPicker.h"
#include "T1SAssetPicker.h"
#include "T1AssetViewTypes.h"
#include "ObjectTools.h"

#include "T1SPathView.h"
#include "T1SAssetView.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Editor/EditorEngine.h"
#include "HAL/FileManager.h"
#include "T1NativeClassHierarchy.h"
#include "CoreMinimal.h"
#include "SourceCodeNavigation.h"
#include "Editor.h"

#define LOCTEXT_NAMESPACE "T1ContentBrowserEditor"

ST1AssetDialog::ST1AssetDialog()
	: DialogType(ET1AssetDialogType::Open)
	, ExistingAssetPolicy(ET1SaveAssetDialogExistingAssetPolicy::Disallow)
	, bLastInputValidityCheckSuccessful(false)
	, bPendingFocusNextFrame(true)
	, bValidAssetsChosen(false)
	, OpenedContextMenuWidget(ET1OpenedContextMenuWidget::None)
{
}

ST1AssetDialog::~ST1AssetDialog()
{
	if (!bValidAssetsChosen)
	{
		OnAssetDialogCancelled.ExecuteIfBound();
	}
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void ST1AssetDialog::Construct(const FArguments& InArgs, const FT1SharedAssetDialogConfig& InConfig)
{
	DialogType = InConfig.GetDialogType();

	AssetClassNames = InConfig.AssetClassNames;

	const FString DefaultPath = InConfig.DefaultPath;

	RegisterActiveTimer( 0.f, FWidgetActiveTimerDelegate::CreateSP( this, &ST1AssetDialog::SetFocusPostConstruct ) );

	FT1PathPickerConfig PathPickerConfig;
	PathPickerConfig.DefaultPath = DefaultPath;
	PathPickerConfig.bFocusSearchBoxWhenOpened = false;
	PathPickerConfig.OnPathSelected = FT1OnPathSelected::CreateSP(this, &ST1AssetDialog::HandlePathSelected);
	PathPickerConfig.SetPathsDelegates.Add(&SetPathsDelegate);
	PathPickerConfig.OnGetFolderContextMenu = FT1OnGetFolderContextMenu::CreateSP(this, &ST1AssetDialog::OnGetFolderContextMenu);	

	FT1AssetPickerConfig AssetPickerConfig;
	AssetPickerConfig.Filter.ClassNames.Append(AssetClassNames);
	AssetPickerConfig.Filter.PackagePaths.Add(FName(*DefaultPath));
	AssetPickerConfig.bAllowDragging = false;
	AssetPickerConfig.InitialAssetViewType = ET1AssetViewType::Tile;
	AssetPickerConfig.OnAssetSelected = FT1OnAssetSelected::CreateSP(this, &ST1AssetDialog::OnAssetSelected);
	AssetPickerConfig.OnAssetsActivated = FT1OnAssetsActivated::CreateSP(this, &ST1AssetDialog::OnAssetsActivated);
	AssetPickerConfig.SetFilterDelegates.Add(&SetFilterDelegate);
	AssetPickerConfig.GetCurrentSelectionDelegates.Add(&GetCurrentSelectionDelegate);
	AssetPickerConfig.SaveSettingsName = TEXT("AssetDialog");
	AssetPickerConfig.bCanShowFolders = true;
	AssetPickerConfig.bCanShowDevelopersFolder = true;
	AssetPickerConfig.OnFolderEntered = FT1OnPathSelected::CreateSP(this, &ST1AssetDialog::HandleAssetViewFolderEntered);
	AssetPickerConfig.OnGetAssetContextMenu = FT1OnGetAssetContextMenu::CreateSP(this, &ST1AssetDialog::OnGetAssetContextMenu);
	AssetPickerConfig.OnGetFolderContextMenu = FT1OnGetFolderContextMenu::CreateSP(this, &ST1AssetDialog::OnGetFolderContextMenu);

	SetCurrentlySelectedPath(DefaultPath);

	// Open and save specific configuration
	FText ConfirmButtonText;
	bool bIncludeNameBox = false;
	if ( DialogType == ET1AssetDialogType::Open )
	{
		const FT1OpenAssetDialogConfig& OpenAssetConfig = static_cast<const FT1OpenAssetDialogConfig&>(InConfig);
		PathPickerConfig.bAllowContextMenu = true;
		ConfirmButtonText = LOCTEXT("AssetDialogOpenButton", "Open");
		AssetPickerConfig.SelectionMode = OpenAssetConfig.bAllowMultipleSelection ? ESelectionMode::Multi : ESelectionMode::Single;
		AssetPickerConfig.bFocusSearchBoxWhenOpened = true;
		bIncludeNameBox = false;
	}
	else if ( DialogType == ET1AssetDialogType::Save )
	{
		const FT1SaveAssetDialogConfig& SaveAssetConfig = static_cast<const FT1SaveAssetDialogConfig&>(InConfig);
		PathPickerConfig.bAllowContextMenu = true;
		ConfirmButtonText = LOCTEXT("AssetDialogSaveButton", "Save");
		AssetPickerConfig.SelectionMode = ESelectionMode::Single;
		AssetPickerConfig.bFocusSearchBoxWhenOpened = false;
		bIncludeNameBox = true;
		ExistingAssetPolicy = SaveAssetConfig.ExistingAssetPolicy;
		SetCurrentlyEnteredAssetName(SaveAssetConfig.DefaultAssetName);
	}
	else
	{
		ensureMsgf(0, TEXT("AssetDialog type %d is not supported."), DialogType);
	}

	PathPicker = StaticCastSharedRef<ST1PathPicker>(FT1ContentBrowserSingleton::Get().CreatePathPicker(PathPickerConfig));
	AssetPicker = StaticCastSharedRef<T1SAssetPicker>(FT1ContentBrowserSingleton::Get().CreateAssetPicker(AssetPickerConfig));

	FT1ContentBrowserCommands::Register();
	BindCommands();

	// The root widget in this dialog.
	TSharedRef<SVerticalBox> MainVerticalBox = SNew(SVerticalBox);

	// Path/Asset view
	MainVerticalBox->AddSlot()
		.FillHeight(1)
		.Padding(0, 0, 0, 4)
		[
			SNew(SSplitter)
		
			+SSplitter::Slot()
			.Value(0.25f)
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
				[
					PathPicker.ToSharedRef()
				]
			]

			+SSplitter::Slot()
			.Value(0.75f)
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
				[
					AssetPicker.ToSharedRef()
				]
			]
		];

	// Input error strip, if we are using a name box
	if (bIncludeNameBox)
	{
		// Name Error label
		MainVerticalBox->AddSlot()
		.AutoHeight()
		[
			// Constant height, whether the label is visible or not
			SNew(SBox).HeightOverride(18)
			[
				SNew(SBorder)
				.Visibility( this, &ST1AssetDialog::GetNameErrorLabelVisibility )
				.BorderImage( FEditorStyle::GetBrush("AssetDialog.ErrorLabelBorder") )
				.Content()
				[
					SNew(STextBlock)
					.Text( this, &ST1AssetDialog::GetNameErrorLabelText )
					.TextStyle( FEditorStyle::Get(), "AssetDialog.ErrorLabelFont" )
				]
			]
		];
	}

	TSharedRef<SVerticalBox> LabelsBox = SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1)
		.VAlign(VAlign_Center)
		.Padding(0, 2, 0, 2)
		[
			SNew(STextBlock).Text(LOCTEXT("PathBoxLabel", "Path:"))
		];

	TSharedRef<SVerticalBox> ContentBox = SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1)
		.VAlign(VAlign_Center)
		.Padding(0, 2, 0, 2)
		[
			SAssignNew(PathText, STextBlock)
			.Text(this, &ST1AssetDialog::GetPathNameText)
		];

	if (bIncludeNameBox)
	{
		LabelsBox->AddSlot()
			.FillHeight(1)
			.VAlign(VAlign_Center)
			.Padding(0, 2, 0, 2)
			[
				SNew(STextBlock).Text(LOCTEXT("NameBoxLabel", "Name:"))
			];

		ContentBox->AddSlot()
			.FillHeight(1)
			.VAlign(VAlign_Center)
			.Padding(0, 2, 0, 2)
			[
				SAssignNew(NameEditableText, SEditableTextBox)
				.Text(this, &ST1AssetDialog::GetAssetNameText)
				.OnTextCommitted(this, &ST1AssetDialog::OnAssetNameTextCommited)
				.OnTextChanged(this, &ST1AssetDialog::OnAssetNameTextCommited, ETextCommit::Default)
				.SelectAllTextWhenFocused(true)
			];
	}

	// Buttons and asset name
	TSharedRef<SHorizontalBox> ButtonsAndNameBox = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Bottom)
		.Padding(bIncludeNameBox ? 80 : 4, 20, 4, 3)
		[
			LabelsBox
		]
		+ SHorizontalBox::Slot()
		.FillWidth(1)
		.VAlign(VAlign_Bottom)
		.Padding(4, 3)
		[
			ContentBox
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Bottom)
		.Padding(4, 3)
		[
			SNew(SButton)
			.Text(ConfirmButtonText)
			.ContentPadding(FMargin(8, 2, 8, 2))
			.IsEnabled(this, &ST1AssetDialog::IsConfirmButtonEnabled)
			.OnClicked(this, &ST1AssetDialog::OnConfirmClicked)
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Bottom)
		.Padding(4, 3)
		[
			SNew(SButton)
			.ContentPadding(FMargin(8, 2, 8, 2))
			.Text(LOCTEXT("AssetDialogCancelButton", "Cancel"))
			.OnClicked(this, &ST1AssetDialog::OnCancelClicked)
		];

	MainVerticalBox->AddSlot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.Padding(0)
		[
			ButtonsAndNameBox
		];

	ChildSlot
	[
		MainVerticalBox
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply ST1AssetDialog::OnKeyDown( const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent )
{
	if ( InKeyEvent.GetKey() == EKeys::Escape )
	{
		CloseDialog();
		return FReply::Handled();
	}
	else if (Commands->ProcessCommandBindings(InKeyEvent))
	{
		return FReply::Handled();
	}

	return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
}

void ST1AssetDialog::BindCommands()
{
	Commands = TSharedPtr< FUICommandList >(new FUICommandList);

	Commands->MapAction(FGenericCommands::Get().Rename, FUIAction(
		FExecuteAction::CreateSP(this, &ST1AssetDialog::ExecuteRename),
		FCanExecuteAction::CreateSP(this, &ST1AssetDialog::CanExecuteRename)
	));

	Commands->MapAction(FGenericCommands::Get().Delete, FUIAction(
		FExecuteAction::CreateSP(this, &ST1AssetDialog::ExecuteDelete),
		FCanExecuteAction::CreateSP(this, &ST1AssetDialog::CanExecuteDelete)
	));

	Commands->MapAction(FT1ContentBrowserCommands::Get().CreateNewFolder, FUIAction(
		FExecuteAction::CreateSP(this, &ST1AssetDialog::ExecuteCreateNewFolder),
		FCanExecuteAction::CreateSP(this, &ST1AssetDialog::CanExecuteCreateNewFolder)
	));
}

bool ST1AssetDialog::CanExecuteRename() const
{
	switch (OpenedContextMenuWidget)
	{
		case ET1OpenedContextMenuWidget::AssetView: return T1ContentBrowserUtils::CanRenameFromAssetView(AssetPicker->GetAssetView());
		case ET1OpenedContextMenuWidget::PathView: return T1ContentBrowserUtils::CanRenameFromPathView(PathPicker->GetPaths());
	}

	return false;
}

void ST1AssetDialog::ExecuteRename()
{
	TArray< FAssetData > AssetViewSelectedAssets = AssetPicker->GetAssetView()->GetSelectedAssets();
	TArray< FString > SelectedFolders = AssetPicker->GetAssetView()->GetSelectedFolders();

	if (SelectedFolders.Num() > 0 || AssetViewSelectedAssets.Num() > 0)
	{
		if (AssetViewSelectedAssets.Num() == 1 && SelectedFolders.Num() == 0)
		{
			// Don't operate on Redirectors
			if (AssetViewSelectedAssets[0].AssetClass != UObjectRedirector::StaticClass()->GetFName())
			{
				AssetPicker->GetAssetView()->RenameAsset(AssetViewSelectedAssets[0]);
			}
		}
		else if (AssetViewSelectedAssets.Num() == 0 && SelectedFolders.Num() == 1)
		{
			AssetPicker->GetAssetView()->RenameFolder(SelectedFolders[0]);
		}
	}
	else
	{		
		const TArray<FString>& SelectedPaths = PathPicker->GetPathView()->GetSelectedPaths();

		if (SelectedPaths.Num() == 1)
		{
			PathPicker->GetPathView()->RenameFolder(SelectedPaths[0]);
		}
	}
}

bool ST1AssetDialog::CanExecuteDelete() const
{
	switch (OpenedContextMenuWidget)
	{
		case ET1OpenedContextMenuWidget::AssetView: return T1ContentBrowserUtils::CanDeleteFromAssetView(AssetPicker->GetAssetView());
		case ET1OpenedContextMenuWidget::PathView: return T1ContentBrowserUtils::CanDeleteFromPathView(PathPicker->GetPaths());
	}

	return false;
}

void ST1AssetDialog::ExecuteDelete()
{
	// Don't allow asset deletion during PIE
	if (GIsEditor)
	{
		UEditorEngine* Editor = GEditor;
		FWorldContext* PIEWorldContext = GEditor->GetPIEWorldContext();
		if (PIEWorldContext)
		{
			FNotificationInfo Notification(LOCTEXT("CannotDeleteAssetInPIE", "Assets cannot be deleted while in PIE."));
			Notification.ExpireDuration = 3.0f;
			FSlateNotificationManager::Get().AddNotification(Notification);
			return;
		}
	}

	TArray<FString> SelectedFolders = AssetPicker->GetAssetView()->GetSelectedFolders();
	TArray<FAssetData> SelectedAssets = AssetPicker->GetAssetView()->GetSelectedAssets();

	if (SelectedFolders.Num() == 0)
	{
		SelectedFolders = PathPicker->GetPaths();
	}

	if (SelectedAssets.Num() > 0 && OpenedContextMenuWidget == ET1OpenedContextMenuWidget::AssetView)
	{
		TArray<FAssetData> AssetsToDelete;
		AssetsToDelete.Reserve(SelectedAssets.Num());

		for (auto AssetData : SelectedAssets)
		{
			// Don't operate on Redirectors
			if (AssetData.AssetClass != UObjectRedirector::StaticClass()->GetFName())
			{
				AssetsToDelete.Add(AssetData);
			}
		}

		if (AssetsToDelete.Num() > 0)
		{
			ObjectTools::DeleteAssets(AssetsToDelete);
		}
	}

	if (SelectedFolders.Num() > 0)
	{
		FText Prompt;
		if (SelectedFolders.Num() == 1)
		{
			Prompt = FText::Format(LOCTEXT("FolderDeleteConfirm_Single", "Delete folder '{0}'?"), FText::FromString(SelectedFolders[0]));
		}
		else
		{
			Prompt = FText::Format(LOCTEXT("FolderDeleteConfirm_Multiple", "Delete {0} folders?"), FText::AsNumber(SelectedFolders.Num()));
		}

		// Spawn a confirmation dialog since this is potentially a highly destructive operation
		FOnClicked OnYesClicked = FOnClicked::CreateSP(this, &ST1AssetDialog::ExecuteDeleteFolderConfirmed);
		T1ContentBrowserUtils::DisplayConfirmationPopup(Prompt, LOCTEXT("FolderDeleteConfirm_Yes", "Delete"), LOCTEXT("FolderDeleteConfirm_No", "Cancel"), AssetPicker->GetAssetView().ToSharedRef(), OnYesClicked);
	}
}

FReply ST1AssetDialog::ExecuteDeleteFolderConfirmed()
{
	TArray< FString > SelectedFolders = AssetPicker->GetAssetView()->GetSelectedFolders();

	if (SelectedFolders.Num() > 0)
	{
		T1ContentBrowserUtils::DeleteFolders(SelectedFolders);
	}
	else
	{
		const TArray<FString>& SelectedPaths = PathPicker->GetPaths();

		if (SelectedPaths.Num() > 0)
		{
			if (T1ContentBrowserUtils::DeleteFolders(SelectedPaths))
			{
				// Since the contents of the asset view have just been deleted, set the selected path to the default "/Game"
				TArray<FString> DefaultSelectedPaths;
				DefaultSelectedPaths.Add(TEXT("/Game"));
				PathPicker->GetPathView()->SetSelectedPaths(DefaultSelectedPaths);

				FT1SourcesData DefaultSourcesData(FName("/Game"));
				AssetPicker->GetAssetView()->SetSourcesData(DefaultSourcesData);
			}
		}
	}

	return FReply::Handled();
}

void ST1AssetDialog::ExecuteExplore()
{
	TArray<FString> SelectedFolders = AssetPicker->GetAssetView()->GetSelectedFolders();
	TArray<FAssetData> SelectedAssets = AssetPicker->GetAssetView()->GetSelectedAssets();

	if (SelectedFolders.Num() == 0 && SelectedAssets.Num() == 0)
	{
		SelectedFolders = PathPicker->GetPaths();
	}

	FString PathToExplore;

	if (SelectedFolders.Num() > 0 && SelectedAssets.Num() == 0)
	{
		for (int32 PathIdx = 0; PathIdx < SelectedFolders.Num(); ++PathIdx)
		{
			const FString& Path = SelectedFolders[PathIdx];

			FString FilePath;
			if (T1ContentBrowserUtils::IsClassPath(Path))
			{
				TSharedRef<FT1NativeClassHierarchy> NativeClassHierarchy = FT1ContentBrowserSingleton::Get().GetNativeClassHierarchy();
				if (NativeClassHierarchy->GetFileSystemPath(Path, FilePath))
				{
					FilePath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FilePath);
				}
			}
			else
			{
				FilePath = FPaths::ConvertRelativePathToFull(FPackageName::LongPackageNameToFilename(Path + TEXT("/")));
			}

			if (!FilePath.IsEmpty())
			{
				// If the folder has not yet been created, make is right before we try to explore to it
				if (!IFileManager::Get().DirectoryExists(*FilePath))
				{
					IFileManager::Get().MakeDirectory(*FilePath, /*Tree=*/true);
				}

				PathToExplore = FilePath;
			}
		}
	}
	else
	{
		for (int32 AssetIdx = 0; AssetIdx < SelectedAssets.Num(); ++AssetIdx)
		{
			const UObject* Asset = SelectedAssets[AssetIdx].GetAsset();
			if (Asset)
			{
				FAssetData AssetData(Asset);
				const FString PackageName = AssetData.PackageName.ToString();
				static const TCHAR* ScriptString = TEXT("/Script/");

				if (PackageName.StartsWith(ScriptString))
				{
					// Handle C++ classes specially, as FPackageName::LongPackageNameToFilename won't return the correct path in this case
					const FString ModuleName = PackageName.RightChop(FCString::Strlen(ScriptString));

					FString ModulePath;
					if (FSourceCodeNavigation::FindModulePath(ModuleName, ModulePath))
					{
						FString RelativePath;
						if (AssetData.GetTagValue("ModuleRelativePath", RelativePath))
						{
							PathToExplore = FPaths::ConvertRelativePathToFull(ModulePath / (*RelativePath));
						}
					}
				}
				else
				{
					const bool bIsWorldAsset = (AssetData.AssetClass == UWorld::StaticClass()->GetFName());
					const FString Extension = bIsWorldAsset ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension();
					const FString FilePath = FPackageName::LongPackageNameToFilename(PackageName, Extension);

					PathToExplore = FPaths::ConvertRelativePathToFull(FilePath);
				}
			}
		}
	}

	if (!PathToExplore.IsEmpty())
	{
		FPlatformProcess::ExploreFolder(*PathToExplore);
	}
}

bool ST1AssetDialog::CanExecuteCreateNewFolder() const
{	
	// We can only create folders when we have a single path selected
	return T1ContentBrowserUtils::IsValidPathToCreateNewFolder(CurrentlySelectedPath);
}

void ST1AssetDialog::ExecuteCreateNewFolder()
{
	PathPicker->CreateNewFolder(CurrentlySelectedPath, CurrentContextMenuCreateNewFolderDelegate);
}

TSharedPtr<SWidget> ST1AssetDialog::OnGetFolderContextMenu(const TArray<FString>& SelectedPaths, FT1ContentBrowserMenuExtender_SelectedPaths InMenuExtender, FT1OnCreateNewFolder InOnCreateNewFolder)
{
	if (FSlateApplication::Get().HasFocusedDescendants(PathPicker.ToSharedRef()))
	{
		OpenedContextMenuWidget = ET1OpenedContextMenuWidget::PathView;
	}
	else if (FSlateApplication::Get().HasFocusedDescendants(AssetPicker.ToSharedRef()))
	{
		OpenedContextMenuWidget = ET1OpenedContextMenuWidget::AssetView;
	}

	TSharedPtr<FExtender> Extender;
	if (InMenuExtender.IsBound())
	{
		Extender = InMenuExtender.Execute(SelectedPaths);
	}

	if (FSlateApplication::Get().HasFocusedDescendants(PathPicker.ToSharedRef()))
	{
		PathPicker->SetPaths(SelectedPaths);
	}

	CurrentContextMenuCreateNewFolderDelegate = InOnCreateNewFolder;

	FMenuBuilder MenuBuilder(true /*bInShouldCloseWindowAfterMenuSelection*/, Commands, Extender);
	SetupContextMenuContent(MenuBuilder, SelectedPaths);

	return MenuBuilder.MakeWidget();
}

TSharedPtr<SWidget> ST1AssetDialog::OnGetAssetContextMenu(const TArray<FAssetData>& SelectedAssets)
{
	OpenedContextMenuWidget = ET1OpenedContextMenuWidget::AssetView;

	FMenuBuilder MenuBuilder(true /*bInShouldCloseWindowAfterMenuSelection*/, Commands);

	CurrentContextMenuCreateNewFolderDelegate = FT1OnCreateNewFolder::CreateSP(AssetPicker->GetAssetView().Get(), &ST1AssetView::OnCreateNewFolder);

	TArray<FString> Paths;
	SetupContextMenuContent(MenuBuilder, Paths);

	return MenuBuilder.MakeWidget();
}

void ST1AssetDialog::SetupContextMenuContent(FMenuBuilder& MenuBuilder, const TArray<FString>& SelectedPaths)
{
	FText NewFolderToolTip;

	if (SelectedPaths.Num() > 0)
	{
		if (CanExecuteCreateNewFolder())
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
		NewFolderToolTip = FText(LOCTEXT("NewFolderTooltip_InvalidAction", "Cannot create new folders when an asset is selected."));
	}

	MenuBuilder.BeginSection("AssetDialogOptions", LOCTEXT("AssetDialogMenuHeading", "Options"));

	MenuBuilder.AddMenuEntry(FT1ContentBrowserCommands::Get().CreateNewFolder, NAME_None, LOCTEXT("NewFolder", "New Folder"), NewFolderToolTip, FSlateIcon(FEditorStyle::GetStyleSetName(), "T1ContentBrowser.NewFolderIcon"));
	MenuBuilder.AddMenuEntry(FGenericCommands::Get().Rename, NAME_None, LOCTEXT("RenameFolder", "Rename"), LOCTEXT("RenameFolderTooltip", "Rename the selected folder."), FSlateIcon(FEditorStyle::GetStyleSetName(), "T1ContentBrowser.AssetActions.Rename"));
	MenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete, NAME_None, LOCTEXT("DeleteFolder", "Delete"), LOCTEXT("DeleteFolderTooltip", "Removes this folder and all assets it contains."));

	MenuBuilder.EndSection();

	MenuBuilder.BeginSection("AssetDialogExplore", LOCTEXT("AssetDialogExploreHeading", "Explore"));

	MenuBuilder.AddMenuEntry(T1ContentBrowserUtils::GetExploreFolderText(), 
							 LOCTEXT("ExploreTooltip", "Finds this folder on disk."), 
							 FSlateIcon(FEditorStyle::GetStyleSetName(), "SystemWideCommands.FindInContentBrowser"), 
							 FUIAction(FExecuteAction::CreateSP(this, &ST1AssetDialog::ExecuteExplore)));

	MenuBuilder.EndSection();
}

EActiveTimerReturnType ST1AssetDialog::SetFocusPostConstruct( double InCurrentTime, float InDeltaTime )
{
	FocusNameBox();
	return EActiveTimerReturnType::Stop;
}

void ST1AssetDialog::SetOnAssetsChosenForOpen(const FT1OnAssetsChosenForOpen& InOnAssetsChosenForOpen)
{
	OnAssetsChosenForOpen = InOnAssetsChosenForOpen;
}

void ST1AssetDialog::SetOnObjectPathChosenForSave(const FT1OnObjectPathChosenForSave& InOnObjectPathChosenForSave)
{
	OnObjectPathChosenForSave = InOnObjectPathChosenForSave;
}

void ST1AssetDialog::SetOnAssetDialogCancelled(const FT1OnAssetDialogCancelled& InOnAssetDialogCancelled)
{
	OnAssetDialogCancelled = InOnAssetDialogCancelled;
}

void ST1AssetDialog::FocusNameBox()
{
	if ( NameEditableText.IsValid() )
	{
		FSlateApplication::Get().SetKeyboardFocus(NameEditableText.ToSharedRef(), EFocusCause::SetDirectly);
	}
}

FText ST1AssetDialog::GetAssetNameText() const
{
	return FText::FromString(CurrentlyEnteredAssetName);
}

FText ST1AssetDialog::GetPathNameText() const
{
	return FText::FromString(CurrentlySelectedPath);
}

void ST1AssetDialog::OnAssetNameTextCommited(const FText& InText, ETextCommit::Type InCommitType)
{
	SetCurrentlyEnteredAssetName(InText.ToString());

	if ( InCommitType == ETextCommit::OnEnter )
	{
		CommitObjectPathForSave();
	}
}

EVisibility ST1AssetDialog::GetNameErrorLabelVisibility() const
{
	return GetNameErrorLabelText().IsEmpty() ? EVisibility::Hidden : EVisibility::Visible;
}

FText ST1AssetDialog::GetNameErrorLabelText() const
{
	if (!bLastInputValidityCheckSuccessful)
	{
		return LastInputValidityErrorText;
	}

	return FText::GetEmpty();
}

void ST1AssetDialog::HandlePathSelected(const FString& NewPath)
{
	FARFilter NewFilter;

	NewFilter.ClassNames.Append(AssetClassNames);
	NewFilter.PackagePaths.Add(FName(*NewPath));

	SetCurrentlySelectedPath(NewPath);

	SetFilterDelegate.ExecuteIfBound(NewFilter);
}

void ST1AssetDialog::HandleAssetViewFolderEntered(const FString& NewPath)
{
	SetCurrentlySelectedPath(NewPath);

	TArray<FString> NewPaths;
	NewPaths.Add(NewPath);
	SetPathsDelegate.Execute(NewPaths);
}

bool ST1AssetDialog::IsConfirmButtonEnabled() const
{
	if ( DialogType == ET1AssetDialogType::Open )
	{
		return CurrentlySelectedAssets.Num() > 0;
	}
	else if ( DialogType == ET1AssetDialogType::Save )
	{
		return bLastInputValidityCheckSuccessful;
	}
	else
	{
		ensureMsgf(0, TEXT("AssetDialog type %d is not supported."), DialogType);
	}

	return false;
}

FReply ST1AssetDialog::OnConfirmClicked()
{
	if ( DialogType == ET1AssetDialogType::Open )
	{
		TArray<FAssetData> SelectedAssets = GetCurrentSelectionDelegate.Execute();
		if (SelectedAssets.Num() > 0)
		{
			ChooseAssetsForOpen(SelectedAssets);
		}
	}
	else if ( DialogType == ET1AssetDialogType::Save )
	{
		// @todo save asset validation (e.g. "asset already exists" check)
		CommitObjectPathForSave();
	}
	else
	{
		ensureMsgf(0, TEXT("AssetDialog type %d is not supported."), DialogType);
	}

	return FReply::Handled();
}

FReply ST1AssetDialog::OnCancelClicked()
{
	CloseDialog();

	return FReply::Handled();
}

void ST1AssetDialog::OnAssetSelected(const FAssetData& AssetData)
{
	CurrentlySelectedAssets = GetCurrentSelectionDelegate.Execute();
	
	if ( AssetData.IsValid() )
	{
		SetCurrentlySelectedPath(AssetData.PackagePath.ToString());
		SetCurrentlyEnteredAssetName(AssetData.AssetName.ToString());
	}
}

void ST1AssetDialog::OnAssetsActivated(const TArray<FAssetData>& SelectedAssets, EAssetTypeActivationMethod::Type ActivationType)
{
	const bool bCorrectActivationMethod = (ActivationType == EAssetTypeActivationMethod::DoubleClicked || ActivationType == EAssetTypeActivationMethod::Opened);
	if (SelectedAssets.Num() > 0 && bCorrectActivationMethod)
	{
		if ( DialogType == ET1AssetDialogType::Open )
		{
			ChooseAssetsForOpen(SelectedAssets);
		}
		else if ( DialogType == ET1AssetDialogType::Save )
		{
			const FAssetData& AssetData = SelectedAssets[0];
			SetCurrentlySelectedPath(AssetData.PackagePath.ToString());
			SetCurrentlyEnteredAssetName(AssetData.AssetName.ToString());
			CommitObjectPathForSave();
		}
		else
		{
			ensureMsgf(0, TEXT("AssetDialog type %d is not supported."), DialogType);
		}
	}
}

void ST1AssetDialog::CloseDialog()
{
	FWidgetPath WidgetPath;
	TSharedPtr<SWindow> ContainingWindow = FSlateApplication::Get().FindWidgetWindow(AsShared(), WidgetPath);

	if (ContainingWindow.IsValid())
	{
		ContainingWindow->RequestDestroyWindow();
	}
}

void ST1AssetDialog::SetCurrentlySelectedPath(const FString& NewPath)
{
	CurrentlySelectedPath = NewPath;
	UpdateInputValidity();
}

void ST1AssetDialog::SetCurrentlyEnteredAssetName(const FString& NewName)
{
	CurrentlyEnteredAssetName = NewName;
	UpdateInputValidity();
}

void ST1AssetDialog::UpdateInputValidity()
{
	bLastInputValidityCheckSuccessful = true;

	if ( CurrentlyEnteredAssetName.IsEmpty() )
	{
		// No error text for an empty name. Just fail validity.
		LastInputValidityErrorText = FText::GetEmpty();
		bLastInputValidityCheckSuccessful = false;
	}

	if (bLastInputValidityCheckSuccessful)
	{
		if ( CurrentlySelectedPath.IsEmpty() )
		{
			LastInputValidityErrorText = LOCTEXT("AssetDialog_NoPathSelected", "You must select a path.");
			bLastInputValidityCheckSuccessful = false;
		}
	}

	if ( DialogType == ET1AssetDialogType::Save )
	{
		if ( bLastInputValidityCheckSuccessful )
		{
			const FString ObjectPath = GetObjectPathForSave();
			FText ErrorMessage;
			const bool bAllowExistingAsset = (ExistingAssetPolicy == ET1SaveAssetDialogExistingAssetPolicy::AllowButWarn);
			if ( !T1ContentBrowserUtils::IsValidObjectPathForCreate(ObjectPath, ErrorMessage, bAllowExistingAsset) )
			{
				LastInputValidityErrorText = ErrorMessage;
				bLastInputValidityCheckSuccessful = false;
			}
		}
	}
}

void ST1AssetDialog::ChooseAssetsForOpen(const TArray<FAssetData>& SelectedAssets)
{
	if ( ensure(DialogType == ET1AssetDialogType::Open) )
	{
		if (SelectedAssets.Num() > 0)
		{
			bValidAssetsChosen = true;
			OnAssetsChosenForOpen.ExecuteIfBound(SelectedAssets);
			CloseDialog();
		}
	}
}

FString ST1AssetDialog::GetObjectPathForSave() const
{
	return CurrentlySelectedPath / CurrentlyEnteredAssetName + TEXT(".") + CurrentlyEnteredAssetName;
}

void ST1AssetDialog::CommitObjectPathForSave()
{
	if ( ensure(DialogType == ET1AssetDialogType::Save) )
	{
		if ( bLastInputValidityCheckSuccessful )
		{
			const FString ObjectPath = GetObjectPathForSave();

			bool bProceedWithSave = true;

			// If we were asked to warn on existing assets, do it now
			if ( ExistingAssetPolicy == ET1SaveAssetDialogExistingAssetPolicy::AllowButWarn )
			{
				FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
				FAssetData ExistingAsset = AssetRegistryModule.Get().GetAssetByObjectPath(FName(*ObjectPath));
				if ( ExistingAsset.IsValid() && AssetClassNames.Contains(ExistingAsset.AssetClass) )
				{
					EAppReturnType::Type ShouldReplace = FMessageDialog::Open( EAppMsgType::YesNo, FText::Format(LOCTEXT("ReplaceAssetMessage", "{ExistingAsset} already exists. Do you want to replace it?"), FText::FromString(CurrentlyEnteredAssetName)) );
					bProceedWithSave = (ShouldReplace == EAppReturnType::Yes);
				}
			}

			if ( bProceedWithSave )
			{
				bValidAssetsChosen = true;
				OnObjectPathChosenForSave.ExecuteIfBound(ObjectPath);
				CloseDialog();
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
