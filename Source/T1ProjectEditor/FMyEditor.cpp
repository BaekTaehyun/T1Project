// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "FMyEditor.h"
#include "MyUI.h"
#include "FMyExtensionStyle.h"
#include "SMyViewport.h"

#include "PropertyEditorModule.h"
#include "AdvancedPreviewSceneModule.h"

const FName FMyEditor::MyEditorAppIdentifier = FName(TEXT("MyEditorApp"));
const FName FMyEditor::ViewportTabId = FName(TEXT("My Viewport"));
const FName FMyEditor::DetailTabId = FName(TEXT("My Detail"));
const FName FMyEditor::PreviewSceneSettingsTabId = FName(TEXT("My PreviewScene Setting"));

#define LOCTEXT_NAMESPACE "MyEditor"

void FMyEditor::InitFMyEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UMyUI * InUMyUI)
{
	// �����ϱ� ���� ���� MyUI ��ü�� ����
	InUMyUI->SetFlags(RF_Transactional); // Undo, Redo ����.
	MyObj = InUMyUI;

	// ������Ƽ������ ����� �����ͼ� ������ �並 ����.
	const bool bIsUpdatable = false;
	const bool bAllowFavorites = true;
	const bool bIsLockable = false;

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	const FDetailsViewArgs DetailsViewArgs(bIsUpdatable, bIsLockable, true, FDetailsViewArgs::ObjectsUseNameArea, false);
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	// ����Ʈ ����.
	Viewport = SNew(SMyViewport)
		.ParentMyEditor(SharedThis(this))
		.ObjectToEdit(MyObj);

	// ������ �� ���� ����.
	FAdvancedPreviewSceneModule& AdvancedPreviewSceneModule = FModuleManager::LoadModuleChecked<FAdvancedPreviewSceneModule>("AdvancedPreviewScene");
	AdvancedPreviewSettingsWidget = AdvancedPreviewSceneModule.CreateAdvancedPreviewSceneSettingsWidget(Viewport->GetPreviewScene());

	// ���ٰ� �� �⺻ ���̾ƿ� ����.
	const TSharedRef<FTabManager::FLayout> EditorDefaultLayout = FTabManager::NewLayout("MyEditor_Layout_v2")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.6)
					->AddTab(ViewportTabId, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.4f)
						->AddTab(DetailTabId, ETabState::OpenedTab)
					)
					->Split
					(
						FTabManager::NewStack()
						->AddTab(PreviewSceneSettingsTabId, ETabState::OpenedTab)
					)
				)
			)
		);

	// ������ �ʱ�ȭ
	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, MyEditorAppIdentifier, EditorDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, InUMyUI);

	// ������ �信 IGC ��ü�� ����.
	if (DetailsView.IsValid())
	{
		DetailsView->SetObject(MyObj);
	}
}

FMyEditor::~FMyEditor()
{
	DetailsView.Reset();
	AdvancedPreviewSettingsWidget.Reset();
}

TSharedRef<SDockTab> FMyEditor::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == ViewportTabId);
	return SNew(SDockTab)
		[
			Viewport.ToSharedRef()
		];
}

TSharedRef<SDockTab> FMyEditor::SpawnTab_Detail(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == DetailTabId);
	return SNew(SDockTab)
		[
			DetailsView.ToSharedRef()
		];
}

TSharedRef<SDockTab> FMyEditor::SpawnTab_PreviewSceneSettings(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == PreviewSceneSettingsTabId);
	return SNew(SDockTab)
		[
			AdvancedPreviewSettingsWidget.ToSharedRef()
		];
}

void FMyEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager)
{
	WorkspaceMenuCategory = TabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_MyAssetEditor", "My Asset Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(TabManager);

	TabManager->RegisterTabSpawner(ViewportTabId, FOnSpawnTab::CreateSP(this, &FMyEditor::SpawnTab_Viewport))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FMyExtensionStyle::GetStyleSetName(), "MyExtensions.Command1"));

	TabManager->RegisterTabSpawner(DetailTabId, FOnSpawnTab::CreateSP(this, &FMyEditor::SpawnTab_Detail))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FMyExtensionStyle::GetStyleSetName(), "MyExtensions.Command2"));

	TabManager->RegisterTabSpawner(PreviewSceneSettingsTabId, FOnSpawnTab::CreateSP(this, &FMyEditor::SpawnTab_PreviewSceneSettings))
		.SetDisplayName(LOCTEXT("PreviewSceneTab", "Preview Scene Settings"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FMyExtensionStyle::GetStyleSetName(), "MyExtensions.Command3"));
}

void FMyEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(TabManager);

	TabManager->UnregisterTabSpawner(ViewportTabId);
	TabManager->UnregisterTabSpawner(DetailTabId);
	TabManager->UnregisterTabSpawner(PreviewSceneSettingsTabId);
}

FName FMyEditor::GetToolkitFName() const
{
	return FName("MY Editor");
}

FText FMyEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "My Editor");
}

FString FMyEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "My ").ToString();
}

FLinearColor FMyEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.0f, 0.0f, 0.2f, 0.5f);
}

#undef LOCTEXT_NAMESPACE 