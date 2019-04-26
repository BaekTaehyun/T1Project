//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "USQLPreviewWidget.h"
#include "Editor.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOCTEXT_NAMESPACE "Synaptech"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SQLPreviewWidget::SQLPreviewWidget() {
	Search = MakeShared<FString>(TEXT(""));
	CopyID = MakeShared<FString>(TEXT(""));
}

SQLPreviewWidget::~SQLPreviewWidget(){}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQLPreviewWidget::Construct(const FArguments &InArgs) {
	Rows = InArgs._Rows.Get();
	check(Rows->IsValidHandle());
	//
	TSharedRef<SQLPreviewWidget>ThisShared = StaticCastSharedRef<SQLPreviewWidget>(AsShared());
	PreviewGrid = SNew(SUniformGridPanel).MinDesiredSlotHeight(25).MinDesiredSlotWidth(35);
	TSharedRef<SScrollBox>ScrollBox = SNew(SScrollBox).OnUserScrolled(this,&SQLPreviewWidget::OnUserScrolled);
	//
	Search->Empty();
	BuildPreview();
	//
	ScrollBox->AddSlot()
	.HAlign(HAlign_Fill)
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot().AutoHeight()
		[
			PreviewGrid.ToSharedRef()
		]///
		+SVerticalBox::Slot().AutoHeight()
		.HAlign(HAlign_Fill).VAlign(VAlign_Fill)
		[
			SNew(SQLPreviewButtonWidget).Owner(ThisShared)
			.Mode(MakeShared<ESQLPreviewAction>(ESQLPreviewAction::Insert))
		]///
		+SVerticalBox::Slot().AutoHeight().Padding(2.f)
		[
			SNew(SSearchBox)
			.SelectAllTextWhenFocused(true)
			.HintText(LOCTEXT("PreviewSearch","Search..."))
			.OnTextChanged(this,&SQLPreviewWidget::OnSearchChanged)
		]///
	];
	//
	//
	ChildSlot.HAlign(HAlign_Fill).VAlign(VAlign_Fill)[ScrollBox];
}

