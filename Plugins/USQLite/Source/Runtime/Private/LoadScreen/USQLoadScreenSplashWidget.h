//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "USQL.h"
#include "Engine/Texture2D.h"
#include "Runtime/Slate/Public/SlateBasics.h"
#include "Runtime/SlateCore/Public/Fonts/SlateFontInfo.h"
#include "Runtime/Slate/Public/Widgets/Layout/SScaleBox.h"
#include "Runtime/SlateCore/Public/Widgets/SCompoundWidget.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SThrobber;
class SProgressBar;
class ASQLoadScreenHUD;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct FSplashBrush : public FSlateDynamicImageBrush, public FGCObject {
	FSplashBrush(class UTexture2D* InTexture, const FVector2D &InImageSize, const FName InImagePath) : FSlateDynamicImageBrush(InTexture, InImageSize, InImagePath) {}
	//
	virtual void AddReferencedObjects(FReferenceCollector &Collector) override {
		auto Resource = GetResourceObject();
		if (Resource) {Collector.AddReferencedObject(Resource);}
	}///
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SQLoadScreenSplashWidget : public SCompoundWidget {
private:
	TSharedPtr<FSplashBrush> SplashBrush;
	TSharedPtr<SProgressBar> ProgressBar;
	TSharedPtr<SThrobber> Throbber;
	//
	TWeakObjectPtr<ASQLoadScreenHUD> OwnerHUD;
	TEnumAsByte<EStretch::Type> SplashStretch;
	FStringAssetReference SplashAsset;
	FLinearColor ProgressBarTint;
	FSlateFontInfo FeedbackFont;
	ESQLThreadSafety TaskMode;
	FText FeedbackText;
	//
	float DPIScale = 1.f;
	float GetDPIScale() const;
	TOptional<float> GetWorkloadProgress() const;
public:
	SLATE_BEGIN_ARGS(SQLoadScreenSplashWidget)
	: _OwnerHUD(), _SplashStretch(), _SplashAsset(), _ProgressBarTint(), _FeedbackFont(), _TaskMode(), _FeedbackText()
	{}
		SLATE_ARGUMENT(TWeakObjectPtr<ASQLoadScreenHUD>,OwnerHUD);
		SLATE_ARGUMENT(TEnumAsByte<EStretch::Type>,SplashStretch);
		SLATE_ARGUMENT(FStringAssetReference,SplashAsset);
		SLATE_ARGUMENT(FLinearColor,ProgressBarTint);
		SLATE_ARGUMENT(FSlateFontInfo,FeedbackFont);
		SLATE_ARGUMENT(ESQLThreadSafety,TaskMode);
		SLATE_ARGUMENT(FText,FeedbackText);
	SLATE_END_ARGS()
	//
	void Construct(const FArguments &InArgs);
	virtual void Tick(const FGeometry &AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////