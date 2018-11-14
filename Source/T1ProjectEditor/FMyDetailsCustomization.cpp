#include "FMyDetailsCustomization.h"

#include "Editor/PropertyEditor/Public/DetailLayoutBuilder.h"
#include "Editor/PropertyEditor/Public/DetailCategoryBuilder.h"
#include "Editor/PropertyEditor/Public/PropertyEditing.h"
#include "IPropertyUtilities.h"
#include "SlateBasics.h"
#include "SBox.h"
#include "SBoxPanel.h"
#include "STextBlock.h"
#include "SlateFontInfo.h"
#include "PropertyCustomizationHelpers.h"
#include "Components/SplineComponent.h"
#include "ScopedTransaction.h"
#include "SNumericEntryBox.h"
#include "MyActor.h"

TSharedRef<IDetailCustomization> FMyDetailsCustomization::MakeInstance()
{
	return MakeShareable(new FMyDetailsCustomization);
}

void FMyDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{	
	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)[
		SNew(SButton)
			.Text(FText::FromString(TEXT("Push Me")))
			.ContentPadding(3)
	];

	TSharedRef<SWindow> CookbookWindow = SNew(SWindow)
		.Title(FText::FromString(TEXT("Test Window")))
		.ClientSize(FVector2D(800, 400))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Hello from Slate")))
			]
		];

	FSlateApplication::Get().AddWindow(CookbookWindow, true);

	IDetailCategoryBuilder& CategoryToEdit = DetailLayout.EditCategory("Test", FText::GetEmpty(), ECategoryPriority::Important);

	CategoryToEdit.AddCustomRow(FText::FromString("Row header name"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString("Custom row header name"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
		]
	.ValueContent().MinDesiredWidth(500)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Custom row content"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
		];

	// Add a row for hand entry of selected spline point locations
	FDetailWidgetRow& SplinePointLocationRow = CategoryToEdit.AddCustomRow(FText::GetEmpty())
		.NameContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString(("Edit Test", "Point(s) Location")))
		.Font(IDetailLayoutBuilder::GetDetailFont())
		];

	SplinePointLocationRow.ValueContent()
		.MaxDesiredWidth(10000)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.Padding(5, 0)
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(FText::FromString(("Edit Test", "X")))
		]
	+ SHorizontalBox::Slot()
		[
			SNew(SBox)
			.MinDesiredWidth(5000)
		[
			SNew(SNumericEntryBox<float>)
			.Delta(0.01f)
		//.Value(SplineEditWidget.Get(), &SSplineEditWidget::OnGetSelectedSplinePointLocation, EAxis::X)
		//.OnValueCommitted(SplineEditWidget.Get(), &SSplineEditWidget::OnSelectedSplinePointLocationCommitted, EAxis::^%X)
		//.IsEnabled(SplineEditWidget.Get(), &SSplineEditWidget::OnGetCanEditSelectedSplinePointLocationAndTangent)
		.UndeterminedString(FText::FromString(("MultipleValues", "Multiple Values")))
		//.AllowSpin(true)
		]
		]
	+ SHorizontalBox::Slot()
		.Padding(5, 0)
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(FText::FromString(("Edit Test", "Y")))
		]
	+ SHorizontalBox::Slot()
		[
			SNew(SBox)
			.MinDesiredWidth(5000)
		[
			SNew(SNumericEntryBox<float>)
			.Delta(0.01f)
		//.Value(SplineEditWidget.Get(), &SSplineEditWidget::OnGetSelectedSplinePointLocation, EAxis::Y)		
		//.OnValueCommitted(SplineEditWidget.Get(), &SSplineEditWidget::OnSelectedSplinePointLocationCommitted, EAxis::Y)
		//.IsEnabled(SplineEditWidget.Get(), &SSplineEditWidget::OnGetCanEditSelectedSplinePointLocationAndTangent)
		.UndeterminedString(FText::FromString(("MultipleValues", "Multiple Values")))
		//.AllowSpin(true)
		]
		]

		];

	//FText Label;
	//switch (TransformField)
	//{
	//case ETransformField::Rotation:
	//	Label = LOCTEXT("RotationLabel", "Rotation");
	//	break;
	//case ETransformField::Scale:
	//	Label = LOCTEXT("ScaleLabel", "Scale");
	//	break;
	//case ETransformField::Location:
	//default:
	//	Label = LOCTEXT("LocationLabel", "Location");
	//	break;
	//}

	//FMenuBuilder MenuBuilder(true, NULL, NULL);

	//FUIAction SetRelativeLocationAction
	//(
	//	//FExecuteAction::CreateSP(this, &FComponentTransformDetails::OnSetAbsoluteTransform, TransformField, false),
	//	//FCanExecuteAction(),
	//	//FIsActionChecked::CreateSP(this, &FComponentTransformDetails::IsAbsoluteTransformChecked, TransformField, false)
	//);

	//FUIAction SetWorldLocationAction
	//(
	//	//FExecuteAction::CreateSP(this, &FComponentTransformDetails::OnSetAbsoluteTransform, TransformField, true),
	//	//FCanExecuteAction(),
	//	//FIsActionChecked::CreateSP(this, &FComponentTransformDetails::IsAbsoluteTransformChecked, TransformField, true)
	//);

	//MenuBuilder.BeginSection(TEXT("TransformType"), FText::Format(LOCTEXT("TransformType", "{0} Type"), Label));

	//MenuBuilder.AddMenuEntry
	//(
	//	FText::Format(LOCTEXT("RelativeLabel", "Relative"), Label),
	//	FText::Format(LOCTEXT("RelativeLabel_ToolTip", "{0} is relative to its parent"), Label),
	//	FSlateIcon(),
	//	SetRelativeLocationAction,
	//	NAME_None,
	//	EUserInterfaceActionType::RadioButton
	//);

	//MenuBuilder.AddMenuEntry
	//(
	//	FText::Format(LOCTEXT("WorldLabel", "World"), Label),
	//	FText::Format(LOCTEXT("WorldLabel_ToolTip", "{0} is relative to the world"), Label),
	//	FSlateIcon(),
	//	SetWorldLocationAction,
	//	NAME_None,
	//	EUserInterfaceActionType::RadioButton
	//);

	//MenuBuilder.EndSection();


	//return
	//	SNew(SComboButton)
	//	.ContentPadding(0)
	//	.ButtonStyle(FEditorStyle::Get(), "NoBorder")
	//	.ForegroundColor(FSlateColor::UseForeground())
	//	.MenuContent()
	//	[
	//		MenuBuilder.MakeWidget()
	//	]
	//.ButtonContent()
	//	[
	//		SNew(SBox)
	//		.Padding(FMargin(0.0f, 0.0f, 2.0f, 0.0f))
	//	[
	//		SNew(STextBlock)
	//		//.Text(this, &FComponentTransformDetails::GetTransformFieldText, TransformField)
	//	.Font(IDetailLayoutBuilder::GetDetailFont())
	//	]
	//	];

	// 가져오는거?
	/*TSharedRef<IPropertyHandle> PropertyHandle =
		DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(AMyActor, PropertyName));
	if (PropertyHandle.IsValid())
	{
		CategoryToEdit.AddProperty(PropertyHandle);
	}*/

	//CategoryToEdit.AddWidget()
	//	[
	//		SNew(SHorizontalBox)
	//		+ SHorizontalBox::Slot()
	//	[
	//		// 새로운 SProperty 생성
	//		SNew(SProperty, EnableOverrideLightmapRes)
	//	]
	//+ SHorizontalBox::Slot()
	//	.Padding(4.0f, 0.0f)
	//	.MaxWidth(50)
	//	[
	//		SNew(SProperty, LightingCategory.GetProperty("OverriddenLightMapRes"))
	//		.NamePlacement(EPropertyNamePlacement::Hidden) // Hide the name
	//	]
	//	];


	//CategoryToEdit.ADd
	//CategoryToEdit.AddProperty("dasdas", FText::FromString("dsadasd"));

	// bOverrideLightmapRes 프로퍼티로의 핸들 구하기
	//TSharedPtr<IPropertyHandle> OverrideLightmapRes = DetailLayout.GetProperty("bOverrideLightmapRes");

	//CategoryToEdit.AddWidget()
	//	[
	//		SNew(SHorizontalBox)
	//		+ SHorizontalBox::Slot()
	//	[
	//		// 새로운 SProperty 생성
	//		SNew(SProperty, EnableOverrideLightmapRes)
	//	]
	//+ SHorizontalBox::Slot()
	//	.Padding(4.0f, 0.0f)
	//	.MaxWidth(50)
	//	[
	//		SNew(SProperty, LightingCategory.GetProperty("OverriddenLightMapRes"))
	//		.NamePlacement(EPropertyNamePlacement::Hidden) // Hide the name
	//	]
	//	];

	//CategoryToEdit.AddGroup()

	//FDetailWidgetRow& SplineEditWidgetCustomRow = CategoryToEdit.AddCustomRow(FText::GetEmpty());

	//CategoryToEdit.AddProperty()
	//CategoryToEdit.AddProperty("bTest", TEXT("Test Static"));

	/*TSharedRef<IPropertyHandle> PropertyHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(AMyActor,  "Test"));
	if (PropertyHandle.IsUnique())
	{
		CategoryToEdit.AddProperty(PropertyHandle);
	}*/
	
	//CategoryToEdit.AddProperty()

	//CategoryToEdit.AddProperty()

	//IDetailCategory& LightingCategory = DetailLayout.EditCategory("Lighting", TEXT("OptionalLocalizedDisplayName"))

	/*TSharedRef<IPropertyHandle> PropertyHandle =
		DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(AMyActor, propertyName));*/
}