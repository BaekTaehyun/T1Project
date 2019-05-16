#include "GsStageManagerLobby.h"
#include "GsStageLobby_Intro.h"
#include "GsStageLobby_AssetDownLoad.h"
#include "GsStageLobby_ServerSelect.h"
#include "GsStageLobby_CharacterSelect.h"
#include "GsStageLobby_CharacterCreate.h"
#include "GsStageLobby_CharacterPreview.h"


//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �ν��Ͻ� �Ҵ������(���� �ν��Ͻ��� �ƴ� ��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
FGsStageLobbyBase* FGsStageLobbyAllocator::Alloc(FGsStageMode::Lobby inMode)
{
	switch (inMode)
	{
	case FGsStageMode::INTRO:
		return new FGsStageLobby_Intro();
	case FGsStageMode::ASSET_DOWNLOAD:
		return new FGsStageLobby_AssetDownLoad();
	case FGsStageMode::SERVER_SELECT:
		return new FGsStageLobby_ServerSelect();
	case FGsStageMode::CAHRACTER_SELECT:
		return new FGsStageLobby_CharacterSelect();
	case FGsStageMode::CHARACTER_CREATE:
		return new FGsStageLobby_CharacterCreate();
	case FGsStageMode::CHARACTER_PREVIEW:
		return new FGsStageLobby_CharecterPreview();
	case FGsStageMode::LOBBY_MAX:
	default:
		return NULL;
	}
	return NULL;
}
//------------------------------------------------------------------------------
// ���Ӹ�带 ����
//------------------------------------------------------------------------------
void FGsStageManagerLobby::RemoveAll()
{
	Super::RemoveAll();
}
void FGsStageManagerLobby::InitState()
{
	constexpr std::initializer_list<FGsStageMode::Lobby> allMode =
	{ 
		FGsStageMode::Lobby::INTRO,
		FGsStageMode::Lobby::ASSET_DOWNLOAD,
		FGsStageMode::Lobby::SERVER_SELECT,
		FGsStageMode::Lobby::CAHRACTER_SELECT,
		FGsStageMode::Lobby::CHARACTER_CREATE,
		FGsStageMode::Lobby::CHARACTER_PREVIEW
	};

	for (auto& e : allMode)
	{
		MakeInstance(e);
	}

	ChangeState(FGsStageMode::Lobby::INTRO);

	Super::InitState();
}
