//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "USQLite.h"
#include "USQLComboBoxWidget.h"
#include "USQLite_Shared.h"

#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SQLTableWidget : public SCompoundWidget {
private:
	TArray<TSharedPtr<SQLComboBoxWidget>> ComboBoxes;
	TArray<TSharedPtr<SEditableTextBox>> TableFields;
	//
	TSharedPtr<IPropertyHandle> ColumnTypes;
	TSharedPtr<IPropertyHandle> Columns;
	TSharedPtr<IPropertyHandle> Header;
	TSharedPtr<IPropertyHandle> Footer;
	TSharedPtr<IPropertyHandle> Lock;
public:
	SQLTableWidget();
	virtual ~SQLTableWidget() override;
	//
	SLATE_BEGIN_ARGS(SQLTableWidget)
	: _Lock(TSharedPtr<IPropertyHandle>())
	, _Header(TSharedPtr<IPropertyHandle>())
	, _Footer(TSharedPtr<IPropertyHandle>())
	, _Columns(TSharedPtr<IPropertyHandle>())
	, _ColumnTypes(TSharedPtr<IPropertyHandle>())
	{}//
		SLATE_ATTRIBUTE(TSharedPtr<IPropertyHandle>,Lock)
		SLATE_ATTRIBUTE(TSharedPtr<IPropertyHandle>,Header)
		SLATE_ATTRIBUTE(TSharedPtr<IPropertyHandle>,Footer)
		SLATE_ATTRIBUTE(TSharedPtr<IPropertyHandle>,Columns)
		SLATE_ATTRIBUTE(TSharedPtr<IPropertyHandle>,ColumnTypes)
	SLATE_END_ARGS()
	//
	void Construct(const FArguments &InArgs);
	void SetColumns(TSharedPtr<IPropertyHandle> InColumns);
	void SetColumnTypes(TSharedPtr<IPropertyHandle> InColumnTypes);
	void OnSetColumnValue(const FText &Input, ETextCommit::Type CommitType) const;
	void SetColumnTypeByID(TSharedPtr<uint32> ColumnID, TSharedPtr<FString> NewType);
	//
	FText OnGetResultString() const;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////