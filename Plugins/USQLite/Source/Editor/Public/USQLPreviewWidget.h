//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "USQLite.h"
#include "USQLite_Shared.h"
#include "USQLPreviewButtonWidget.h"

#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Editor/PropertyEditor/Public/PropertyEditing.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SQLPreviewWidget : public SCompoundWidget {
private:
	TSharedPtr<FString> Search;
	TSharedPtr<FString> CopyID;
	TSharedPtr<IPropertyHandle> Rows;
	//
	TSharedPtr<SButton> CLEAR_Button;
	TSharedPtr<SButton> INSERT_Button;
	TSharedPtr<SUniformGridPanel> PreviewGrid;
	//
	TArray<TSharedPtr<SEditableTextBox>> RowIDBoxes;
	TArray<TSharedPtr<SEditableTextBox>> NewDataBoxes;
	//
	TArray<TSharedPtr<SQLPreviewButtonWidget>> COPY_Buttons;
	TArray<TSharedPtr<SQLPreviewButtonWidget>> PASTE_Buttons;
	TArray<TSharedPtr<SQLPreviewButtonWidget>> DELETE_Buttons;
public:
	SQLPreviewWidget();
	virtual ~SQLPreviewWidget() override;
	//
	SLATE_BEGIN_ARGS(SQLPreviewWidget)
	: _Rows(TSharedPtr<IPropertyHandle>())
	{}//
		SLATE_ATTRIBUTE(TSharedPtr<IPropertyHandle>,Rows)
	SLATE_END_ARGS()
	//
	void Construct(const FArguments &InArgs);
	//
	void OnInsertRow();
	void BuildPreview();
	void RefreshPanel();
	void OnUserScrolled(float ScrollOffset);
	void OnSearchChanged(const FText &Filter);
	void OnCopyRow(TSharedPtr<FString> RowID);
	void OnPasteRow(TSharedPtr<FString> ToID);
	void OnDeleteRow(TSharedPtr<FString> RowID);
	//
	FReply OnClickedCLEAR();
	FReply OnClickedUPSERT();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////