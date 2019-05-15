#pragma once
#include "../GsStageMode.h"
#include "../../../Class/GsState.h"

//------------------------------------------------------------------------------
// �ΰ��� ���������� �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
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
