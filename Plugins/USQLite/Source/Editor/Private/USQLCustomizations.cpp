//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "USQLCustomizations.h"
#include "USQLEditor_Shared.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOCTEXT_NAMESPACE "Synaptech"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// General Templates

template<typename T>
T* GetBuilder(IDetailLayoutBuilder* DetailBuilder) {
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder->GetObjectsBeingCustomized(Objects);
	//
	T* OBJ = nullptr;
	//
	if (Objects.Num() > 0) {
		OBJ = Cast<T>(Objects[0].Get());
	} return OBJ;
}

UBlueprint* GetBlueprint(UObject* OBJ) {
	UBlueprint* BP = nullptr;
	//
	if (OBJ != nullptr) {
		BP = Cast<UBlueprint>(OBJ);
		if (BP == nullptr) {
			BP = Cast<UBlueprint>(OBJ->GetClass()->ClassGeneratedBy);
	}} return BP;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// General Details Injection

TSharedRef<IDetailCustomization> FUSQLDetails::MakeInstance() {
	return MakeShareable(new FUSQLDetails);
}

void FUSQLDetails::CustomizeDetails(IDetailLayoutBuilder &DetailBuilder) {
	const auto &SQL = GetBuilder<USQLite>(&DetailBuilder);
	if (SQL==nullptr) {return;}

	/// :: TABLE SECTION ::
	
	FText Lock = (SQL->DB_TABLE.Lock) ? FText::FromString(TEXT("UNLOCK TABLE")) : FText::FromString(TEXT("LOCK TABLE"));
	IDetailCategoryBuilder &TB_Category=DetailBuilder.EditCategory("_TABLE");
	TB_Category.AddCustomRow(FText::FromString("_TABLE"))
	.WholeRowContent().MinDesiredWidth(200)
	[
		SNew(SUniformGridPanel).MinDesiredSlotWidth(100).MinDesiredSlotHeight(45).SlotPadding(FMargin(2.0f))
		+SUniformGridPanel::Slot(0,0).HAlign(HAlign_Fill)
		[
			SNew(SButton).IsEnabled(&FUSQLDetails::OnEnable)
			.OnClicked(FOnClicked::CreateStatic(&FUSQLDetails::OnLockTable,&DetailBuilder))
			.ButtonStyle(FEditorStyle::Get(),"FlatButton.Default")
			.ForegroundColor(FSlateColor::UseForeground())
			.VAlign(VAlign_Center).HAlign(HAlign_Fill)
			.ContentPadding(FMargin(2.0f))
			[
				SNew(STextBlock)
				.Justification(ETextJustify::Center).Text(Lock)
				.Font(FEditorStyle::GetFontStyle(TEXT("BoldFont")))
			]///
		]///
		+SUniformGridPanel::Slot(1,0).HAlign(HAlign_Fill)
		[
			SNew(SButton)
			.ButtonStyle(FEditorStyle::Get(),"FlatButton.Default").IsEnabled(!SQL->DB_TABLE.Lock)
			.OnClicked(FOnClicked::CreateStatic(&FUSQLDetails::OnAddTableColumn,&DetailBuilder))
			.ForegroundColor(FSlateColor::UseForeground())
			.VAlign(VAlign_Center).HAlign(HAlign_Fill)
			[
				SNew(STextBlock)
				.Justification(ETextJustify::Center)
				.Text(LOCTEXT("AddTableColumn","+ADD COLUMN"))
				.Font(FEditorStyle::GetFontStyle(TEXT("BoldFont")))
				.ToolTipText(LOCTEXT("AddColumnTooltip","Add a new Column Category to the Table Structure of this Database.\nWon't take affect to the physical Database file until it's rebuilt."))
			]///
		]///
		+SUniformGridPanel::Slot(2,0).HAlign(HAlign_Fill)
		[
			SNew(SButton)
			.OnClicked(FOnClicked::CreateStatic(&FUSQLDetails::OnDeleteTableColumn,&DetailBuilder))
			.ButtonStyle(FEditorStyle::Get(),"FlatButton.Default").IsEnabled(!SQL->DB_TABLE.Lock)
			.ForegroundColor(FSlateColor::UseForeground())
			.VAlign(VAlign_Center).HAlign(HAlign_Fill)
			[
				SNew(STextBlock)
				.Justification(ETextJustify::Center)
				.Font(FEditorStyle::GetFontStyle(TEXT("BoldFont")))
				.Text(LOCTEXT("DeleteTableColumn","DELETE LAST COLUMN"))
				.ToolTipText(LOCTEXT("DeleteColumnTooltip","Delete last Column Category from Table Structure of this Database.\nWon't take affect to the physical Database file until it's rebuilt."))
			]///
		]///
		+SUniformGridPanel::Slot(3,0).HAlign(HAlign_Fill)
		[
			SNew(SButton)
			.ButtonStyle(FEditorStyle::Get(),"FlatButton.Default").IsEnabled(!SQL->DB_TABLE.Lock)
			.OnClicked(FOnClicked::CreateStatic(&FUSQLDetails::OnClearTable,&DetailBuilder))
			.ForegroundColor(FSlateColor::UseForeground())
			.VAlign(VAlign_Center).HAlign(HAlign_Fill)
			[
				SNew(STextBlock)
				.Justification(ETextJustify::Center)
				.Font(FEditorStyle::GetFontStyle(TEXT("BoldFont")))
				.Text(LOCTEXT("ClearTable","CLEAR TABLE"))
				.ToolTipText(LOCTEXT("ClearTableTooltip","Clear up Table Structure of this Database. Won't take affect to the physical Database file until it's rebuilt."))
			]///
		]///
		+SUniformGridPanel::Slot(4,0).HAlign(HAlign_Fill)
		[
			SNew(SButton)
			.ButtonStyle(FEditorStyle::Get(),"FlatButton.Default").IsEnabled(!SQL->DB_TABLE.Lock)
			.OnClicked(FOnClicked::CreateStatic(&FUSQLDetails::OnTableApply,&DetailBuilder))
			.ForegroundColor(FSlateColor::UseForeground())
			.VAlign(VAlign_Center).HAlign(HAlign_Fill)
			[
				SNew(STextBlock)
				.Justification(ETextJustify::Center)
				.Font(FEditorStyle::GetFontStyle(TEXT("BoldFont")))
				.Text(LOCTEXT("Rebuild","DROP & REBUILD DATABASE"))
				.ToolTipText(LOCTEXT("RebuildToolTip",":: WARNING ::\nAll records currently stored in physical Database will be erased if its Table is rebuilt !!"))
			]///
		]///
	];

	/// :: PREVIEW SECTION ::

	IDetailCategoryBuilder &PV_Category = DetailBuilder.EditCategory("DATABASE");
	PV_Category.AddCustomRow(FText::FromString("DATABASE"))
	.WholeRowContent().MinDesiredWidth(100)
	[
		SNew(SUniformGridPanel).MinDesiredSlotWidth(100).MinDesiredSlotHeight(45).SlotPadding(FMargin(2.0f))
		+SUniformGridPanel::Slot(0,0)
		.HAlign(HAlign_Fill)
		[
			SNew(SButton).IsEnabled(&FUSQLDetails::OnEnable)
			.OnClicked(FOnClicked::CreateStatic(&FUSQLDetails::OnRefreshPreview,&DetailBuilder))
			.ButtonStyle(FEditorStyle::Get(),"FlatButton.Default")
			.ForegroundColor(FSlateColor::UseForeground())
			.VAlign(VAlign_Center).HAlign(HAlign_Fill)
			[
				SNew(STextBlock)
				.Justification(ETextJustify::Center)
				.Font(FEditorStyle::GetFontStyle(TEXT("BoldFont")))
				.Text(LOCTEXT("RefreshPreview","REFRESH DATA PREVIEW"))
				.ToolTipText(LOCTEXT("PreviewTooltip","Reimports Preview Data from Versioned Table of this Database.\nOnly Rows owned by 'UObjects' are visible in this Panel.\nOwner UObject of a Row must be currently loaded to be visible in this Panel."))
			]///
		]///
	];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TAttribute<bool> FUSQLDetails::OnEnable() {
	return TAttribute<bool>(USQL::GetThreadSafety()==ESQLThreadSafety::IsCurrentlyThreadSafe);
}

FReply FUSQLDetails::OnAddTableColumn(IDetailLayoutBuilder* DetailBuilder) {
	if (USQL::GetThreadSafety()!=ESQLThreadSafety::IsCurrentlyThreadSafe) {return FReply::Handled();}
	auto USQLiteOBJ = GetBuilder<USQLite>(DetailBuilder);
	//
	if (USQLiteOBJ&&(!USQLiteOBJ->DB_TABLE.Lock)) {
		USQLiteOBJ->DB_TABLE.ColumnTypes.Add(USQL::USQLTypes.Ints);
		USQLiteOBJ->DB_TABLE.Columns.Add(FString::Printf(TEXT("ID%i"),USQLiteOBJ->DB_TABLE.Columns.Num()));
		//
		USQLiteOBJ->MarkPackageDirty();
		DetailBuilder->ForceRefreshDetails();
	} return FReply::Handled();
}

FReply FUSQLDetails::OnDeleteTableColumn(IDetailLayoutBuilder* DetailBuilder) {
	if (USQL::GetThreadSafety()!=ESQLThreadSafety::IsCurrentlyThreadSafe) {return FReply::Handled();}
	auto USQLiteOBJ = GetBuilder<USQLite>(DetailBuilder);
	//
	if (USQLiteOBJ&&(!USQLiteOBJ->DB_TABLE.Lock)) {
		if (USQLiteOBJ->DB_TABLE.Columns.Num()<1) {return FReply::Handled();}
		USQLiteOBJ->DB_TABLE.ColumnTypes.RemoveAt(USQLiteOBJ->DB_TABLE.ColumnTypes.Num()-1);
		USQLiteOBJ->DB_TABLE.Columns.RemoveAt(USQLiteOBJ->DB_TABLE.Columns.Num()-1);
		//
		USQLiteOBJ->MarkPackageDirty();
		DetailBuilder->ForceRefreshDetails();
	} return FReply::Handled();
}

FReply FUSQLDetails::OnClearTable(IDetailLayoutBuilder* DetailBuilder) {
	if (USQL::GetThreadSafety()!=ESQLThreadSafety::IsCurrentlyThreadSafe) {return FReply::Handled();}
	auto USQLiteOBJ = GetBuilder<USQLite>(DetailBuilder);
	//
	if (USQLiteOBJ&&(!USQLiteOBJ->DB_TABLE.Lock)) {
		USQLiteOBJ->DB_TABLE.Columns.Empty();
		USQLiteOBJ->DB_TABLE.ColumnTypes.Empty();
		USQLiteOBJ->DB_TABLE.Columns.Add(TEXT("ID"));
		USQLiteOBJ->DB_TABLE.ColumnTypes.Add(USQL::USQLTypes.Texts+TEXT(" PRIMARY KEY NOT NULL"));
		//
		USQLiteOBJ->MarkPackageDirty();
		DetailBuilder->ForceRefreshDetails();
	} return FReply::Handled();
}

FReply FUSQLDetails::OnLockTable(IDetailLayoutBuilder* DetailBuilder) {
	if (USQL::GetThreadSafety()!=ESQLThreadSafety::IsCurrentlyThreadSafe) {return FReply::Handled();}
	auto USQLiteOBJ = GetBuilder<USQLite>(DetailBuilder);
	//
	if (USQLiteOBJ) {
		USQLiteOBJ->DB_TABLE.Lock = !USQLiteOBJ->DB_TABLE.Lock;
		DetailBuilder->ForceRefreshDetails();
	} return FReply::Handled();
}

FReply FUSQLDetails::OnTableApply(IDetailLayoutBuilder* DetailBuilder) {
	auto USQLiteOBJ = GetBuilder<USQLite>(DetailBuilder);
	if ((USQLiteOBJ==nullptr)||(USQLiteOBJ->DB_TABLE.Lock)) {return FReply::Handled();}
	if (USQL::GetThreadSafety()!=ESQLThreadSafety::IsCurrentlyThreadSafe) {return FReply::Handled();}
	//
	EAppReturnType::Type Result;
	auto Title = FText::FromString(*USQLiteOBJ->GetName());
	auto Warning = FText::FromString(TEXT("WARNING !!! \n\n Are you sure you want to rebuild this whole \n Table for the physical Database file ? \n All data currently recorded will be lost! \n\n This action cannot be undone!"));
	Result = FMessageDialog::Open(EAppMsgType::YesNo,Warning,&Title);
	//
	//
	switch (Result) {
		case EAppReturnType::Yes:
		{
			USQLiteOBJ->OnTableDirty();
			USQLiteOBJ->MarkPackageDirty();
			DetailBuilder->ForceRefreshDetails();
		}	break;
	default: break;}
	//
	//
	return FReply::Handled();
}

FReply FUSQLDetails::OnRefreshPreview(IDetailLayoutBuilder* DetailBuilder) {
	if (USQL::GetThreadSafety()!=ESQLThreadSafety::IsCurrentlyThreadSafe) {return FReply::Handled();}
	auto USQLiteOBJ = GetBuilder<USQLite>(DetailBuilder);
	//
	if (USQLiteOBJ) {
		USQLiteOBJ->DB_RefreshPreview();
		DetailBuilder->ForceRefreshDetails();
	} return FReply::Handled();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DB Table Widget Injection

TSharedRef<IPropertyTypeCustomization> FUSQLTableDetails::MakeInstance() {
	return MakeShareable(new FUSQLTableDetails());
}

void FUSQLTableDetails::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow &HeaderRow, IPropertyTypeCustomizationUtils &StructCustomizationUtils) {
	ColumnTypes = StructPropertyHandle->GetChildHandle(ColumnTypesID).ToSharedRef();
	Columns = StructPropertyHandle->GetChildHandle(ColumnsID).ToSharedRef();
	Header = StructPropertyHandle->GetChildHandle(HeaderID).ToSharedRef();
	Footer = StructPropertyHandle->GetChildHandle(FooterID).ToSharedRef();
	Lock = StructPropertyHandle->GetChildHandle(LockID).ToSharedRef();
	//
	if (!Lock->IsValidHandle()) {return;}
	if (!Header->IsValidHandle()) {return;}
	if (!Footer->IsValidHandle()) {return;}
	if (!Columns->IsValidHandle()) {return;}
	if (!ColumnTypes->IsValidHandle()) {return;}
	//
	HeaderRow.WholeRowContent().MinDesiredWidth(200)
	[
		SNew(SQLTableWidget).Header(Header).ColumnTypes(ColumnTypes).Columns(Columns).Footer(Footer).Lock(Lock)
	];
}

void FUSQLTableDetails::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder &StructBuilder, IPropertyTypeCustomizationUtils &StructCustomizationUtils) {}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DB Preview Widget Injection

TSharedRef<IPropertyTypeCustomization> FUSQLPreviewDetails::MakeInstance() {
	return MakeShareable(new FUSQLPreviewDetails());
}

void FUSQLPreviewDetails::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow &HeaderRow, IPropertyTypeCustomizationUtils &StructCustomizationUtils) {
	Rows = StructPropertyHandle->GetChildHandle(0).ToSharedRef();
	//
	HeaderRow.WholeRowContent().MinDesiredWidth(300)
	[
		SNew(SQLPreviewWidget).Rows(Rows)
	];
}