void SQLPreviewWidget::BuildPreview() {
////////////////////////////////////////////////////////////////////////////////////////////////////
	if (!Rows->IsValidHandle()) {return;}
	//
	PreviewGrid->ClearChildren();
	DELETE_Buttons.Empty();
	PASTE_Buttons.Empty();
	COPY_Buttons.Empty();
	NewDataBoxes.Empty();
	RowIDBoxes.Empty();
	//
	uint32 R, P=0; Rows->GetNumChildren(R);
	TSharedRef<SQLPreviewWidget>ThisShared = StaticCastSharedRef<SQLPreviewWidget>(AsShared());
	//
	for (uint32 I=0; I<R; I++) {RowIDBoxes.Add(TSharedPtr<SEditableTextBox>(nullptr));}
	for (uint32 I=0; I<R; I++) {COPY_Buttons.Add(TSharedPtr<SQLPreviewButtonWidget>(nullptr));}
	for (uint32 I=0; I<R; I++) {PASTE_Buttons.Add(TSharedPtr<SQLPreviewButtonWidget>(nullptr));}
	for (uint32 I=0; I<R; I++) {DELETE_Buttons.Add(TSharedPtr<SQLPreviewButtonWidget>(nullptr));}
////////////////////////////////////////////////////////////////////////////////////////////////////
	PreviewGrid->AddSlot(0,0).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
	[
		SNew(SEditableTextBox).IsReadOnly(true)
		.BackgroundColor(FSlateColor(FLinearColor(0.25f,0.25f,0.25f)))
	];
////////////////////////////////////////////////////////////////////////////////////////////////////
if ((R>0)&&(Search->Len()<=1)) {
	for (uint32 I=0; I<R; I++) {
		TSharedPtr<IPropertyHandle>Row = Rows->GetChildHandle(I).ToSharedRef();
		TSharedPtr<IPropertyHandle>Types = Row->GetChildHandle(1).ToSharedRef();
		TSharedPtr<IPropertyHandle>Previews = Row->GetChildHandle(0).ToSharedRef();
		//
		Previews->GetNumChildren(P);
		for (uint32 C=0; C<P; C++) {
			TSharedPtr<IPropertyHandle>Preview = Previews->GetChildHandle(C).ToSharedRef();
			TSharedPtr<IPropertyHandle>Category = Types->GetChildHandle(C).ToSharedRef();
			//
			FString Column; Category->GetValue(Column);
			FString Value; Preview->GetValue(Value);
			FText Field = FText::FromString(Value);
			FText Tip = FText::FromString(Value);
			//
			if (I==0) {
				PreviewGrid->AddSlot(C+1,0).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
				[
					SNew(SEditableTextBox).IsReadOnly(true).Text(FText::FromString(Column))
					.BackgroundColor(FSlateColor(FLinearColor(0.25f,0.25f,0.25f)))
					.Font(FEditorStyle::GetFontStyle(TEXT("BoldFont")))
					.Padding(FMargin(5.f,0.f))
				];
			}///
			//
			if (C==0) {
				FString RowID;
				Preview->GetValue(RowID);
				//
				SAssignNew(DELETE_Buttons[I],SQLPreviewButtonWidget).Owner(ThisShared)
				.Mode(MakeShared<ESQLPreviewAction>(ESQLPreviewAction::Delete))
				.RowID(MakeShared<FString>(RowID));
				//
				PreviewGrid->AddSlot(0,I+1).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
				[
					DELETE_Buttons[I].ToSharedRef()
				];
			}///
			//
			if (Value!=TEXT("NULL")) {
				if (Value.Len()>=20) {
					Field = FText::FromString(Value.LeftChop(Value.Len()-5)+TEXT("..."));
				} if (C==0) {
					PreviewGrid->AddSlot(C+1,I+1).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
					[
						SAssignNew(RowIDBoxes[I],SEditableTextBox)
						.IsReadOnly(true).Text(Field).ToolTipText(Tip)
					];
				} else {
					PreviewGrid->AddSlot(C+1,I+1).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
					[
						SNew(SEditableTextBox).IsReadOnly(true).Text(Field).ToolTipText(Tip)
					];
				}///
			} else {
				PreviewGrid->AddSlot(C+1,I+1).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
				[
					SNew(SEditableTextBox)
					.IsReadOnly(true).Text(Field)
					.ForegroundColor(FSlateColor(FLinearColor::Gray))
					.BackgroundColor(FSlateColor(FLinearColor(0.85f,0.85f,0.85f,0.55f)))
				];
			}///
		}///
		//
		TSharedPtr<IPropertyHandle>Preview = Previews->GetChildHandle(0).ToSharedRef();
		FString RowID; Preview->GetValue(RowID);
		//
		if ((*CopyID.Get()).IsEmpty()||(*CopyID.Get()).Equals(RowID)) {
			SAssignNew(COPY_Buttons[I],SQLPreviewButtonWidget).Owner(ThisShared)
			.Mode(MakeShared<ESQLPreviewAction>(ESQLPreviewAction::Copy))
			.RowID(MakeShared<FString>(RowID));
			//
			PreviewGrid->AddSlot(P+1,I+1).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
			[
				COPY_Buttons[I].ToSharedRef()
			];
		} else if ((!(*CopyID.Get()).Equals(RowID))&&(!(*CopyID.Get()).IsEmpty())) {
			SAssignNew(PASTE_Buttons[I],SQLPreviewButtonWidget).Owner(ThisShared)
			.Mode(MakeShared<ESQLPreviewAction>(ESQLPreviewAction::Paste))
			.RowID(MakeShared<FString>(RowID));
			//
			PreviewGrid->AddSlot(P+1,I+1).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
			[
				PASTE_Buttons[I].ToSharedRef()
			];
		}///
	}///
}///
////////////////////////////////////////////////////////////////////////////////////////////////////
else if (R>0) {
	for (uint32 I=0; I<R; I++) {
		TSharedPtr<IPropertyHandle>Row = Rows->GetChildHandle(I).ToSharedRef();
		TSharedPtr<IPropertyHandle>Types = Row->GetChildHandle(1).ToSharedRef();
		TSharedPtr<IPropertyHandle>Previews = Row->GetChildHandle(0).ToSharedRef();
		//
		TSharedPtr<IPropertyHandle> ID = Previews->GetChildHandle(0).ToSharedRef();
		FString _ID; ID->GetValue(_ID); bool FoundID = _ID.Contains((*Search.Get()));
		//
		Previews->GetNumChildren(P);
		for (uint32 C=0; C<P; C++) {
			TSharedPtr<IPropertyHandle>Preview = Previews->GetChildHandle(C).ToSharedRef();
			TSharedPtr<IPropertyHandle>Category = Types->GetChildHandle(C).ToSharedRef();
			//
			FString Column; Category->GetValue(Column);
			FString Value; Preview->GetValue(Value);
			FText Field = FText::FromString(Value);
			FText Tip = FText::FromString(Value);
			//
			if (I==0) {
				PreviewGrid->AddSlot(C+1,0).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
				[
					SNew(SEditableTextBox).IsReadOnly(true).Text(FText::FromString(Column))
					.BackgroundColor(FSlateColor(FLinearColor(0.25f,0.25f,0.25f)))
					.Font(FEditorStyle::GetFontStyle(TEXT("BoldFont")))
					.Padding(FMargin(5.f,0.f))
				];
			}///
			//
			if (C==0) {
				FString RowID;
				Preview->GetValue(RowID);
				//
				SAssignNew(DELETE_Buttons[I],SQLPreviewButtonWidget).Owner(ThisShared)
				.Mode(MakeShared<ESQLPreviewAction>(ESQLPreviewAction::Delete))
				.RowID(MakeShared<FString>(RowID));
				//
				PreviewGrid->AddSlot(0,I+1).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
				[
					DELETE_Buttons[I].ToSharedRef()
				];
			}///
			//
			bool FoundField = Value.Contains((*Search.Get()));
			//
			if ((!FoundID)&&(!FoundField)) {
				if (Value.Len()>=20) {Field = FText::FromString(Value.LeftChop(Value.Len()-5)+TEXT("..."));}
				PreviewGrid->AddSlot(C+1,I+1).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
				[
					SNew(SEditableTextBox).Text(Field).IsReadOnly(true).ToolTipText(Tip)
					.BackgroundColor(FSlateColor(FLinearColor(0.65f,0.65f,0.65f,0.45f)))
				];
			continue;}
			//
			//
			if (Value!=TEXT("NULL")) {
				if (Value.Len()>=20) {
					Field = FText::FromString(Value.LeftChop(Value.Len()-5)+TEXT("..."));
				} if (C==0) {
					PreviewGrid->AddSlot(C+1,I+1).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
					[
						SAssignNew(RowIDBoxes[I],SEditableTextBox)
						.IsReadOnly(true).Text(Field).ToolTipText(Tip)
						.BackgroundColor(FSlateColor(FLinearColor(0.25f,0.85f,0.25f,0.85f)))
					];
				} else {
					PreviewGrid->AddSlot(C+1,I+1).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
					[
						SNew(SEditableTextBox).IsReadOnly(true).Text(Field).ToolTipText(Tip)
						.BackgroundColor(FSlateColor(FLinearColor(0.25f,0.85f,0.25f,0.85f)))
					];
				}///
			} else {
				PreviewGrid->AddSlot(C+1,I+1).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
				[
					SNew(SEditableTextBox)
					.IsReadOnly(true).Text(Field)
					.ForegroundColor(FSlateColor(FLinearColor::Gray))
					.BackgroundColor(FSlateColor(FLinearColor(0.25f,0.85f,0.25f,0.45f)))
				];
			}///
		}///
		//
		TSharedPtr<IPropertyHandle>Preview = Previews->GetChildHandle(0).ToSharedRef();
		FString RowID; Preview->GetValue(RowID);
		//
		if ((*CopyID.Get()).IsEmpty()||(*CopyID.Get()).Equals(RowID)) {
			SAssignNew(COPY_Buttons[I],SQLPreviewButtonWidget).Owner(ThisShared)
			.Mode(MakeShared<ESQLPreviewAction>(ESQLPreviewAction::Copy))
			.RowID(MakeShared<FString>(RowID));
			//
			PreviewGrid->AddSlot(P+1,I+1).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
			[
				COPY_Buttons[I].ToSharedRef()
			];
		} else if ((!(*CopyID.Get()).Equals(RowID))&&(!(*CopyID.Get()).IsEmpty())) {
			SAssignNew(PASTE_Buttons[I],SQLPreviewButtonWidget).Owner(ThisShared)
			.Mode(MakeShared<ESQLPreviewAction>(ESQLPreviewAction::Paste))
			.RowID(MakeShared<FString>(RowID));
			//
			PreviewGrid->AddSlot(P+1,I+1).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
			[
				PASTE_Buttons[I].ToSharedRef()
			];
		}///
	}///
}///
////////////////////////////////////////////////////////////////////////////////////////////////////
	if (R==0) {return;}	
	TSharedPtr<IPropertyHandle>Row = Rows->GetChildHandle(0).ToSharedRef();
	TSharedPtr<IPropertyHandle>Types = Row->GetChildHandle(1).ToSharedRef();
	//
	uint32 T; Types->GetNumChildren(T);
	for (uint32 C=0; C<T; C++) {NewDataBoxes.Add(TSharedPtr<SEditableTextBox>(nullptr));}
	//
	PreviewGrid->AddSlot(0,R+1).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
	[
		SNew(SEditableTextBox).IsReadOnly(true)
		.BackgroundColor(FSlateColor(FLinearColor(0.25f,0.25f,0.25f)))
	];
	//
	PreviewGrid->AddSlot(0,R+2).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
	[
		SAssignNew(CLEAR_Button,SButton)
		.ButtonStyle(FEditorStyle::Get(),"FlatButton.Default")
		.OnClicked(this,&SQLPreviewWidget::OnClickedCLEAR)
		.ForegroundColor(FSlateColor::UseForeground())
		.HAlign(HAlign_Fill).VAlign(VAlign_Center)
		[
			SNew(STextBlock).Margin(FMargin(8.f))
			.Font(FEditorStyle::GetFontStyle(TEXT("BoldFont")))
			.Text(LOCTEXT("ClearButton","CLEAR"))
			.Justification(ETextJustify::Center)
		]
	];
	//
	for (uint32 C=0; C<T; C++) {
		TSharedPtr<IPropertyHandle> Category = Types->GetChildHandle(C).ToSharedRef();
		FString Column; Category->GetValue(Column);
		//
		PreviewGrid->AddSlot(C+1,R+1).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
		[
			SNew(SEditableTextBox).IsReadOnly(true).Text(FText::FromString(Column))
			.BackgroundColor(FSlateColor(FLinearColor(0.25f,0.25f,0.25f)))
			.Font(FEditorStyle::GetFontStyle(TEXT("BoldFont")))
		];
		//
		PreviewGrid->AddSlot(C+1,R+2).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
		[
			SAssignNew(NewDataBoxes[C],SEditableTextBox)
			.BackgroundColor(FSlateColor(FLinearColor(0.55f,0.55f,0.55f)))
			.Padding(FMargin(8.f)).IsReadOnly(false)
		];
	}///
	//
	PreviewGrid->AddSlot(T+1,R+2).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
	[
		SAssignNew(INSERT_Button,SButton)
		.ButtonStyle(FEditorStyle::Get(),"FlatButton.Default")
		.OnClicked(this,&SQLPreviewWidget::OnClickedUPSERT)
		.ForegroundColor(FSlateColor::UseForeground())
		.HAlign(HAlign_Fill).VAlign(VAlign_Center)
		[
			SNew(STextBlock).Margin(FMargin(8.f))
			.Font(FEditorStyle::GetFontStyle(TEXT("BoldFont")))
			.Text(LOCTEXT("UpsertButton","UPSERT"))
			.Justification(ETextJustify::Center)
		]
	];
	//
	PreviewGrid->AddSlot(T+1,R+1).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
	[
		SNew(SEditableTextBox).IsReadOnly(true)
		.BackgroundColor(FSlateColor(FLinearColor(0.25f,0.25f,0.25f)))
	];
////////////////////////////////////////////////////////////////////////////////////////////////////
	PreviewGrid->AddSlot(T+1,0).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
	[
		SNew(SEditableTextBox).IsReadOnly(true)
		.BackgroundColor(FSlateColor(FLinearColor(0.25f,0.25f,0.25f)))
	];
////////////////////////////////////////////////////////////////////////////////////////////////////
}

