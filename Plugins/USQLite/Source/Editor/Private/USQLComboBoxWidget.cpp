//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "USQLComboBoxWidget.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOCTEXT_NAMESPACE "Synaptech"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SQLComboBoxWidget::SQLComboBoxWidget(){}

SQLComboBoxWidget::~SQLComboBoxWidget() {
	KeyTypeList.Empty();
	ColumnType.Reset();
	TypeList.Empty();
	ComboBox.Reset();
	ColumnID.Reset();
	Owner.Reset();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQLComboBoxWidget::Construct(const FArguments &InArgs) {
	ColumnType = InArgs._ColumnType.Get();
	ColumnID = InArgs._ColumnID.Get();
	Owner = InArgs._Owner.Get();
	//
	check(Owner.IsValid());
	check(ColumnType.IsValid());
	check(ColumnID.IsValid());
	//
	TypeList.Empty();
	KeyTypeList.Empty();
	TypeList.Add(MakeShared<FString>(USQL::USQLTypes.Arrays));
	TypeList.Add(MakeShared<FString>(USQL::USQLTypes.Bools));
	TypeList.Add(MakeShared<FString>(USQL::USQLTypes.Bytes));
	TypeList.Add(MakeShared<FString>(USQL::USQLTypes.Colors));
	TypeList.Add(MakeShared<FString>(USQL::USQLTypes.DateTimes));
	TypeList.Add(MakeShared<FString>(USQL::USQLTypes.Enums));
	TypeList.Add(MakeShared<FString>(USQL::USQLTypes.Floats));
	TypeList.Add(MakeShared<FString>(USQL::USQLTypes.Ints));
	TypeList.Add(MakeShared<FString>(USQL::USQLTypes.Maps));
	TypeList.Add(MakeShared<FString>(USQL::USQLTypes.Names));
	TypeList.Add(MakeShared<FString>(USQL::USQLTypes.ObjectPtrs));
	TypeList.Add(MakeShared<FString>(USQL::USQLTypes.Rotators));
	TypeList.Add(MakeShared<FString>(USQL::USQLTypes.Strings));
	TypeList.Add(MakeShared<FString>(USQL::USQLTypes.Sets));
	TypeList.Add(MakeShared<FString>(USQL::USQLTypes.Structs));
	TypeList.Add(MakeShared<FString>(USQL::USQLTypes.Texts));
	TypeList.Add(MakeShared<FString>(USQL::USQLTypes.TimeStamps));
	TypeList.Add(MakeShared<FString>(USQL::USQLTypes.Vector2D));
	TypeList.Add(MakeShared<FString>(USQL::USQLTypes.Vector3D));
	KeyTypeList.Add(MakeShared<FString>(USQL::USQLTypes.Texts+TEXT(" PRIMARY KEY NOT NULL")));
	//
	//
	if (ColumnID.ToSharedRef().Get()<1) {
		SAssignNew(ComboBox,SComboBox<TSharedPtr<FString>>)
		.OnSelectionChanged(this,&SQLComboBoxWidget::OnTypeComboBoxSelectionChanged)
		.OnGenerateWidget(this,&SQLComboBoxWidget::OnGenerateTypeComboBox)
		.OptionsSource(&KeyTypeList)
		.Content()
		[
			SNew(STextBlock)
			.Text(this,&SQLComboBoxWidget::GetComboBoxContent)
			.Font(IDetailLayoutBuilder::GetDetailFont())
		];
	} else {
		SAssignNew(ComboBox,SComboBox<TSharedPtr<FString>>)
		.OnSelectionChanged(this,&SQLComboBoxWidget::OnTypeComboBoxSelectionChanged)
		.OnGenerateWidget(this,&SQLComboBoxWidget::OnGenerateTypeComboBox)
		.OptionsSource(&TypeList)
		.Content()
		[
			SNew(STextBlock)
			.Text(this,&SQLComboBoxWidget::GetComboBoxContent)
		];
	}///
	//
	ChildSlot.HAlign(HAlign_Fill).VAlign(VAlign_Fill)
	[
		ComboBox.ToSharedRef()
	];
}

TSharedRef<SWidget> SQLComboBoxWidget::OnGenerateTypeComboBox(TSharedPtr<FString> InItem) {
	return SNew(STextBlock).Text(FText::FromString(*InItem.Get()));
}

FText SQLComboBoxWidget::GetComboBoxContent() const {
	if (!ColumnType.IsValid()) {return LOCTEXT("SQLComboBoxContent","NONE");}
	const bool HasSelectedItem = ComboBox.IsValid() && ComboBox->GetSelectedItem().IsValid();
	return (HasSelectedItem) ? FText::FromString(*ComboBox->GetSelectedItem().Get()) : FText::FromString(*ColumnType.Get());
}

void SQLComboBoxWidget::OnTypeComboBoxSelectionChanged(TSharedPtr<FString>NewSelection, ESelectInfo::Type SelectInfo) {
	Owner->SetColumnTypeByID(ColumnID,NewSelection);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////