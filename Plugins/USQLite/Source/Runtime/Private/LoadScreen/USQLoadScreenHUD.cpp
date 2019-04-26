//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoadScreen/USQLoadScreenHUD.h"
#include "USQLoadScreenSplashWidget.h"
#include "USQLoadScreenMovieWidget.h"
#include "USQLoadScreenBlurWidget.h"

#include "Runtime/Engine/Classes/Engine/GameViewportClient.h"

#include "Widgets/SWeakWidget.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ASQLoadScreenHUD::DisplayBlurLoadScreenHUD(const ESQLThreadSafety TaskMode, const FText FeedbackText, const FSlateFontInfo &FeedbackFont, const FLinearColor &ProgressBarTint, const float BlurPower) {
	if (GEngine==nullptr||GEngine->GameViewport==nullptr) {return;}
	//
	SAssignNew(LoadScreenBlurWidget,SQLoadScreenBlurWidget)
	.ProgressBarTint(ProgressBarTint)
	.FeedbackFont(FeedbackFont)
	.FeedbackText(FeedbackText)
	.TaskMode(TaskMode)
	.Blur(BlurPower)
	.OwnerHUD(this);
	//
	GEngine->GameViewport->AddViewportWidgetContent(LoadScreenBlurWidget.ToSharedRef());
	LoadScreenBlurWidget->SetVisibility(EVisibility::Visible);
	//
	OnBeginLoadScreen.Broadcast();
}

void ASQLoadScreenHUD::RemoveBlurLoadScreenHUD() {
	if (GEngine==nullptr||GEngine->GameViewport==nullptr) {return;}
	//
	LoadScreenBlurWidget->SetVisibility(EVisibility::Hidden);
	GEngine->GameViewport->RemoveViewportWidgetContent(LoadScreenBlurWidget.ToSharedRef());
	//
	OnFinishLoadScreen.Broadcast();
}

void ASQLoadScreenHUD::DisplaySplashLoadScreenHUD(const ESQLThreadSafety TaskMode, const FText FeedbackText, const FSlateFontInfo &FeedbackFont, const FLinearColor &ProgressBarTint, const FStringAssetReference SplashAsset, TEnumAsByte<EStretch::Type> SplashStretch) {
	if (GEngine==nullptr||GEngine->GameViewport==nullptr) {return;}
	//
	SAssignNew(LoadScreenSplashWidget,SQLoadScreenSplashWidget)
	.ProgressBarTint(ProgressBarTint)
	.SplashStretch(SplashStretch)
	.FeedbackFont(FeedbackFont)
	.FeedbackText(FeedbackText)
	.SplashAsset(SplashAsset)
	.TaskMode(TaskMode)
	.OwnerHUD(this);
	//
	GEngine->GameViewport->AddViewportWidgetContent(LoadScreenSplashWidget.ToSharedRef());
	LoadScreenSplashWidget->SetVisibility(EVisibility::Visible);
	//
	OnBeginLoadScreen.Broadcast();
}

void ASQLoadScreenHUD::RemoveSplashLoadScreenHUD() {
	if (GEngine==nullptr||GEngine->GameViewport==nullptr) {return;}
	//
	LoadScreenSplashWidget->SetVisibility(EVisibility::Hidden);
	GEngine->GameViewport->RemoveViewportWidgetContent(LoadScreenSplashWidget.ToSharedRef());
	//
	OnFinishLoadScreen.Broadcast();
}

void ASQLoadScreenHUD::DisplayMovieLoadScreenHUD(const ESQLThreadSafety TaskMode, const FText FeedbackText, const FSlateFontInfo &FeedbackFont, const FLinearColor &ProgressBarTint, const FStringAssetReference SplashMovie, const bool ProgressBarOnMovie) {
	if (GEngine==nullptr||GEngine->GameViewport==nullptr) {return;}
	if (GetMoviePlayer()->IsMovieCurrentlyPlaying()) {return;}
	//
	auto MoviePath = SplashMovie.ToString();
	MoviePath.Split(TEXT("/Movies/"),nullptr,&MoviePath);
	MoviePath.Split(TEXT("."),&MoviePath,nullptr);
	//
	FLoadingScreenAttributes Attributes;
	Attributes.MoviePaths.Add(MoviePath);
	Attributes.bWaitForManualStop = false;
	Attributes.bMoviesAreSkippable = false;
	Attributes.bAllowInEarlyStartup = false;
	Attributes.bAutoCompleteWhenLoadingCompletes = false;
	Attributes.PlaybackType = EMoviePlaybackType::MT_Normal;
	//
	//
	if (ProgressBarOnMovie) {
		SAssignNew(LoadScreenMovieWidget,SQLoadScreenMovieWidget)
		.ProgressBarTint(ProgressBarTint)
		.FeedbackFont(FeedbackFont)
		.FeedbackText(FeedbackText)
		.TaskMode(TaskMode)
		.OwnerHUD(this);
		//
		LoadScreenMovieWidget->SetVisibility(EVisibility::Visible);
		Attributes.WidgetLoadingScreen = LoadScreenMovieWidget;
	}///
	//
	GetMoviePlayer()->SetupLoadingScreen(Attributes);
	if (GetMoviePlayer()->LoadingScreenIsPrepared()) {
		GetMoviePlayer()->PlayMovie();
	OnFinishMovie = GetMoviePlayer()->OnMoviePlaybackFinished().AddUObject(this,&ASQLoadScreenHUD::BreakLoadScreenMovie);}
	//
	OnBeginLoadScreen.Broadcast();
}

void ASQLoadScreenHUD::RemoveMovieLoadScreenHUD() {
	if (GEngine==nullptr||GEngine->GameViewport==nullptr) {return;}
	if (GetMoviePlayer()->IsMovieCurrentlyPlaying()) {return;}
	//
	LoadScreenMovieWidget->SetVisibility(EVisibility::Hidden);
	OnFinishLoadScreen.Broadcast();
}

void ASQLoadScreenHUD::BreakLoadScreenMovie() {
	if ((USQL::GetSaveProgress()>=100.f)&&(USQL::GetLoadProgress()>=100.f)) {
		GetMoviePlayer()->OnMoviePlaybackFinished().Remove(OnFinishMovie);
		RemoveMovieLoadScreenHUD();
	} else {GetMoviePlayer()->PlayMovie();}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////