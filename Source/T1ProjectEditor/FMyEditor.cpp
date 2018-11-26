// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "FMyEditor.h"
#include "DataActorComponent.h"
#include "FMyExtensionStyle.h"

#include "PropertyEditorModule.h"
#include "T1ContentBrowserEditor/Public/T1ContentBrowserEditor.h"
#include "T1ContentBrowserEditor/Public/IT1ContentBrowserSingleton.h"
#include "Dom/JsonObject.h"
#include "Widgets/Text/STextBlock.h"
#include "Misc/FileHelper.h"
#include "Modules/ModuleManager.h"
#include "Serialization/JsonReader.h"
#include "Policies/PrettyJsonPrintPolicy.h"
#include "Serialization/JsonSerializer.h"
#include "Fonts/FontMeasure.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SScrollBar.h"
#include "Framework/Layout/Overscroll.h"
#include "Widgets/Layout/SScrollBox.h"
#include "EditorStyleSet.h"
#include "Editor.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Views/SListView.h"
#include "IDocumentation.h"
#include "Widgets/SToolTip.h"
#include "SMyRowEditor.h"

#pragma warning(disable:4828)

const FName FMyEditor::MyEditorAppIdentifier = FName(TEXT("MyEditorApp"));
const FName FMyEditor::DataActorComponentTabId = FName(TEXT("My DataActorComponent"));
const FName FMyEditor::DataTableSelectTabId = FName(TEXT("My DataTableSelect"));
const FName FMyEditor::DataTableTabId = FName(TEXT("My DataTable"));
const FName FMyEditor::RowNameColumnId("RowName");
const FName FMyEditor::RowEditorTabId("MY RowEditor");

#define LOCTEXT_NAMESPACE "MyEditor"

class SDataTableListViewRow : public SMultiColumnTableRow<FDataTableEditorRowListViewDataPtr>
{
public:
	SLATE_BEGIN_ARGS(SDataTableListViewRow) {}
	/** The widget that owns the tree.  We'll only keep a weak reference to it. */
	SLATE_ARGUMENT(TSharedPtr<FMyEditor>, DataTableEditor)
		/** The list item for this row */
		SLATE_ARGUMENT(FDataTableEditorRowListViewDataPtr, Item)
		SLATE_END_ARGS()

		/** Construct function for this widget */
		void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		DataTableEditor = InArgs._DataTableEditor;
		Item = InArgs._Item;
		SMultiColumnTableRow<FDataTableEditorRowListViewDataPtr>::Construct(
			FSuperRowType::FArguments()
			.Style(FEditorStyle::Get(), "DataTableEditor.CellListViewRow"),
			InOwnerTableView
		);
	}

	/** Overridden from SMultiColumnTableRow.  Generates a widget for this column of the list view. */
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override
	{
		TSharedPtr<FMyEditor> DataTableEditorPtr = DataTableEditor.Pin();
		return (DataTableEditorPtr.IsValid())
			? DataTableEditorPtr->MakeCellWidget(Item, IndexInList, ColumnName)
			: SNullWidget::NullWidget;
	}

private:
	/** Weak reference to the data table editor that owns our list */
	TWeakPtr<FMyEditor> DataTableEditor;
	/** The item associated with this row of data */
	FDataTableEditorRowListViewDataPtr Item;
};

TSharedRef<SWidget> FMyEditor::MakeCellWidget(FDataTableEditorRowListViewDataPtr InRowDataPtr, const int32 InRowIndex, const FName& InColumnId)
{
	int32 ColumnIndex = 0;
	for (; ColumnIndex < AvailableColumns.Num(); ++ColumnIndex)
	{
		const FDataTableEditorColumnHeaderDataPtr& ColumnData = AvailableColumns[ColumnIndex];
		if (ColumnData->ColumnId == InColumnId)
		{
			break;
		}
	}

	// Valid column ID?
	if (AvailableColumns.IsValidIndex(ColumnIndex) && InRowDataPtr->CellData.IsValidIndex(ColumnIndex))
	{
		return SNew(SBox)
			.Padding(FMargin(4, 2, 4, 2))
			[
				SNew(STextBlock)
				.TextStyle(FEditorStyle::Get(), "DataTableEditor.CellText")
			.ColorAndOpacity(this, &FMyEditor::GetRowTextColor, InRowDataPtr->RowId)
			.Text(this, &FMyEditor::GetCellText, InRowDataPtr, ColumnIndex)
			.HighlightText(this, &FMyEditor::GetFilterText)
			.ToolTipText(this, &FMyEditor::GetCellToolTipText, InRowDataPtr, ColumnIndex)
			];
	}

	return SNullWidget::NullWidget;
}