void SQLPreviewWidget::OnSearchChanged(const FText& Filter) {
	*Search.Get() = Filter.ToString();
	//
	Invalidate(EInvalidateWidget::Layout);
	BuildPreview();
}

void SQLPreviewWidget::OnDeleteRow(TSharedPtr<FString> RowID) {
	if (!Rows->IsValidHandle()) {return;}
	//
	TArray<UObject*> Packages;
	Rows->GetOuterObjects(Packages);
	//
	for (auto Pack : Packages) {
		auto Owner = Cast<USQLite>(Pack);
		if (Owner) {Owner->OnPreviewDelete(*RowID.Get());}
	}///
	//
	Invalidate(EInvalidateWidget::Layout);
	BuildPreview();
}

void SQLPreviewWidget::OnCopyRow(TSharedPtr<FString> RowID) {
	(*CopyID.Get())=(*RowID.Get());
	//
	Invalidate(EInvalidateWidget::Layout);
	BuildPreview();
}

void SQLPreviewWidget::OnPasteRow(TSharedPtr<FString> ToID) {
	if ((*CopyID.Get()).Len()==0) {return;}
	if (!Rows->IsValidHandle()) {return;}
	//
	TArray<UObject*> Packages;
	Rows->GetOuterObjects(Packages);
	//
	for (auto Pack : Packages) {
		auto Owner = Cast<USQLite>(Pack);
		if (Owner) {Owner->OnPreviewPaste(*CopyID.Get(),*ToID.Get());}
	}///
	//
	CopyID = MakeShared<FString>(TEXT(""));
	//
	Invalidate(EInvalidateWidget::Layout);
	Search->Empty();
	BuildPreview();
}

