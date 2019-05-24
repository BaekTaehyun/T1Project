#include "GsStageLobby_AssetDownload.h"
#include "GameService.h"
#include "UI/GsUIManager.h"
#include "GameMode/GsGameModeLobby.h"


FGsStageLobby_AssetDownLoad::FGsStageLobby_AssetDownLoad() : FGsStageLobbyBase(FGsStageMode::Lobby::ASSET_DOWNLOAD)
{
}

FGsStageLobby_AssetDownLoad::~FGsStageLobby_AssetDownLoad()
{
	GSLOG(Warning, TEXT("FGsStageLobby_AssetDownLoad : ~FGsStageLobby_AssetDownLoad"));
}

void FGsStageLobby_AssetDownLoad::Enter()
{
	FGsStageLobbyBase::Enter();
	GSLOG(Warning, TEXT("FGsStageLobby_AssetDownLoad : Enter"));

	AGsGameModeLobby* GameModeLobby = AGsGameModeLobby::GetGameModeLobby();
	if (nullptr != GameModeLobby)
	{
		AGsUIManager* UIManager = GameModeLobby->GetUIManager();
		if (nullptr != UIManager)
		{
			UIManager->PushByKeyName(FName(TEXT("WindowAssetDownload")));
			
#pragma todo("yjchoung: Test Code")
			// TEST: 다운로드 시작
			GameModeLobby->TestStartDownload();
		}
	}
}

void FGsStageLobby_AssetDownLoad::Exit()
{
	FGsStageLobbyBase::Exit();
	GSLOG(Warning, TEXT("FGsStageLobby_AssetDownLoad : Exit"));
}

void FGsStageLobby_AssetDownLoad::Update()
{
	FGsStageLobbyBase::Update();
	GSLOG(Warning, TEXT("FGsStageLobby_AssetDownLoad : Update"));
}