FText FMyEditor::GetCellText(FDataTableEditorRowListViewDataPtr InRowDataPointer, int32 ColumnIndex) const
{
	if (InRowDataPointer.IsValid() && ColumnIndex < InRowDataPointer->CellData.Num())
	{
		return InRowDataPointer->CellData[ColumnIndex];
	}

	return FText();
}

FText FMyEditor::GetCellToolTipText(FDataTableEditorRowListViewDataPtr InRowDataPointer, int32 ColumnIndex) const
{
	FText TooltipText;

	if (ColumnIndex < AvailableColumns.Num())
	{
		TooltipText = AvailableColumns[ColumnIndex]->DisplayName;
	}

	if (InRowDataPointer.IsValid() && ColumnIndex < InRowDataPointer->CellData.Num())
	{
		TooltipText = FText::Format(LOCTEXT("ColumnRowNameFmt", "{0}: {1}"), TooltipText, InRowDataPointer->CellData[ColumnIndex]);
	}

	return TooltipText;
}

void FMyEditor::InitFMyEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class UDataActorComponent* InComponent)
{	
	// 툴바가 들어갈 기본 레이아웃 설계.
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
					->AddTab(DataActorComponentTabId, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.4f)
					->AddTab(DataTableSelectTabId, ETabState::OpenedTab)
				)
				->Split
				(

					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.5f)
						->AddTab(DataTableTabId, ETabState::OpenedTab)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.5f)
						->AddTab(RowEditorTabId, ETabState::OpenedTab)
					)
				)
			)
		);

	// 편집하기 위해 들어온 MyUI 객체의 설정
	InComponent->SetFlags(RF_Transactional); // Undo, Redo 지원.
	MyObj = InComponent;
	//Test = InComponent->CharacterTable;

	// 프로퍼티에디터 모듈을 가져와서 디테일 뷰를 생성.
	const bool bIsUpdatable = false;
	const bool bAllowFavorites = true;
	const bool bIsLockable = false;

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	const FDetailsViewArgs DetailsViewArgs(bIsUpdatable, bIsLockable, true, FDetailsViewArgs::ObjectsUseNameArea, false);
	DataActorComponentView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	//IContentBrowserSingleton& ContentBrowserSingleton = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get();;
	IT1ContentBrowserSingleton& ContentBrowserSingleton = FModuleManager::Get().LoadModuleChecked<T1ContentBrowserEditor>("T1ContentBrowserEditor").Get();;
	FT1ContentBrowserConfig Config;
	Config.bCanShowClasses = false;
	Config.bCanShowRealTimeThumbnails = false;
	Config.InitialAssetViewType = ET1AssetViewType::Tile;
	Config.bCanShowDevelopersFolder = false;
	Config.bCanShowFolders = false;
	Config.bUseSourcesView = true;
	Config.bExpandSourcesView = true;
	Config.ThumbnailLabel = EThumbnailLabel::NoLabel;
	Config.ThumbnailScale = 0.4f;
	Config.bCanShowFilters = true;
	Config.bUsePathPicker = true;
	Config.bShowAssetPathTree = true;
	Config.bAlwaysShowCollections = false;
	Config.bShowBottomToolbar = true;
	Config.bCanShowLockButton = false;

	DataTableSelectView = ContentBrowserSingleton.CreateContentBrowser("DataTableContentBrowser", nullptr, &Config);
	//DataTableSelectView->SetOnMouseDoubleClick()

	DataTableTabWidget = CreateContentBox();
	RowEditorTabWidget = CreateRowEditorBox();

	// 에디터 초기화
	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, MyEditorAppIdentifier, EditorDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, InComponent);

	// 디테일 뷰에 객체를 지정.
	if(DataActorComponentView.IsValid())
	{
		DataActorComponentView->SetObject(MyObj);
	}
}

