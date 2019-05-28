#include "GsGameFlowLobby.h"
#include "GameService.h"
#include "Stage/StageLobby/GsStageManagerLobby.h"
#include "Message/GsMessageManager.h"


FGsGameFlowLobby::FGsGameFlowLobby() : FGsGameFlowBase(FGsGameFlow::Mode::LOBBY)
{
}

FGsGameFlowLobby::~FGsGameFlowLobby()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : ~FGsGameFlowLobby"));
}

void FGsGameFlowLobby::Init()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : Init"));
	InitMessageHandler();
}

void FGsGameFlowLobby::Enter()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : Enter"));
	_stageManager = TUniquePtr<FGsStageManagerLobby>(new FGsStageManagerLobby());
	if (_stageManager.IsValid())
	{
		_stageManager->InitState();
	}
	
	//spawner = GetWorld()->SpawnActor<AGsGameObjectManager>();
	//spawner->Initialize();
}

void FGsGameFlowLobby::Exit()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : Exit"));
	if (_stageManager.IsValid())
	{
		_stageManager->RemoveAll();
	}
}

void FGsGameFlowLobby::Update()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : Update"));
}

void FGsGameFlowLobby::OnReconnectionStart()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : OnReconectionStart"));
}

void FGsGameFlowLobby::OnReconnectionEnd()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : OnReconectionEnd"));
}

void FGsGameFlowLobby::InitMessageHandler()
{
	GMessage()->GetStage().AddRaw(MessageLobby::Stage::INTRO_COMPLETE, this, &FGsGameFlowLobby::OnIntroComplete);
	GMessage()->GetStage().AddRaw(MessageLobby::Stage::ASSETDOWN_COMPLETE, this, &FGsGameFlowLobby::OnAssetDownloadComplete);	
	GMessage()->GetStage().AddRaw(MessageLobby::Stage::SERVER_SELECTCOMPLETE, this, &FGsGameFlowLobby::OnServerSelectComplete);
	GMessage()->GetStage().AddRaw(MessageLobby::Stage::BACKTO_SERVER_SELECT, this, &FGsGameFlowLobby::OnBackToServerSelect);
	
	//GMessage()->GetStage().AddRaw(MessageLobby::Stage::INGAME_LOAD_COMPLETE, this, &FGsGameFlowLobby::OnIngameLoadComplete);

	GMessage()->GetHive().AddRaw(MessageLobby::Hive::GVS_DOWNLOAD_COMPLETE, this, &FGsGameFlowLobby::OnGVSDownloadComplete);
	GMessage()->GetHive().AddRaw(MessageLobby::Hive::HIVE_LOGIN_COMPLETE, this, &FGsGameFlowLobby::OnHiveLoginComplete);
}

void FGsGameFlowLobby::OnIntroComplete()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : OnIntroComplete"));

	_stageManager->ChangeState(FGsStageMode::Lobby::ASSET_DOWNLOAD);
}

void FGsGameFlowLobby::OnAssetDownloadComplete()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : OnAssetDownloadComplete"));

	_stageManager->ChangeState(FGsStageMode::Lobby::SERVER_SELECT);
}

void FGsGameFlowLobby::OnServerSelectComplete()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : OnServerSelectComplete"));

	_stageManager->ChangeState(FGsStageMode::Lobby::CAHRACTER_SELECT);
}

void FGsGameFlowLobby::OnGVSDownloadComplete()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : OnGVSDownloadComplete"));

	// TOOD:
}

void FGsGameFlowLobby::OnHiveLoginComplete()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : OnHiveLoginComplete"));

	GMessage()->GetStage().SendMessage(MessageLobby::Stage::LOGIN_COMPLETE);
}

void FGsGameFlowLobby::OnBackToServerSelect()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : OnBackToServerSelect"));

	_stageManager->ChangeState(FGsStageMode::Lobby::SERVER_SELECT);
}