void SQLPreviewWidget::OnInsertRow() {
	if (!Rows->IsValidHandle()) {return;}
	if (!GEditor) {return;}
	//
	int32 CA = GEditor->GetSelectedActorCount();
	int32 CC = GEditor->GetSelectedComponentCount();
	//
	if ((CA+CC)==0) {
		EAppReturnType::Type Result;
		auto Title = FText::FromString("USQLite");
		auto Warning = FText::FromString(TEXT(" Select Actors or Components then click 'Insert'.\n\n Target Object must have 'SQL Database Interface'\n added and implemented properly, else all values\n inserted will be NULL!"));
		Result = FMessageDialog::Open(EAppMsgType::Ok,Warning,&Title);
	return;}
	//
	TArray<UObject*> Packages;
	USQLite* Database = nullptr;
	Rows->GetOuterObjects(Packages);
	for (auto Pack : Packages) {Database = Cast<USQLite>(Pack);}
	//
	TArray<UObject*>Actors; TArray<UObject*>Components;
	GEditor->GetSelectedActors()->GetSelectedObjects(AActor::StaticClass(),Actors);
	GEditor->GetSelectedComponents()->GetSelectedObjects(UActorComponent::StaticClass(),Components);
	//
	if (Database) {
		for (auto CMP : Components) {
			FString SQL = Database->DB_GenerateSQL_Component_INSERT(CastChecked<UActorComponent>(CMP));
			Database->OnPreviewInsert(SQL);
		} if (Components.Num()>0) {return;}
		//
		for (auto Actor : Actors) {
			FString SQL = Database->DB_GenerateSQL_Actor_INSERT(CastChecked<AActor>(Actor));
			Database->OnPreviewInsert(SQL);
		}///
	}///
	//
	Invalidate(EInvalidateWidget::Layout);
	Search->Empty();
	BuildPreview();
}