void FMyEditor::PostUndo(bool bSuccess)
{
	HandleUndoRedo();
}

void FMyEditor::PostRedo(bool bSuccess)
{
	HandleUndoRedo();
}

void FMyEditor::HandleUndoRedo()
{
	const UDataTable* Table = GetDataTable();
	if (Table)
	{
		HandlePostChange();
		CallbackOnDataTableUndoRedo.ExecuteIfBound();
	}
}

void FMyEditor::PreChange(const class UUserDefinedStruct* Struct, FStructureEditorUtils::EStructureEditorChangeInfo Info)
{
}

void FMyEditor::PostChange(const class UUserDefinedStruct* Struct, FStructureEditorUtils::EStructureEditorChangeInfo Info)
{
	const UDataTable* Table = GetDataTable();
	if (Struct && Table && (Table->RowStruct == Struct))
	{
		HandlePostChange();
	}
}

void FMyEditor::SelectionChange(const UDataTable* Changed, FName RowName)
{
	const UDataTable* Table = GetDataTable();
	if (Changed == Table)
	{
		const bool bSelectionChanged = HighlightedRowName != RowName;
		SetHighlightedRow(RowName);

		if (bSelectionChanged)
		{
			CallbackOnRowHighlighted.ExecuteIfBound(HighlightedRowName);
		}
	}
}

void FMyEditor::PreChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info)
{
}

void FMyEditor::PostChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info)
{
	const UDataTable* Table = GetDataTable();
	if (Changed == Table)
	{
		HandlePostChange();
	}
}

TSharedRef<SWidget> FMyEditor::CreateRowEditorBox()
{
	//UDataTable* Table = Cast<UDataTable>(GetDataTable());//Cast<UDataTable>(GetEditingObject());

	UDataTable* Table = GetDataTable();
	//if (!Table)

	// Support undo/redo
	if (Table)
	{
		Table->SetFlags(RF_Transactional);
	}

	auto RowEditor = SNew(SMyRowEditor, Table);
	RowEditor->RowSelectedCallback.BindSP(this, &FMyEditor::SetHighlightedRow);
	CallbackOnRowHighlighted.BindSP(RowEditor, &SMyRowEditor::SelectRow);
	CallbackOnDataTableUndoRedo.BindSP(RowEditor, &SMyRowEditor::HandleUndoRedo);
	return RowEditor;
}

FMyEditor::~FMyEditor()
{
	DataActorComponentView.Reset();
	DataTableSelectView.Reset();
	DataTableTabWidget.Reset();
	RowEditorTabWidget.Reset();
}

TSharedRef<SDockTab> FMyEditor::SpawnTab_DataActorComponent(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == DataActorComponentTabId);
	return SNew(SDockTab)
		[
			DataActorComponentView.ToSharedRef()
		];
}

TSharedRef<SDockTab> FMyEditor::SpawnTab_DataTableSelect(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == DataTableSelectTabId);
	return SNew(SDockTab)
		[
			DataTableSelectView.ToSharedRef()
		];
}

TSharedRef<SDockTab> FMyEditor::SpawnTab_DataTable(const FSpawnTabArgs& Args)
{
	/*check(Args.GetTabId() == DataTableTabId);
	return SNew(SDockTab)
		[
			DataTableView.ToSharedRef()
		];*/
	check(Args.GetTabId().TabType == DataTableTabId);

	UDataTable* Table = Cast<UDataTable>(GetEditingObject());

	// Support undo/redo
	if (Table)
	{
		Table->SetFlags(RF_Transactional);
	}

	LoadLayoutData();

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("DataTableEditor.Tabs.Properties"))
		.Label(LOCTEXT("DataTableTitle", "Data Table"))
		.TabColorScale(GetTabColorScale())
		[
			SNew(SBorder)
			.Padding(2)
		.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			DataTableTabWidget.ToSharedRef()
		]
		];
}

