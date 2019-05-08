#include "GSFStageManagerLobby.h"
#include "GSFStageLobby_Intro.h"
#include "GSFStageLobby_AssetDownLoad.h"
#include "GSFStageLobby_ServerSelect.h"
#include "GSFStageLobby_CharacterSelect.h"
#include "GSFStageLobby_CharacterCreate.h"
#include "GSFStageLobby_CharacterPreview.h"


//------------------------------------------------------------------------------
// 게임모드를 관리할때 필요한 인스턴스 할당관리자(동일 인스턴스가 아닌 상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
GSFStageLobbyBase* GSFStageLobbyAllocator::Alloc(GSFStageMode::Lobby inMode)
{
	switch (inMode)
	{
	case GSFStageMode::INTRO:
		return new GSFStageLobby_Intro();
	case GSFStageMode::ASSET_DOWNLOAD:
		return new GSFStageLobby_AssetDownLoad();
	case GSFStageMode::SERVER_SELECT:
		return new GSFStageLobby_ServerSelect();
	case GSFStageMode::CAHRACTER_SELECT:
		return new GSFStageLobby_CharacterSelect();
	case GSFStageMode::CHARACTER_CREATE:
		return new GSFStageLobby_CharacterCreate();
	case GSFStageMode::CHARACTER_PREVIEW:
		return new GSFStageLobby_CharecterPreview();
	case GSFStageMode::LOBBY_MAX:
	default:
		return NULL;
	}
	return NULL;
}
//------------------------------------------------------------------------------
// 게임모드를 관리
//------------------------------------------------------------------------------
void GSStageManagerLobby::RemoveAll()
{
	Super::RemoveAll();
}
void GSStageManagerLobby::InitState()
{
	constexpr std::initializer_list<GSFStageMode::Lobby> allMode =
	{ 
		GSFStageMode::Lobby::INTRO,
		GSFStageMode::Lobby::ASSET_DOWNLOAD,
		GSFStageMode::Lobby::SERVER_SELECT,
		GSFStageMode::Lobby::CAHRACTER_SELECT,
		GSFStageMode::Lobby::CHARACTER_CREATE,
		GSFStageMode::Lobby::CHARACTER_PREVIEW
	};

	for (auto& e : allMode)
	{
		MakeInstance(e);
	}

	ChangeState(GSFStageMode::Lobby::INTRO);

	Super::InitState();
}
