#include "GSFGameModeLobby.h"
#include "GameService.h"

GSFGameModeLobby::GSFGameModeLobby() : GSFGameModeBase(GSFGameMode::Mode::LOBBY)
{
}

GSFGameModeLobby::~GSFGameModeLobby()
{
	GSLOG(Warning, TEXT("GSFGameModeLobby : ~GSFGameModeLobby"));
}

void GSFGameModeLobby::Enter()
{
	GSLOG(Warning, TEXT("GSFGameModeLobby : Enter"));
}

void GSFGameModeLobby::Exit()
{
	GSLOG(Warning, TEXT("GSFGameModeLobby : Exit"));
}

void GSFGameModeLobby::Update()
{
	GSLOG(Warning, TEXT("GSFGameModeLobby : Update"));
}