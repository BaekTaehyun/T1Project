#include "GsStageLobby_Intro.h"
#include "GameService.h"

FGsStageLobby_Intro::FGsStageLobby_Intro() : FGsStageLobbyBase(FGsStageMode::Lobby::INTRO)
{
}

FGsStageLobby_Intro::~FGsStageLobby_Intro()
{
	GSLOG(Warning, TEXT("FGsStageLobby_Intro : ~FGsStageLobby_Intro"));
}

void FGsStageLobby_Intro::Enter()
{
	FGsStageLobbyBase::Enter();
	GSLOG(Warning, TEXT("FGsStageLobby_Intro : Enter"));
}

void FGsStageLobby_Intro::Exit()
{
	FGsStageLobbyBase::Exit();
	GSLOG(Warning, TEXT("FGsStageLobby_Intro : Exit"));
}

void FGsStageLobby_Intro::Update()
{
	FGsStageLobbyBase::Update();
	GSLOG(Warning, TEXT("FGsStageLobby_Intro : Update"));
}