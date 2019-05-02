#include "GSGameModeManager.h"


void GSGameModeManager::RemoveAll()
{
	GSTStateMng<GSFGameMode::Mode, GSFGameModeBase, GSFGameModeAllocator>::RemoveAll();
}
void GSGameModeManager::InitState()
{
	constexpr std::initializer_list<GSFGameMode::Mode> allMode = { GSFGameMode::Mode::LOBBY, GSFGameMode::Mode::GAME };
	for (auto& e : allMode)
	{
		MakeInstance(e);
	}

	ChangeState(GSFGameMode::Mode::LOBBY);
}