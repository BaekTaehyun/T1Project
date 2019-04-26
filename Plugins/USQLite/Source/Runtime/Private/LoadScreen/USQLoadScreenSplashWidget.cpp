//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "USQLoadScreenSplashWidget.h"
#include "USQLoadScreenStyle.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SSafeZone.h"
#include "Widgets/Images/SThrobber.h"
#include "Widgets/Layout/SDPIScaler.h"
#include "Engine/UserInterfaceSettings.h"
#include "Widgets/Notifications/SProgressBar.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOCTEXT_NAMESPACE "USQLoadScreen"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQLoadScreenSplashWidget::Construct(const FArguments &InArgs) {
	ProgressBarTint = InArgs._ProgressBarTint;
	SplashStretch = InArgs._SplashStretch;
	FeedbackFont = InArgs._FeedbackFont;
	FeedbackText = InArgs._FeedbackText;
	OwnerHUD = InArgs._OwnerHUD.Get();
	SplashAsset = InArgs._SplashAsset;
	TaskMode = InArgs._TaskMode;
	DPIScale = 1.f;
	//
	TSharedRef<SOverlay>HUD_Overlay = SNew(SOverlay);
	//
	//
	auto SplashOBJ = SplashAsset.TryLoad();
	if (UTexture2D* SplashImage = Cast<UTexture2D>(SplashOBJ)) {
		FVector2D SplashSize = FVector2D(SplashImage->GetSizeX(),SplashImage->GetSizeY());
		SplashBrush = MakeShareable(new FSplashBrush(SplashImage,SplashSize,FName(*SplashAsset.ToString())));
		HUD_Overlay->AddSlot(1000).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
		[
			SNew(SScaleBox)
			.Stretch(SplashStretch)
			[
				SNew(SImage).Image(SplashBrush.Get())
			]///
		];
	}///
	//
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
			.DPIScale(this,&SQLoadScreenSplashWidget::GetDPIScale)
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

void SQLoadScreenSplashWidget::Tick(const FGeometry &AllottedGeometry, const double InCurrentTime, const float InDeltaTime) {
	const FVector2D &LocalSize = AllottedGeometry.GetLocalSize();
	FIntPoint Size((int32)LocalSize.X,(int32)LocalSize.Y);
	//
	const float NewDPI = GetDefault<UUserInterfaceSettings>()->GetDPIScaleBasedOnSize(Size);
	if (NewDPI!=DPIScale) {DPIScale=NewDPI; SlatePrepass(1.0f);}
	//
	ProgressBar->SetPercent(GetWorkloadProgress());
}

float SQLoadScreenSplashWidget::GetDPIScale() const {
	return DPIScale;
}

TOptional<float> SQLoadScreenSplashWidget::GetWorkloadProgress() const {
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