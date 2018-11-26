// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Toolkits/IToolkitHost.h"
#include "Styling/SlateColor.h"
#include "Types/SlateStructs.h"
#include "Widgets/SWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Toolkits/IToolkitHost.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"
#include "EditorUndoClient.h"
#include "Kismet2/StructureEditorUtils.h"
#include "DataTableEditorUtils.h"

//#include "Editor/DataTableEditor/Private/DataTableEditor.h"
DECLARE_DELEGATE_OneParam(FOnRowHighlighted, FName /*Row name*/);

class FMyEditor : public FAssetEditorToolkit
	, public FEditorUndoClient
	, public FStructureEditorUtils::INotifyOnStructChanged
	, public FDataTableEditorUtils::INotifyOnDataTableChanged
{
	friend class SDataTableListViewRow;

public:
	~FMyEditor();

	// 초기화 함수. 	
	void InitFMyEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class UDataActorComponent* InComponent);

	// IToolkit에서 상속받아 구현해야 할 가상함수들.
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FString GetDocumentationLink() const override
	{
		return TEXT("NotAvailable");
	}

	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;
	void HandleUndoRedo();

	// INotifyOnStructChanged
	virtual void PreChange(const class UUserDefinedStruct* Struct, FStructureEditorUtils::EStructureEditorChangeInfo Info) override;
	virtual void PostChange(const class UUserDefinedStruct* Struct, FStructureEditorUtils::EStructureEditorChangeInfo Info) override;

	// INotifyOnDataTableChanged
	virtual void PreChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info) override;
	virtual void PostChange(const UDataTable* Changed, FDataTableEditorUtils::EDataTableChangeInfo Info) override;
	virtual void SelectionChange(const UDataTable* Changed, FName RowName) override;	

private:
	// Slate 를 생성할 함수
	TSharedRef<SDockTab> SpawnTab_DataActorComponent(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_DataTableSelect(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_DataTable(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_RowEditor(const FSpawnTabArgs& Args);

	TSharedRef<SVerticalBox> CreateContentBox();
	TSharedRef<SWidget> CreateRowEditorBox();

	void LoadLayoutData();
	void SaveLayoutData();

	void RefreshCachedDataTable(const FName InCachedSelection = NAME_None, const bool bUpdateEvenIfValid = false);
	void UpdateVisibleRows(const FName InCachedSelection = NAME_None, const bool bUpdateEvenIfValid = false);
	void RestoreCachedSelection(const FName InCachedSelection, const bool bUpdateEvenIfValid = false);

	FText GetFilterText() const;
	void OnFilterTextChanged(const FText& InFilterText);
	FSlateColor GetRowTextColor(FName RowName) const;
	FText GetCellText(FDataTableEditorRowListViewDataPtr InRowDataPointer, int32 ColumnIndex) const;
	FText GetCellToolTipText(FDataTableEditorRowListViewDataPtr InRowDataPointer, int32 ColumnIndex) const;

	TSharedRef<ITableRow> MakeRowNameWidget(FDataTableEditorRowListViewDataPtr InRowDataPtr, const TSharedRef<STableViewBase>& OwnerTable);
	TSharedRef<ITableRow> MakeRowWidget(FDataTableEditorRowListViewDataPtr InRowDataPtr, const TSharedRef<STableViewBase>& OwnerTable);	
	TSharedRef<SWidget> MakeCellWidget(FDataTableEditorRowListViewDataPtr InRowDataPtr, const int32 InRowIndex, const FName& InColumnId);

	void OnRowNamesListViewScrolled(double InScrollOffset);
	void OnCellsListViewScrolled(double InScrollOffset);
	void OnRowSelectionChanged(FDataTableEditorRowListViewDataPtr InNewSelection, ESelectInfo::Type InSelectInfo);

	FOptionalSize GetRowNameColumnWidth() const;
	float GetColumnWidth(const int32 ColumnIndex) const;
	void OnColumnResized(const float NewWidth, const int32 ColumnIndex);

	void HandlePostChange();
	void SetHighlightedRow(FName Name);	

	const UDataTable* GetDataTable() const;
	UDataTable* GetDataTable();

private:
	struct FColumnWidth
	{
		FColumnWidth()
			: bIsAutoSized(true)
			, CurrentWidth(0.0f)
		{
		}

		/** True if this column is being auto-sized rather than sized by the user */
		bool bIsAutoSized;
		/** The width of the column, either sized by the user, or auto-sized */
		float CurrentWidth;
	};

	// 편집할 My Obj
	class UDataActorComponent* MyObj;	

	// My 에디터가 사용할 고유한 앱의 명칭.
	static const FName MyEditorAppIdentifier;

	// 각 Tab이 사용할 고유 명칭.
	static const FName DataActorComponentTabId;
	static const FName DataTableSelectTabId;
	static const FName DataTableTabId;	
	static const FName RowEditorTabId;
	static const FName RowNameColumnId;

	FName HighlightedRowName;
	FOnRowHighlighted CallbackOnRowHighlighted;	
	FSimpleDelegate CallbackOnDataTableUndoRedo;

	FText ActiveFilterText;
	float RowNameColumnWidth;
	TArray<FColumnWidth> ColumnWidths;

	TArray<FDataTableEditorColumnHeaderDataPtr> AvailableColumns;
	TArray<FDataTableEditorRowListViewDataPtr> AvailableRows;
	
	TSharedPtr<class IDetailsView> DataActorComponentView;
	TSharedPtr<SWidget> DataTableSelectView;
	TSharedPtr<SWidget> DataTableTabWidget;
	TSharedPtr<SWidget> RowEditorTabWidget;

	TSharedPtr<SHeaderRow> ColumnNamesHeaderRow;
	TSharedPtr<SListView<FDataTableEditorRowListViewDataPtr>> RowNamesListView;
	TSharedPtr<SListView<FDataTableEditorRowListViewDataPtr>> CellsListView;
	TArray<FDataTableEditorRowListViewDataPtr> VisibleRows;

	TSharedPtr<FJsonObject> LayoutData;
};
