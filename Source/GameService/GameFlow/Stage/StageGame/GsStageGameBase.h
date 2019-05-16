#pragma once
#include "../GsStageMode.h"
#include "../../../Class/GsState.h"

//------------------------------------------------------------------------------
// 인게임 스테이지를 관리할때 필요한 메모리 할당관리자(상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
class FGsStageGameBase : public TGsState<FGsStageMode::Game>
{
public:
	FGsStageGameBase() : TGsState<FGsStageMode::Game>(FGsStageMode::Game::GAME_MAX) {}
	FGsStageGameBase(FGsStageMode::Game inMode) : TGsState<FGsStageMode::Game>(inMode) {}
	virtual ~FGsStageGameBase() {}
	virtual void Enter() override {};
	virtual void Exit() override {};
	virtual void Update() override {};

	virtual void OnReconectionStart() {};
	virtual void OnReconectionEnd() {};
};