TSharedRef<SDockTab> FMyEditor::SpawnTab_RowEditor(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId().TabType == RowEditorTabId);

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("DataTableEditor.Tabs.Properties"))
		.Label(LOCTEXT("RowEditorTitle", "Row Editor"))
		.TabColorScale(GetTabColorScale())
		[
			SNew(SBorder)
			.Padding(2)
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Fill)
		.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			RowEditorTabWidget.ToSharedRef()
		]
		];
}

void FMyEditor::LoadLayoutData()
{
	LayoutData.Reset();

	const UDataTable* Table = GetDataTable();
	if (!Table)
	{
		return;
	}

	const FString LayoutDataFilename = FPaths::ProjectSavedDir() / TEXT("AssetData") / TEXT("MyEditor_Layout_v2") / Table->GetName() + TEXT(".json");

	FString JsonText;
	if (FFileHelper::LoadFileToString(JsonText, *LayoutDataFilename))
	{
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonText);
		FJsonSerializer::Deserialize(JsonReader, LayoutData);
	}
}

UDataTable* FMyEditor::GetDataTable()
{
	return MyObj->GetDataTable();
}

const UDataTable* FMyEditor::GetDataTable() const
{
	//FString CharInfoDataPath = TEXT("/Game/Data/CCharInfoData.CCharInfoData");
	//FString CharInfoDataPath = TEXT("/Content/Data/CCharInfoData.json");
	//static ConstructorHelpers::FObjectFinder<UDataTable> DT_CHARACTER(*CharInfoDataPath);
	return MyObj->GetDataTable();
	/*T1CHECK(DT_CHARACTER.Succeeded());
	CharacterTable = DT_CHARACTER.Object;
	T1CHECK(CharacterTable->RowMap.Num() > 0);*/

	//return MyObj->CharacterTable;
	//return Cast<const UDataTable>(GetEditingObject());
}

void FMyEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager)
{
	WorkspaceMenuCategory = TabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_MyAssetEditor", "My Asset Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(TabManager);

	TabManager->RegisterTabSpawner(DataActorComponentTabId, FOnSpawnTab::CreateSP(this, &FMyEditor::SpawnTab_DataActorComponent))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FMyExtensionStyle::GetStyleSetName(), "MyExtensions.DataTool"));

	TabManager->RegisterTabSpawner(DataTableSelectTabId, FOnSpawnTab::CreateSP(this, &FMyEditor::SpawnTab_DataTableSelect))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FMyExtensionStyle::GetStyleSetName(), "MyExtensions.DataTool"));

	TabManager->RegisterTabSpawner(DataTableTabId, FOnSpawnTab::CreateSP(this, &FMyEditor::SpawnTab_DataTable))		
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FMyExtensionStyle::GetStyleSetName(), "MyExtensions.DataTool"));	

	TabManager->RegisterTabSpawner(RowEditorTabId, FOnSpawnTab::CreateSP(this, &FMyEditor::SpawnTab_RowEditor))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FMyExtensionStyle::GetStyleSetName(), "MyExtensions.DataTool"));		
}

void FMyEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(TabManager);

	TabManager->UnregisterTabSpawner(DataActorComponentTabId);
	TabManager->UnregisterTabSpawner(DataTableSelectTabId);
	TabManager->UnregisterTabSpawner(DataTableTabId);
	TabManager->UnregisterTabSpawner(RowEditorTabId);

	DataActorComponentView.Reset();
	DataTableSelectView.Reset();
	DataTableTabWidget.Reset();
	RowEditorTabWidget.Reset();
}

