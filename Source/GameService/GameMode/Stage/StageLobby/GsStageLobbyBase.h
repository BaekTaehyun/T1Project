#pragma once

#include "../GsStageMode.h"
#include "../../../Class/GsState.h"

//------------------------------------------------------------------------------
// �κ� ���������� �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class FGsStageLobbyBase : public TGsState<FGsStageMode::Lobby>
{
public:
	FGsStageLobbyBase() : TGsState<FGsStageMode::Lobby>(FGsStageMode::Lobby::LOBBY_MAX) {}
	FGsStageLobbyBase(FGsStageMode::Lobby inMode) : TGsState<FGsStageMode::Lobby>(inMode) {}
	virtual ~FGsStageLobbyBase() {}
	virtual void Enter() override {};
	virtual void Exit() override {};
	virtual void Update() override {};

	virtual void OnReconectionStart() {};
	virtual void OnReconectionEnd() {};
};