void FUSQLPreviewDetails::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder &StructBuilder, IPropertyTypeCustomizationUtils &StructCustomizationUtils) {}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DB Version Widget Injection

TSharedRef<IPropertyTypeCustomization> FUSQLVersionDetails::MakeInstance() {
	return MakeShareable(new FUSQLVersionDetails());
}

void FUSQLVersionDetails::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow &HeaderRow, IPropertyTypeCustomizationUtils &StructCustomizationUtils) {
	Version = StructPropertyHandle->GetChildHandle(0).ToSharedRef();
	Versions = StructPropertyHandle->GetChildHandle(1).ToSharedRef();
	uint32 V; Versions->GetNumChildren(V);
	//
	if (!Version->IsValidHandle()||!Versions->IsValidHandle()) {return;}
	HeaderRow.NameContent()
	[
		SNew(SHorizontalBox)
		.ToolTipText(FText::FromString("Database Table's Active Version.\nMultiple versions of Main Table can be generated and used across multiple game releases."))
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(0,0,5,0)
		.AutoWidth()
		[
			SNew(SImage)
			.Image(FSQLStyle::Get()->GetBrush("ClassIcon.SQLite"))
		]///
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Font(IDetailLayoutBuilder::GetDetailFont())
			.Text(FText::FromString("Target Active Version:"))
		]///
	]///
	.ValueContent()
	.MinDesiredWidth(300)
	.MaxDesiredWidth(600)
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			SNew(SQLVersionBoxWidget).VersionList(Versions).Version(Version)
		]///
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center).Padding(2.f)
		[
			SNew(SButton).IsEnabled(V>0)
			.ButtonStyle(FEditorStyle::Get(),"FlatButton.Default")
			.OnClicked(this,&FUSQLVersionDetails::OnDropTableButtonClicked)
			.ForegroundColor(FSlateColor::UseForeground())
			.VAlign(VAlign_Center).HAlign(HAlign_Fill)
			[
				SNew(STextBlock)
				.Justification(ETextJustify::Center)
				.Font(FEditorStyle::GetFontStyle(TEXT("BoldFont")))
				.Text(LOCTEXT("DropTableButton","DROP THIS TABLE"))
				.ToolTipText(LOCTEXT("DropTableTooltip","Completely removes Active Table from Database's physical file."))
			]///
		]///
	];
}

FReply FUSQLVersionDetails::OnDropTableButtonClicked() {
	if (USQL::GetThreadSafety()!=ESQLThreadSafety::IsCurrentlyThreadSafe) {return FReply::Handled();}
	if (!Version->IsValidHandle()) {return FReply::Handled();}
	//
	EAppReturnType::Type Result;
	auto Title = FText::FromString("USQLite");
	auto Warning = FText::FromString(TEXT("Are you sure you want to delete this \n Table from the physical Database file ? \n\n This action cannot be undone!"));
	Result = FMessageDialog::Open(EAppMsgType::YesNo,Warning,&Title);
	//
	switch (Result) {
		case EAppReturnType::Yes:
		{
			TArray<UObject*> Packages;
			Version->GetOuterObjects(Packages);
			//
			for (auto Pack : Packages) {
				auto Owner = Cast<USQLite>(Pack);
				if (Owner) {Owner->OnVersioningDelete();}
			}///
		}	break;
	default: break;}
	//
	return FReply::Handled();
}

void FUSQLVersionDetails::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder &StructBuilder, IPropertyTypeCustomizationUtils &StructCustomizationUtils) {}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////