//TSharedRef<SWidget> FMyEditor::CreateRowEditorBox()
//{
//	UDataTable* Table = Cast<UDataTable>(GetEditingObject());
//
//	// Support undo/redo
//	if (Table)
//	{
//		Table->SetFlags(RF_Transactional);
//	}
//
//	auto RowEditor = SNew(SRowEditor, Table);
//	RowEditor->RowSelectedCallback.BindSP(this, &FDataTableEditor::SetHighlightedRow);
//	CallbackOnRowHighlighted.BindSP(RowEditor, &SRowEditor::SelectRow);
//	CallbackOnDataTableUndoRedo.BindSP(RowEditor, &SRowEditor::HandleUndoRedo);
//	return RowEditor;
//}

TSharedRef<SVerticalBox> FMyEditor::CreateContentBox()
{
	TSharedRef<SScrollBar> HorizontalScrollBar = SNew(SScrollBar)
		.Orientation(Orient_Horizontal)
		.Thickness(FVector2D(8.0f, 8.0f));

	TSharedRef<SScrollBar> VerticalScrollBar = SNew(SScrollBar)
		.Orientation(Orient_Vertical)
		.Thickness(FVector2D(8.0f, 8.0f));

	TSharedRef<SHeaderRow> RowNamesHeaderRow = SNew(SHeaderRow);
	RowNamesHeaderRow->AddColumn(
		SHeaderRow::Column(RowNameColumnId)
		.DefaultLabel(FText::GetEmpty())
	);

	ColumnNamesHeaderRow = SNew(SHeaderRow);

	RowNamesListView = SNew(SListView<FDataTableEditorRowListViewDataPtr>)
		.ListItemsSource(&VisibleRows)
		.HeaderRow(RowNamesHeaderRow)
		.OnGenerateRow(this, &FMyEditor::MakeRowNameWidget)
		.OnListViewScrolled(this, &FMyEditor::OnRowNamesListViewScrolled)
		.OnSelectionChanged(this, &FMyEditor::OnRowSelectionChanged)
		.ScrollbarVisibility(EVisibility::Collapsed)
		.ConsumeMouseWheel(EConsumeMouseWheel::Always)
		.SelectionMode(ESelectionMode::Single)
		.AllowOverscroll(EAllowOverscroll::No);

	CellsListView = SNew(SListView<FDataTableEditorRowListViewDataPtr>)
		.ListItemsSource(&VisibleRows)
		.HeaderRow(ColumnNamesHeaderRow)
		.OnGenerateRow(this, &FMyEditor::MakeRowWidget)
		.OnListViewScrolled(this, &FMyEditor::OnCellsListViewScrolled)
		.OnSelectionChanged(this, &FMyEditor::OnRowSelectionChanged)
		.ExternalScrollbar(VerticalScrollBar)
		.ConsumeMouseWheel(EConsumeMouseWheel::Always)
		.SelectionMode(ESelectionMode::Single)
		.AllowOverscroll(EAllowOverscroll::No);

	RefreshCachedDataTable();

	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SSearchBox)
			.InitialText(this, &FMyEditor::GetFilterText)
		.OnTextChanged(this, &FMyEditor::OnFilterTextChanged)
		]
	+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBox)
			.WidthOverride(this, &FMyEditor::GetRowNameColumnWidth)
		[
			RowNamesListView.ToSharedRef()
		]
		]
	+ SHorizontalBox::Slot()
		[
			SNew(SScrollBox)
			.Orientation(Orient_Horizontal)
		.ExternalScrollbar(HorizontalScrollBar)
		+ SScrollBox::Slot()
		[
			CellsListView.ToSharedRef()
		]
		]
	+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			VerticalScrollBar
		]
		]
	+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBox)
			.WidthOverride(this, &FMyEditor::GetRowNameColumnWidth)
		[
			SNullWidget::NullWidget
		]
		]
	+ SHorizontalBox::Slot()
		[
			HorizontalScrollBar
		]
		];
}

void FMyEditor::OnRowNamesListViewScrolled(double InScrollOffset)
{
	// Synchronize the list views
	CellsListView->SetScrollOffset(InScrollOffset);
}

