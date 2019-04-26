//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "USQLite.h"
#include "USQLStyle.h"
#include "USQLTableWidget.h"
#include "USQLPreviewWidget.h"
#include "USQLVersionBoxWidget.h"

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "SCurveEditor.h"
#include "PropertyEditing.h"
#include "DetailWidgetRow.h"
#include "IDetailPropertyRow.h"
#include "DetailLayoutBuilder.h"
#include "IDetailCustomization.h"
#include "DetailCategoryBuilder.h"
#include "IPropertyChangeListener.h"
#include "Widgets/Input/SNumericEntryBox.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class USQLEDITOR_API FUSQLDetails : public IDetailCustomization {
private:
	static IDetailLayoutBuilder* LayoutBuilder;
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	virtual void CustomizeDetails(IDetailLayoutBuilder &DetailBuilder) override;
	//
	static TAttribute<bool> OnEnable();
	static FReply OnLockTable(IDetailLayoutBuilder* DetailBuilder);
	static FReply OnClearTable(IDetailLayoutBuilder* DetailBuilder);
	static FReply OnTableApply(IDetailLayoutBuilder* DetailBuilder);
	static FReply OnAddTableColumn(IDetailLayoutBuilder* DetailBuilder);
	static FReply OnRefreshPreview(IDetailLayoutBuilder* DetailBuilder);
	static FReply OnDeleteTableColumn(IDetailLayoutBuilder* DetailBuilder);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class USQLEDITOR_API FUSQLTableDetails : public IPropertyTypeCustomization {
private:
	TSharedPtr<IPropertyHandle> Lock;
	TSharedPtr<IPropertyHandle> Header;
	TSharedPtr<IPropertyHandle> Footer;
	TSharedPtr<IPropertyHandle> Columns;
	TSharedPtr<IPropertyHandle> ColumnTypes;
	//
	uint32 LockID = 4;
	uint32 HeaderID = 0;
	uint32 FooterID = 3;
	uint32 ColumnsID = 2;
	uint32 ColumnTypesID = 1;
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	//
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow &HeaderRow, IPropertyTypeCustomizationUtils &StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder &StructBuilder, IPropertyTypeCustomizationUtils &StructCustomizationUtils) override;
};

class USQLEDITOR_API FUSQLPreviewDetails : public IPropertyTypeCustomization {
private:
	TSharedPtr<IPropertyHandle> Rows;
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	//
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow &HeaderRow, IPropertyTypeCustomizationUtils &StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder &StructBuilder, IPropertyTypeCustomizationUtils &StructCustomizationUtils) override;
};

class USQLEDITOR_API FUSQLVersionDetails : public IPropertyTypeCustomization {
private:
	TSharedPtr<IPropertyHandle> Version;
	TSharedPtr<IPropertyHandle> Versions;
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	//
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow &HeaderRow, IPropertyTypeCustomizationUtils &StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder &StructBuilder, IPropertyTypeCustomizationUtils &StructCustomizationUtils) override;
	//
	FReply OnDropTableButtonClicked();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
