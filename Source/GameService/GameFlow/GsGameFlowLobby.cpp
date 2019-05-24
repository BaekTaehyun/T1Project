#include "GsGameFlowLobby.h"
#include "GameService.h"
#include "Stage/StageLobby/GsStageManagerLobby.h"
#include "Message/GsMessageManager.h"
#include "GameMode/GsGameModeLobby.h"


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
	GMessage()->GetStage().AddRaw(MessageLobby::Stage::ENTER_INGAME, this, &FGsGameFlowLobby::OnEnterIngame);
	GMessage()->GetStage().AddRaw(MessageLobby::Stage::INGAME_LOAD_COMPLETE, this, &FGsGameFlowLobby::OnIngameLoadComplete);

	GMessage()->GetHive().AddRaw(MessageLobby::Hive::GVS_DOWNLOAD_COMPLETE, this, &FGsGameFlowLobby::OnGVSDownloadComplete);
	GMessage()->GetHive().AddRaw(MessageLobby::Hive::HIVE_LOGIN_COMPLETE, this, &FGsGameFlowLobby::OnHiveLoginComplete);
}

void FGsGameFlowLobby::OnIntroComplete()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : OnIntroComplete"));

	AGsGameModeLobby* GameModeLobby = AGsGameModeLobby::GetGameModeLobby();
	if (nullptr != GameModeLobby)
	{
		// 개발모드일 경우 바로 로그인 화면으로 연결
		if (GameModeLobby->IsDevMode())
		{
			_stageManager->ChangeState(FGsStageMode::Lobby::SERVER_SELECT);
			return;
		}
	}

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

	// 현재 UI 스택에서 빼기?

	_stageManager->ChangeState(FGsStageMode::Lobby::SERVER_SELECT);
}

void FGsGameFlowLobby::OnEnterIngame()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : OnEnterIngame"));

	// FIX: 여기서 받는게 맞는가
}

void FGsGameFlowLobby::OnIngameLoadComplete()
{
	// FIX: 여기서 받는게 맞는가
}