void FMyEditor::OnCellsListViewScrolled(double InScrollOffset)
{
	// Synchronize the list views
	RowNamesListView->SetScrollOffset(InScrollOffset);
}

void FMyEditor::HandlePostChange()
{
	// We need to cache and restore the selection here as RefreshCachedDataTable will re-create the list view items
	const FName CachedSelection = HighlightedRowName;
	HighlightedRowName = NAME_None;
	RefreshCachedDataTable(CachedSelection, true/*bUpdateEvenIfValid*/);
}

void FMyEditor::SetHighlightedRow(FName Name)
{
	if (Name == HighlightedRowName)
	{
		return;
	}

	if (Name.IsNone())
	{
		HighlightedRowName = NAME_None;

		// Synchronize the list views
		RowNamesListView->ClearSelection();
		CellsListView->ClearSelection();
	}
	else
	{
		HighlightedRowName = Name;

		FDataTableEditorRowListViewDataPtr* NewSelectionPtr = VisibleRows.FindByPredicate([&Name](const FDataTableEditorRowListViewDataPtr& RowData) -> bool
		{
			return RowData->RowId == Name;
		});

		// Synchronize the list views
		if (NewSelectionPtr)
		{
			RowNamesListView->SetSelection(*NewSelectionPtr);
			CellsListView->SetSelection(*NewSelectionPtr);

			CellsListView->RequestScrollIntoView(*NewSelectionPtr);
		}
		else
		{
			RowNamesListView->ClearSelection();
			CellsListView->ClearSelection();
		}
	}
}

void FMyEditor::OnRowSelectionChanged(FDataTableEditorRowListViewDataPtr InNewSelection, ESelectInfo::Type InSelectInfo)
{
	const bool bSelectionChanged = !InNewSelection.IsValid() || InNewSelection->RowId != HighlightedRowName;
	const FName NewRowName = (InNewSelection.IsValid()) ? InNewSelection->RowId : NAME_None;

	SetHighlightedRow(NewRowName);

	if (bSelectionChanged)
	{
		CallbackOnRowHighlighted.ExecuteIfBound(HighlightedRowName);
	}
}

FText FMyEditor::GetFilterText() const
{
	return ActiveFilterText;
}

void FMyEditor::OnFilterTextChanged(const FText& InFilterText)
{
	ActiveFilterText = InFilterText;
	UpdateVisibleRows();
}