void SQLPreviewWidget::OnUserScrolled(float ScrollOffset) {
	if (!Rows->IsValidHandle()) {return;}
	//
	TArray<UObject*> Packages;
	Rows->GetOuterObjects(Packages);
	//
	for (auto Pack : Packages) {
		auto Owner = Cast<USQLite>(Pack);
		if (Owner && Owner->DB_IsPreviewDirty()) {
			Invalidate(EInvalidateWidget::Layout);
			BuildPreview();
		}///
	}///
}

FReply SQLPreviewWidget::OnClickedCLEAR() {
	for (TSharedPtr<SEditableTextBox> Field : NewDataBoxes) {
		Field->SetText(FText());
	}///
	//
	return FReply::Handled();
}

FReply SQLPreviewWidget::OnClickedUPSERT() {
	if (NewDataBoxes.Num()==0) {return FReply::Handled();}
	if (!Rows->IsValidHandle()) {return FReply::Handled();}
	if (!NewDataBoxes[0].IsValid()) {return FReply::Handled();}
	//
	USQLite* Database = nullptr;
	TArray<UObject*> Packages;
	Rows->GetOuterObjects(Packages);
	//
	for (auto Pack : Packages) {
		Database = Cast<USQLite>(Pack);
		if (Database) {break;}
	} if (Database==nullptr) {return FReply::Handled();}
	//
	if (Database->HasAnyFlags(RF_ArchetypeObject)) {return FReply::Handled();}
	if (Database->DB_TABLE.ColumnTypes.Num()!=NewDataBoxes.Num()) {return FReply::Handled();}
	//
	//
	FString ID = NewDataBoxes[0]->GetText().ToString();
	if (ID.IsEmpty()) {return FReply::Handled();}
	//
	FString SQL, Header, Values;
	if (Database->DB_VERSION.CurrentVersion.IsEmpty()) {Header = FString::Printf(TEXT("INSERT OR REPLACE INTO %s ("),*Database->GetNameSanitized());}
	else {Header = FString::Printf(TEXT("INSERT OR REPLACE INTO %s ("),*((Database->GetNameSanitized()+TEXT("_"))+Database->DB_VERSION.CurrentVersion));}
	//
	for (int32 I=0; I<Database->DB_TABLE.Columns.Num(); ++I) {
		Header += FString::Printf(TEXT("%s,"),*Database->DB_TABLE.Columns[I]);
	} Header += TEXT(") ");
	//
	uint32 V=0; Values = FString(TEXT("VALUES ("));
	for (TSharedPtr<SEditableTextBox> Field : NewDataBoxes) {
		if (!Field.IsValid()) {V++; continue;}
		if (Field->GetText().IsEmpty()) {Values+=DBNULL; V++; continue;}
		if (V==0) {Values += (FString(TEXT("'"))+USQL::SanitizeString(Field->GetText().ToString())+TEXT("'")+TEXT(",")); V++; continue;}
		//
		if (Database->DB_TABLE.ColumnTypes[V].Equals(USQL::USQLTypes.Bools)) {
			const FString Value = (Field->GetText().ToString().Equals(TEXT("true"),ESearchCase::IgnoreCase)) ? TEXT("true") : TEXT("false");
			Values += FString(TEXT("'UBool>>"))+Value+TEXT("',");
		V++; continue;}
		//
		if (Database->DB_TABLE.ColumnTypes[V].Equals(USQL::USQLTypes.Bytes)) {
			const uint8 Value = FCString::Atoi(*Field->GetText().ToString());
			Values += FString::Printf(TEXT("%i"),Value)+TEXT(",");
		V++; continue;}
		//
		if (Database->DB_TABLE.ColumnTypes[V].Equals(USQL::USQLTypes.Ints)) {
			const int32 Value = FCString::Atoi(*Field->GetText().ToString());
			Values += FString::Printf(TEXT("%i"),Value)+TEXT(",");
		V++; continue;}
		//
		if (Database->DB_TABLE.ColumnTypes[V].Equals(USQL::USQLTypes.Floats)) {
			const float Value = FCString::Atof(*Field->GetText().ToString());
			Values += FString::Printf(TEXT("%f"),Value)+TEXT(",");
		V++; continue;}
		//
		if (Database->DB_TABLE.ColumnTypes[V].Equals(USQL::USQLTypes.Enums)) {
			Values += (FString(TEXT("'UEnum>>"))+Field->GetText().ToString()+TEXT("',"));
		V++; continue;}
		//
		if (Database->DB_TABLE.ColumnTypes[V].Equals(USQL::USQLTypes.Names)) {
			Values += (FString(TEXT("'UName>>"))+Field->GetText().ToString()+TEXT("',"));
		V++; continue;}
		//
		if (Database->DB_TABLE.ColumnTypes[V].Equals(USQL::USQLTypes.Texts)) {
			Values += (FString(TEXT("'UText>>"))+Field->GetText().ToString()+TEXT("',"));
		V++; continue;}
		//
		if (Database->DB_TABLE.ColumnTypes[V].Equals(USQL::USQLTypes.Strings)) {
			Values += (FString(TEXT("'UString>>"))+Field->GetText().ToString()+TEXT("',"));
		V++; continue;}
		//
		if (Database->DB_TABLE.ColumnTypes[V].Equals(USQL::USQLTypes.TimeStamps)) {
			Values += FString(TEXT("DATETIME('now','localtime'),"));
		V++; continue;}
		//
		if (Database->DB_TABLE.ColumnTypes[V].Equals(USQL::USQLTypes.DateTimes)) {
			const int64 Value = FCString::Atoi64(*Field->GetText().ToString());
			Values += FString::Printf(TEXT("%i"),Value)+TEXT(",");
		V++; continue;}
		//
		if (Database->DB_TABLE.ColumnTypes[V].Equals(USQL::USQLTypes.Arrays)) {
			Values += (FString(TEXT("'UArray>>{"))+Field->GetText().ToString()+TEXT("}',"));
		V++; continue;}
		//
		if (Database->DB_TABLE.ColumnTypes[V].Equals(USQL::USQLTypes.Sets)) {
			Values += (FString(TEXT("'USet>>{"))+Field->GetText().ToString()+TEXT("}',"));
		V++; continue;}
		//
		if (Database->DB_TABLE.ColumnTypes[V].Equals(USQL::USQLTypes.Maps)) {
			Values += (FString(TEXT("'UMap>>{"))+Field->GetText().ToString()+TEXT("}',"));
		V++; continue;}
		//
		if (Database->DB_TABLE.ColumnTypes[V].Equals(USQL::USQLTypes.Colors)) {
			Values += (FString(TEXT("'UColor>>{"))+Field->GetText().ToString()+TEXT("}',"));
		V++; continue;}
		//
		if (Database->DB_TABLE.ColumnTypes[V].Equals(USQL::USQLTypes.Vector2D)) {
			Values += (FString(TEXT("'UVector2D>>{"))+Field->GetText().ToString()+TEXT("}',"));
		V++; continue;}
		//
		if (Database->DB_TABLE.ColumnTypes[V].Equals(USQL::USQLTypes.Vector3D)) {
			Values += (FString(TEXT("'UVector3D>>{"))+Field->GetText().ToString()+TEXT("}',"));
		V++; continue;}
		//
		if (Database->DB_TABLE.ColumnTypes[V].Equals(USQL::USQLTypes.Rotators)) {
			Values += (FString(TEXT("'URotator>>{"))+Field->GetText().ToString()+TEXT("}',"));
		V++; continue;}
		//
		if (Database->DB_TABLE.ColumnTypes[V].Equals(USQL::USQLTypes.Structs)) {
			Values += (FString(TEXT("'UStruct>>{"))+Field->GetText().ToString()+TEXT("}',"));
		V++; continue;}
		//
		if (Database->DB_TABLE.ColumnTypes[V].Equals(USQL::USQLTypes.ObjectPtrs)) {
			Values += (FString(TEXT("'UObject>>"))+Field->GetText().ToString()+TEXT("',"));
		V++; continue;}
	V++;} Values += FString(TEXT(");"));
	//
	SQL = Header + Values;
	SQL = SQL.Replace(TEXT(",)"),TEXT(")"));
	//
	LOG_DB(true,ESQLSeverity::Info,SQL);
	Database->DB_UpsertPreview(SQL);
	//
	//
	Invalidate(EInvalidateWidget::Layout);
	RefreshPanel();
	//
	return FReply::Handled();
}

void SQLPreviewWidget::RefreshPanel() {
	FPropertyEditorModule &PE_Module = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PE_Module.NotifyCustomizationModuleChanged();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////