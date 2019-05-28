#include "GsStageLobby_AssetDownload.h"
#include "GameService.h"
#include "Message/GsMessageManager.h"


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

	GMessage()->GetStage().SendMessage(MessageLobby::Stage::ENTER_ASSETDOWNLOAD_STAGE);
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