void FMyEditor::RefreshCachedDataTable(const FName InCachedSelection, const bool bUpdateEvenIfValid)
{
	const UDataTable* Table = GetDataTable();
	TArray<FDataTableEditorColumnHeaderDataPtr> PreviousColumns = AvailableColumns;

	FDataTableEditorUtils::CacheDataTableForEditing(Table, AvailableColumns, AvailableRows);

	// Update the desired width of the row names column
	// This prevents it growing or shrinking as you scroll the list view
	{
		TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
		const FTextBlockStyle& CellTextStyle = FEditorStyle::GetWidgetStyle<FTextBlockStyle>("DataTableEditor.CellText");
		static const float CellPadding = 10.0f;

		RowNameColumnWidth = 10.0f;
		for (const FDataTableEditorRowListViewDataPtr& RowData : AvailableRows)
		{
			const float RowNameWidth = FontMeasure->Measure(RowData->DisplayName, CellTextStyle.Font).X + CellPadding;
			RowNameColumnWidth = FMath::Max(RowNameColumnWidth, RowNameWidth);
		}
	}

	// Setup the default auto-sized columns
	ColumnWidths.SetNum(AvailableColumns.Num());
	for (int32 ColumnIndex = 0; ColumnIndex < AvailableColumns.Num(); ++ColumnIndex)
	{
		const FDataTableEditorColumnHeaderDataPtr& ColumnData = AvailableColumns[ColumnIndex];
		FColumnWidth& ColumnWidth = ColumnWidths[ColumnIndex];
		ColumnWidth.CurrentWidth = FMath::Clamp(ColumnData->DesiredColumnWidth, 10.0f, 400.0f); // Clamp auto-sized columns to a reasonable limit
	}

	// Load the persistent column widths from the layout data
	{
		const TSharedPtr<FJsonObject>* LayoutColumnWidths = nullptr;
		if (LayoutData.IsValid() && LayoutData->TryGetObjectField(TEXT("ColumnWidths"), LayoutColumnWidths))
		{
			for (int32 ColumnIndex = 0; ColumnIndex < AvailableColumns.Num(); ++ColumnIndex)
			{
				const FDataTableEditorColumnHeaderDataPtr& ColumnData = AvailableColumns[ColumnIndex];

				double LayoutColumnWidth = 0.0f;
				if ((*LayoutColumnWidths)->TryGetNumberField(ColumnData->ColumnId.ToString(), LayoutColumnWidth))
				{
					FColumnWidth& ColumnWidth = ColumnWidths[ColumnIndex];
					ColumnWidth.bIsAutoSized = false;
					ColumnWidth.CurrentWidth = static_cast<float>(LayoutColumnWidth);
				}
			}
		}
	}

	if (PreviousColumns != AvailableColumns)
	{
		ColumnNamesHeaderRow->ClearColumns();
		for (int32 ColumnIndex = 0; ColumnIndex < AvailableColumns.Num(); ++ColumnIndex)
		{
			const FDataTableEditorColumnHeaderDataPtr& ColumnData = AvailableColumns[ColumnIndex];

			ColumnNamesHeaderRow->AddColumn(
				SHeaderRow::Column(ColumnData->ColumnId)
				.DefaultLabel(ColumnData->DisplayName)
				.ManualWidth(TAttribute<float>::Create(TAttribute<float>::FGetter::CreateSP(this, &FMyEditor::GetColumnWidth, ColumnIndex)))
				.OnWidthChanged(this, &FMyEditor::OnColumnResized, ColumnIndex)
				[
					SNew(SBox)
					.Padding(FMargin(0, 4, 0, 4))
				.VAlign(VAlign_Fill)
				.ToolTip(IDocumentation::Get()->CreateToolTip(FDataTableEditorUtils::GetRowTypeInfoTooltipText(ColumnData), nullptr, *FDataTableEditorUtils::VariableTypesTooltipDocLink, FDataTableEditorUtils::GetRowTypeTooltipDocExcerptName(ColumnData)))
				[
					SNew(STextBlock)
					.Justification(ETextJustify::Center)
				.Text(ColumnData->DisplayName)
				]
				]
			);
		}
	}

	UpdateVisibleRows(InCachedSelection, bUpdateEvenIfValid);
}

void FMyEditor::OnColumnResized(const float NewWidth, const int32 ColumnIndex)
{
	if (ColumnWidths.IsValidIndex(ColumnIndex))
	{
		FColumnWidth& ColumnWidth = ColumnWidths[ColumnIndex];
		ColumnWidth.bIsAutoSized = false;
		ColumnWidth.CurrentWidth = NewWidth;

		// Update the persistent column widths in the layout data
		{
			if (!LayoutData.IsValid())
			{
				LayoutData = MakeShareable(new FJsonObject());
			}

			TSharedPtr<FJsonObject> LayoutColumnWidths;
			if (!LayoutData->HasField(TEXT("ColumnWidths")))
			{
				LayoutColumnWidths = MakeShareable(new FJsonObject());
				LayoutData->SetObjectField(TEXT("ColumnWidths"), LayoutColumnWidths);
			}
			else
			{
				LayoutColumnWidths = LayoutData->GetObjectField(TEXT("ColumnWidths"));
			}

			const FString& ColumnName = AvailableColumns[ColumnIndex]->ColumnId.ToString();
			LayoutColumnWidths->SetNumberField(ColumnName, NewWidth);
		}
	}
}

FOptionalSize FMyEditor::GetRowNameColumnWidth() const
{
	return FOptionalSize(RowNameColumnWidth);
}

