#include "GsStageLobby_CharacterPreview.h"
#include "GameService.h"

FGsStageLobby_CharecterPreview::FGsStageLobby_CharecterPreview() : FGsStageLobbyBase(FGsStageMode::Lobby::CHARACTER_PREVIEW)
{
}

FGsStageLobby_CharecterPreview::~FGsStageLobby_CharecterPreview()
{
	GSLOG(Warning, TEXT("FGsStageLobby_CharecterPreview : ~FGsStageLobby_CharecterPreview"));
}

void FGsStageLobby_CharecterPreview::Enter()
{
	FGsStageLobbyBase::Enter();
	GSLOG(Warning, TEXT("FGsStageLobby_CharecterPreview : Enter"));
}

void FGsStageLobby_CharecterPreview::Exit()
{
	FGsStageLobbyBase::Exit();
	GSLOG(Warning, TEXT("FGsStageLobby_CharecterPreview : Exit"));
}

void FGsStageLobby_CharecterPreview::Update()
{
	FGsStageLobbyBase::Update();
	GSLOG(Warning, TEXT("FGsStageLobby_CharecterPreview : Update"));
}