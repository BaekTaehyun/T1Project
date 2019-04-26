//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "USQLTableWidget.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOCTEXT_NAMESPACE "Synaptech"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SQLTableWidget::SQLTableWidget(){}

SQLTableWidget::~SQLTableWidget() {
	TableFields.Empty();
	ComboBoxes.Empty();
	//
	Header.Reset();
	Footer.Reset();
	Columns.Reset();
	ColumnTypes.Reset();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQLTableWidget::Construct(const FArguments &InArgs) {
	Lock = InArgs._Lock.Get();
	Header = InArgs._Header.Get();
	Footer = InArgs._Footer.Get();
	Columns = InArgs._Columns.Get();
	ColumnTypes = InArgs._ColumnTypes.Get();
	//
	check(Lock->IsValidHandle());
	check(Header->IsValidHandle());
	check(Footer->IsValidHandle());
	check(Columns->IsValidHandle());
	check(ColumnTypes->IsValidHandle());
	//
	uint32 R, T;
	ComboBoxes.Empty();
	Columns->GetNumChildren(R);
	ColumnTypes->GetNumChildren(T);
	check(R==T);
	//
	//
	for (uint32 I=0; I<R; I++) {ComboBoxes.Add(TSharedPtr<SQLComboBoxWidget>(nullptr));}
	for (uint32 I=0; I<R; I++) {TableFields.Add(TSharedPtr<SEditableTextBox>(nullptr));}
	//
	TSharedRef<SQLTableWidget>ThisShared = StaticCastSharedRef<SQLTableWidget>(AsShared());
	TSharedRef<SUniformGridPanel>PanelUniformGrid = SNew(SUniformGridPanel).MinDesiredSlotHeight(40).MinDesiredSlotWidth(200).SlotPadding(FMargin(1.0f));
	//
	for (uint32 I=0; I<R; I++) {
		FString ColumnValue; bool Locked; Lock->GetValue(Locked);
		TSharedPtr<IPropertyHandle> ColumnHandle = ColumnTypes->GetChildHandle(I).ToSharedRef();
		//
		ColumnHandle->GetValue(ColumnValue);
		TSharedPtr<FString> ColumnType = MakeShared<FString>(ColumnValue);
		//
		PanelUniformGrid->AddSlot(I,0).HAlign(HAlign_Fill)
		[
			SAssignNew(ComboBoxes[I],SQLComboBoxWidget).IsEnabled(!Locked)
			.Owner(ThisShared).ColumnID(MakeShared<uint32>(I)).ColumnType(ColumnType)
		];
		//
		TSharedPtr<IPropertyHandle> Handle = Columns->GetChildHandle(I).ToSharedRef();
		FText Field; Handle->GetValueAsDisplayText(Field);
		PanelUniformGrid->AddSlot(I,1).HAlign(HAlign_Fill)
		[
			SAssignNew(TableFields[I],SEditableTextBox).IsReadOnly(Locked)
			.OnTextCommitted(this,&SQLTableWidget::OnSetColumnValue).Text(Field)
		];
	}///
	//
	//
	ChildSlot.HAlign(HAlign_Fill).VAlign(VAlign_Fill)[PanelUniformGrid];
}

void SQLTableWidget::OnSetColumnValue(const FText &Input, ETextCommit::Type CommitType) const {
	for (TSharedPtr<SEditableTextBox> Field : TableFields) {
		Field->SetBorderBackgroundColor(FSlateColor(FLinearColor::White));
		Field->SetForegroundColor(FSlateColor(FLinearColor::Black));
		Field->SetToolTipText(Input);
	}///
	//
	uint32 C=0; FText Warning = FText();
	for (TSharedPtr<SEditableTextBox> Field : TableFields) {
		TSharedPtr<IPropertyHandle> Handle = Columns->GetChildHandle(C).ToSharedRef();
		Handle->SetValue(Field->GetText().ToString());
		//
		for (TSharedPtr<SEditableTextBox> Other : TableFields) {
			if (Other==Field) {continue;}
			if (Other->GetText().ToString().Equals(Field->GetText().ToString(),ESearchCase::IgnoreCase)) {
				Field->SetForegroundColor(FSlateColor(FLinearColor::White));
				Other->SetForegroundColor(FSlateColor(FLinearColor::White));
				Field->SetBorderBackgroundColor(FSlateColor(FLinearColor(0.5f,0.1f,0.1f,0.5f)));
				Other->SetBorderBackgroundColor(FSlateColor(FLinearColor(0.5f,0.1f,0.1f,0.5f)));
				//
				Warning = FText::FromString(TEXT("SQL Table's Categories must be unique.\nYour Table has duplicate columns."));
				LOG_DB(USQL::Logs,ESQLSeverity::Warning,Warning.ToString());
				Field->SetToolTipText(Warning);
			}///
		}///
	C++;}
	//
	if (!Warning.IsEmpty()) {
		auto Title = FText::FromString(":: USQLite ::");
		FMessageDialog::Open(EAppMsgType::Ok,Warning,&Title);
	}///
}

FText SQLTableWidget::OnGetResultString() const {
	FString SQL, _Query;
	FString _Header; Header->GetValue(_Header);
	FString _Footer; Footer->GetValue(_Footer);
	uint32 R; Columns->GetNumChildren(R);
	//
	for (uint32 I=0; I<R; I++) {
		FString ColumnValue, ColumnType;
		TSharedPtr<IPropertyHandle> ColumnHandle = Columns->GetChildHandle(I).ToSharedRef();
		TSharedPtr<IPropertyHandle> TypeHandle = ColumnTypes->GetChildHandle(I).ToSharedRef();
		ColumnHandle->GetValue(ColumnValue); TypeHandle->GetValue(ColumnType);
		_Query += FString::Printf(TEXT("%s %s,"),*ColumnValue,*USQL::USQLTypes.Downcast(ColumnType,false));
	} SQL += _Header + _Query + _Footer; SQL = SQL.Replace(TEXT(",);"),TEXT(");"));
	//
	return FText::FromString(*SQL);
}

void SQLTableWidget::SetColumns(TSharedPtr<IPropertyHandle>InColumns) {
	Columns = InColumns;
}

void SQLTableWidget::SetColumnTypes(TSharedPtr<IPropertyHandle> InColumnTypes) {
	ColumnTypes = InColumnTypes;
}

void SQLTableWidget::SetColumnTypeByID(TSharedPtr<uint32> ColumnID, TSharedPtr<FString> NewType) {
	if (!ColumnID.IsValid()||!NewType.IsValid()) {return;}
	//
	TSharedPtr<IPropertyHandle>ColumnHandle = ColumnTypes->GetChildHandle(ColumnID.ToSharedRef().Get()).ToSharedRef();
	ColumnHandle->SetValue(*NewType.Get());
	//
	if (!ColumnTypes->IsValidHandle()) {return;}
	//
	TArray<UObject*> Packages;
	ColumnTypes->GetOuterObjects(Packages);
	//
	for (auto Pack : Packages) {
		Pack->MarkPackageDirty();
		auto Owner = Cast<USQLite>(Pack);
		if (Owner) {Owner->OnTableDirty();}
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////