float FMyEditor::GetColumnWidth(const int32 ColumnIndex) const
{
	if (ColumnWidths.IsValidIndex(ColumnIndex))
	{
		return ColumnWidths[ColumnIndex].CurrentWidth;
	}
	return 0.0f;
}

void FMyEditor::UpdateVisibleRows(const FName InCachedSelection, const bool bUpdateEvenIfValid)
{
	if (ActiveFilterText.IsEmptyOrWhitespace())
	{
		VisibleRows = AvailableRows;
	}
	else
	{
		VisibleRows.Empty(AvailableRows.Num());

		const FString& ActiveFilterString = ActiveFilterText.ToString();
		for (const FDataTableEditorRowListViewDataPtr& RowData : AvailableRows)
		{
			bool bPassesFilter = false;

			if (RowData->DisplayName.ToString().Contains(ActiveFilterString))
			{
				bPassesFilter = true;
			}
			else
			{
				for (const FText& CellText : RowData->CellData)
				{
					if (CellText.ToString().Contains(ActiveFilterString))
					{
						bPassesFilter = true;
						break;
					}
				}
			}

			if (bPassesFilter)
			{
				VisibleRows.Add(RowData);
			}
		}
	}

	// Abort restoring the cached selection if data was changed while the user is selecting a different row
	if (RowNamesListView->GetSelectedItems() == CellsListView->GetSelectedItems())
	{
		RowNamesListView->RequestListRefresh();
		CellsListView->RequestListRefresh();

		RestoreCachedSelection(InCachedSelection, bUpdateEvenIfValid);
	}
}

void FMyEditor::RestoreCachedSelection(const FName InCachedSelection, const bool bUpdateEvenIfValid)
{
	// Validate the requested selection to see if it matches a known row
	bool bSelectedRowIsValid = false;
	if (!InCachedSelection.IsNone())
	{
		bSelectedRowIsValid = VisibleRows.ContainsByPredicate([&InCachedSelection](const FDataTableEditorRowListViewDataPtr& RowData) -> bool
		{
			return RowData->RowId == InCachedSelection;
		});
	}

	// Apply the new selection (if required)
	if (!bSelectedRowIsValid)
	{
		SetHighlightedRow((VisibleRows.Num() > 0) ? VisibleRows[0]->RowId : NAME_None);
		CallbackOnRowHighlighted.ExecuteIfBound(HighlightedRowName);
	}
	else if (bUpdateEvenIfValid)
	{
		SetHighlightedRow(InCachedSelection);
		CallbackOnRowHighlighted.ExecuteIfBound(HighlightedRowName);
	}
}

TSharedRef<ITableRow> FMyEditor::MakeRowNameWidget(FDataTableEditorRowListViewDataPtr InRowDataPtr, const TSharedRef<STableViewBase>& OwnerTable)
{
	return
		SNew(STableRow<FDataTableEditorRowListViewDataPtr>, OwnerTable)
		.Style(FEditorStyle::Get(), "DataTableEditor.NameListViewRow")
		[
			SNew(SBox)
			.Padding(FMargin(4, 2, 4, 2))
		[
			SNew(SBox)
			.HeightOverride(InRowDataPtr->DesiredRowHeight)
		[
			SNew(STextBlock)
			.ColorAndOpacity(this, &FMyEditor::GetRowTextColor, InRowDataPtr->RowId)
		.Text(InRowDataPtr->DisplayName)
		.HighlightText(this, &FMyEditor::GetFilterText)
		]
		]
		];
}

TSharedRef<ITableRow> FMyEditor::MakeRowWidget(FDataTableEditorRowListViewDataPtr InRowDataPtr, const TSharedRef<STableViewBase>& OwnerTable)
{
	return
		SNew(SDataTableListViewRow, OwnerTable)
		.DataTableEditor(SharedThis(this))
		.Item(InRowDataPtr);
}

FSlateColor FMyEditor::GetRowTextColor(FName RowName) const
{
	if (RowName == HighlightedRowName)
	{
		return FSlateColor(FColorList::Orange);
	}
	return FSlateColor::UseForeground();
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