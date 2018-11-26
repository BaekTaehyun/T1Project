// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "T1SMetaDataView.h"

#include "Widgets/Text/SMultiLineEditableText.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableRow.h"

namespace MetaDataViewColumns
{
	/** IDs for list columns */
	static const FName ColumnID_Tag("Tag");
	static const FName ColumnID_Value("Value");
}

struct FT1MetaDataLine
{
	FT1MetaDataLine(FName InTag, const FString& InValue)
		: Tag(InTag)
		, Value(InValue)
	{
	}

	FName Tag;
	FString Value;
};

/**
 * The widget that represents a row in the MetaDataView's list view widget.  Generates a widget for each column, on-demand.
 */
class SMetaDataViewRow : public SMultiColumnTableRow< TSharedPtr< FT1MetaDataLine > >
{

public:

	SLATE_BEGIN_ARGS(SMetaDataViewRow) {}
	SLATE_END_ARGS()

	/**
	 * Construct this widget.  Called by the SNew() Slate macro.
	 *
	 * @param	InArgs				Declaration used by the SNew() macro to construct this widget
	 * @param	InMetaData			The metadata tag/value to display in the row widget
	 * @param	InOwnerTableView	The owner of the row widget
	 */
	void Construct(const FArguments& InArgs, TSharedRef< FT1MetaDataLine > InMetaData, TSharedRef< STableViewBase > InOwnerTableView);

	/**
	 * Constructs the widget that represents the specified ColumnID for this Row
	 *
	 * @param ColumnID    A unique ID for a column in this TableView; see SHeaderRow::FColumn for more info.
	 *
	 * @return a widget to represent the contents of a cell in this row of a TableView.
	 */
	virtual TSharedRef< SWidget > GenerateWidgetForColumn(const FName& ColumnID) override;

private:
	TSharedPtr< FT1MetaDataLine > MetaDataLine;
};

void SMetaDataViewRow::Construct(const FArguments& InArgs, TSharedRef< FT1MetaDataLine > InMetadata, TSharedRef< STableViewBase > InOwnerTableView)
{
	MetaDataLine = InMetadata;

	SMultiColumnTableRow< TSharedPtr< FT1MetaDataLine > >::Construct(FSuperRowType::FArguments(), InOwnerTableView);
}

TSharedRef< SWidget > SMetaDataViewRow::GenerateWidgetForColumn(const FName& ColumnID)
{
	TSharedPtr< SWidget > TableRowContent;

	static const FTextBlockStyle MetadataTextStyle = FTextBlockStyle(FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
		.SetFontSize(10);

	if (ColumnID == MetaDataViewColumns::ColumnID_Tag)
	{
		SAssignNew(TableRowContent, SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(1.5f)
		.FillWidth(100.0f)
		[
			SNew(SMultiLineEditableText)
			.Text(FText::FromName(MetaDataLine->Tag))
			.TextStyle(&MetadataTextStyle)
			.IsReadOnly(true)
		];
	}
	else if (ColumnID == MetaDataViewColumns::ColumnID_Value)
	{
		SAssignNew(TableRowContent, SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(1.5f)
		.FillWidth(400.0f)
		[
			SNew(SMultiLineEditableText)
			.Text(FText::FromString(*(MetaDataLine->Value)))
			.TextStyle(&MetadataTextStyle)
			.IsReadOnly(true)
			.AutoWrapText(true)
		];
	}
	else
	{
		checkf(false, TEXT("Unknown ColumnID provided to T1SMetaDataView"));
	}

	return TableRowContent.ToSharedRef();
}

void T1SMetaDataView::Construct(const FArguments& InArgs, const TMap<FName, FString>& InMetadata)
{
	for (auto It = InMetadata.CreateConstIterator(); It; ++It)
	{
		MetaDataLines.Add(MakeShared<FT1MetaDataLine>(FT1MetaDataLine(It->Key, It->Value)));
	}

	TSharedPtr< SHeaderRow > HeaderRowWidget =
		SNew(SHeaderRow)

		// Tag column
		+ SHeaderRow::Column(MetaDataViewColumns::ColumnID_Tag)
		.FillWidth(100.0f)
		.DefaultLabel(NSLOCTEXT("MetadataView", "ColumnID_Tag", "Tag"))
		.DefaultTooltip(FText())

		// Value column
		+ SHeaderRow::Column(MetaDataViewColumns::ColumnID_Value)
		.FillWidth(400.0f)
		.DefaultLabel(NSLOCTEXT("MetadataView", "ColumnID_Value", "Value"))
		.DefaultTooltip(FText());

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(SListView< TSharedPtr< FT1MetaDataLine > >)
			.ListItemsSource(&MetaDataLines)
			.OnGenerateRow(this, &T1SMetaDataView::OnGenerateRow)
			.HeaderRow(HeaderRowWidget)
		]
	];
}

TSharedRef< ITableRow > T1SMetaDataView::OnGenerateRow(const TSharedPtr< FT1MetaDataLine > Item, const TSharedRef< STableViewBase >& OwnerTable)
{
	return SNew(SMetaDataViewRow, Item.ToSharedRef(), OwnerTable);
}
