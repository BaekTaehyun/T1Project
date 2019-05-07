#include "GSFStageLobby_AssetDownload.h"
#include "GameService.h"

GSFStageLobby_AssetDownLoad::GSFStageLobby_AssetDownLoad() : GSFStageLobbyBase(GSFStageMode::Lobby::ASSET_DOWNLOAD)
{
}

GSFStageLobby_AssetDownLoad::~GSFStageLobby_AssetDownLoad()
{
	GSLOG(Warning, TEXT("GSFStageLobby_AssetDownLoad : ~GSFStageLobby_AssetDownLoad"));
}

void GSFStageLobby_AssetDownLoad::Enter()
{
	GSFStageLobbyBase::Enter();
	GSLOG(Warning, TEXT("GSFStageLobby_AssetDownLoad : Enter"));
}

void GSFStageLobby_AssetDownLoad::Exit()
{
	GSFStageLobbyBase::Exit();
	GSLOG(Warning, TEXT("GSFStageLobby_AssetDownLoad : Exit"));
}

void GSFStageLobby_AssetDownLoad::Update()
{
	GSFStageLobbyBase::Update();
	GSLOG(Warning, TEXT("GSFStageLobby_AssetDownLoad : Update"));
}