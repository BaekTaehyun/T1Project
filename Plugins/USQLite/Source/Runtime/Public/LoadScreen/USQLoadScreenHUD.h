//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "USQL.h"
#include "USQLite_Shared.h"

#include "MoviePlayer.h"
#include "GameFramework/HUD.h"
#include "Runtime/Slate/Public/Widgets/Layout/SScaleBox.h"

#include "USQLoadScreenHUD.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class USQLite;
class SQLoadScreenBlurWidget;
class SQLoadScreenMovieWidget;
class SQLoadScreenSplashWidget;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSQLHUD_LoadScreen);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS(ClassGroup = Synaptech, Category="Synaptech", BlueprintType, Blueprintable, meta = (DisplayName="USQLite Load-Screen HUD"))
class USQLITE_API ASQLoadScreenHUD : public AHUD {
	GENERATED_BODY()
	friend class USQLite;
private:
	FDelegateHandle OnFinishMovie;
protected:
	TSharedPtr<SQLoadScreenBlurWidget> LoadScreenBlurWidget;
	TSharedPtr<SQLoadScreenMovieWidget> LoadScreenMovieWidget;
	TSharedPtr<SQLoadScreenSplashWidget> LoadScreenSplashWidget;
	//
	void DisplayBlurLoadScreenHUD(const ESQLThreadSafety TaskMode, const FText FeedbackText, const FSlateFontInfo &FeedbackFont, const FLinearColor &ProgressBarTint, const float BlurPower);
	void RemoveBlurLoadScreenHUD();
	//
	void DisplaySplashLoadScreenHUD(const ESQLThreadSafety TaskMode, const FText FeedbackText, const FSlateFontInfo &FeedbackFont, const FLinearColor &ProgressBarTint, const FStringAssetReference SplashAsset, TEnumAsByte<EStretch::Type> SplashStretch);
	void RemoveSplashLoadScreenHUD();
	//
	void DisplayMovieLoadScreenHUD(const ESQLThreadSafety TaskMode, const FText FeedbackText, const FSlateFontInfo &FeedbackFont, const FLinearColor &ProgressBarTint, const FStringAssetReference SplashMovie, const bool ProgressBarOnMovie);
	void RemoveMovieLoadScreenHUD();
	void BreakLoadScreenMovie();
public:
	UPROPERTY(Category="USQL", BlueprintAssignable, meta = (DisplayName="USQL: On Begin Load-Screen"))
	FSQLHUD_LoadScreen OnBeginLoadScreen;
	//
	UPROPERTY(Category="USQL", BlueprintAssignable, meta = (DisplayName="USQL: On Finish Load-Screen"))
	FSQLHUD_LoadScreen OnFinishLoadScreen;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////