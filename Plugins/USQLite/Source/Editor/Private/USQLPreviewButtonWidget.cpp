//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "USQLPreviewButtonWidget.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOCTEXT_NAMESPACE "Synaptech"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SQLPreviewButtonWidget::SQLPreviewButtonWidget(){
	Mode = MakeShared<ESQLPreviewAction>(ESQLPreviewAction::Copy);
}

SQLPreviewButtonWidget::~SQLPreviewButtonWidget(){}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQLPreviewButtonWidget::Construct(const FArguments &InArgs) {
	RowID = InArgs._RowID.Get();
	Owner = InArgs._Owner.Get();
	Mode = InArgs._Mode.Get();
	check(Owner.Get());
	//
	TSharedRef<SButton>Button = SNew(SButton);
	FText Caption = FText::FromString(TEXT(""));
	FText Tooltip = FText::FromString(TEXT(""));
	//
	switch (*Mode.Get()) {
		case ESQLPreviewAction::Copy:
		{
			Caption = LOCTEXT("CopyButton","COPY");
			Tooltip = LOCTEXT("CopyToolTip","Copy this entire row record from the Database file.");
		}	break;
		//
		case ESQLPreviewAction::Paste:
		{
			Caption = LOCTEXT("PasteButton","PASTE");
			Tooltip = LOCTEXT("PasteToolTip","Paste copy onto this row record in the Database file.");
		}	break;
		//
		case ESQLPreviewAction::Delete:
		{
			Caption = LOCTEXT("DeleteButton","DELETE");
			Tooltip = LOCTEXT("DeleteToolTip","Delete this entire row record from the Database file.");
		}	break;
		//
		case ESQLPreviewAction::Insert:
		{
			Caption = LOCTEXT("InsertButton","INSERT");
			Tooltip = LOCTEXT("InsertToolTip","Insert a new row record into the Database file.");
		}	break;
	default: break;}
	//
	//
	SAssignNew(Button,SButton)
	.ButtonStyle(FEditorStyle::Get(),"FlatButton.Default")
	.OnClicked(this,&SQLPreviewButtonWidget::OnButtonClicked)
	.ForegroundColor(FSlateColor::UseForeground())
	.HAlign(HAlign_Fill).VAlign(VAlign_Center)
	[
		SNew(STextBlock).Margin(FMargin(8.f))
		.Font(FEditorStyle::GetFontStyle(TEXT("BoldFont")))
		.Justification(ETextJustify::Center)
		.ToolTipText(Tooltip)
		.Text(Caption)
	];
	//
	//
	ChildSlot.HAlign(HAlign_Fill).VAlign(VAlign_Fill)[Button];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FReply SQLPreviewButtonWidget::OnButtonClicked() {
	switch (*Mode.Get()) {
		case ESQLPreviewAction::Copy:
		{
			return OnClickedCOPY();
		}	break;
		//
		case ESQLPreviewAction::Paste:
		{
			return OnClickedPASTE();
		}	break;
		//
		case ESQLPreviewAction::Delete:
		{
			return OnClickedDELETE();
		}	break;
		//
		case ESQLPreviewAction::Insert:
		{
			return OnClickedINSERT();
		}	break;
	default: break;}
	//
	return FReply::Handled();
}

FReply SQLPreviewButtonWidget::OnClickedDELETE() {
	EAppReturnType::Type Result;
	auto Title = FText::FromString("USQLite");
	auto Warning = FText::FromString(TEXT("Are you sure you want to delete this \n record from the physical Database file ? \n\n This action cannot be undone!"));
	Result = FMessageDialog::Open(EAppMsgType::YesNo,Warning,&Title);
	//
	switch (Result) {
		case EAppReturnType::Yes:
		{
			Owner->OnDeleteRow(RowID);
		}	break;
	default: break;}
	//
	return FReply::Handled();
}

FReply SQLPreviewButtonWidget::OnClickedCOPY() {
	Owner->OnCopyRow(RowID);
	return FReply::Handled();
}

FReply SQLPreviewButtonWidget::OnClickedPASTE() {
	EAppReturnType::Type Result;
	auto Title = FText::FromString("USQLite");
	auto Warning = FText::FromString(TEXT("Are you sure you want to overwrite this \n record in the physical Database file ? \n\n This action cannot be undone!"));
	Result = FMessageDialog::Open(EAppMsgType::YesNo,Warning,&Title);
	//
	switch (Result) {
		case EAppReturnType::Yes:
		{
			Owner->OnPasteRow(RowID);
		}	break;
	default: break;}
	//
	return FReply::Handled();
}

FReply SQLPreviewButtonWidget::OnClickedINSERT() {
	Owner->OnInsertRow();
	return FReply::Handled();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////