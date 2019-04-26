//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "USQLoadScreenBlurWidget.h"
#include "USQLoadScreenStyle.h"

#include "Widgets/SOverlay.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SSafeZone.h"
#include "Widgets/Images/SThrobber.h"
#include "Widgets/Layout/SDPIScaler.h"
#include "Engine/UserInterfaceSettings.h"
#include "Widgets/Layout/SBackgroundBlur.h"
#include "Widgets/Notifications/SProgressBar.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOCTEXT_NAMESPACE "USQLoadScreen"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQLoadScreenBlurWidget::Construct(const FArguments &InArgs) {
	ProgressBarTint = InArgs._ProgressBarTint;
	FeedbackFont = InArgs._FeedbackFont;
	FeedbackText = InArgs._FeedbackText;
	OwnerHUD = InArgs._OwnerHUD.Get();
	TaskMode = InArgs._TaskMode;
	Blur = InArgs._Blur;
	DPIScale = 1.f;
	//
	TSharedRef<SOverlay>HUD_Overlay = SNew(SOverlay);
	//
	//
	HUD_Overlay->AddSlot(1000)
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		SAssignNew(Background,SBackgroundBlur)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.BlurStrength(Blur)
	];
	//
	HUD_Overlay->AddSlot(1001)
	.VAlign(VAlign_Bottom)
	.HAlign(HAlign_Fill)
	[
		SNew(SSafeZone)
		.IsTitleSafe(true)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Bottom)
		[
			SNew(SDPIScaler)
			.DPIScale(this,&SQLoadScreenBlurWidget::GetDPIScale)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot().AutoHeight()
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.Padding(FMargin(10,0.0f,0,0))
					.VAlign(VAlign_Center)
					.AutoWidth()
					[
						SNew(SCircularThrobber)
						.Radius((FeedbackFont.Size*96.f/72.f)/2.f)
					]///
					+SHorizontalBox::Slot()
					.Padding(FMargin(10.0f,0.0f,0,0))
					.VAlign(VAlign_Center)
					.AutoWidth()
					[
						SNew(STextBlock)
						.Text(FeedbackText)
						.Font(FeedbackFont)
					]///
					+SHorizontalBox::Slot()
					.HAlign(HAlign_Fill)
					.FillWidth(1)
					[
						SNew(SSpacer)
						.Size(FVector2D(1.0f,1.0f))
					]///
				]///
				+SVerticalBox::Slot().AutoHeight()
				[
					SAssignNew(ProgressBar,SProgressBar)
					.FillColorAndOpacity(FSlateColor(ProgressBarTint))
					.Style(&FSQLoadScreenStyle::IGet().GetWidgetStyle<FProgressBarStyle>("USQLProgressBar"))
				]///
			]///
		]///
	];
	//
	//
	ChildSlot
	[
		HUD_Overlay
	];
}

void SQLoadScreenBlurWidget::Tick(const FGeometry &AllottedGeometry, const double InCurrentTime, const float InDeltaTime) {
	const FVector2D &LocalSize = AllottedGeometry.GetLocalSize();
	FIntPoint Size((int32)LocalSize.X,(int32)LocalSize.Y);
	//
	const float NewDPI = GetDefault<UUserInterfaceSettings>()->GetDPIScaleBasedOnSize(Size);
	if (NewDPI!=DPIScale) {DPIScale=NewDPI; SlatePrepass(1.0f);}
	//
	ProgressBar->SetPercent(GetWorkloadProgress());
}

float SQLoadScreenBlurWidget::GetDPIScale() const {
	return DPIScale;
}

TOptional<float> SQLoadScreenBlurWidget::GetWorkloadProgress() const {
	switch (TaskMode) {
		case ESQLThreadSafety::AsynchronousSaving:
			return TOptional<float>(USQL::GetSaveProgress()/100);
		break;
		case ESQLThreadSafety::AsynchronousLoading:
			return TOptional<float>(USQL::GetLoadProgress()/100);
		break;
	default: return 0.f; break;}
}

#undef LOCTEXT_NAMESPACE

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////