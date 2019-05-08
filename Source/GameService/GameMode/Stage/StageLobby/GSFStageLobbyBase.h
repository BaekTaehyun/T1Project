#pragma once

#include "../GSFStageMode.h"
#include "../../../Class/GSTState.h"

//------------------------------------------------------------------------------
// �κ� ���������� �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class GSFStageLobbyBase : public GSTState<GSFStageMode::Lobby>
{
public:
	GSFStageLobbyBase() : GSTState<GSFStageMode::Lobby>(GSFStageMode::Lobby::LOBBY_MAX) {}
	GSFStageLobbyBase(GSFStageMode::Lobby inMode) : GSTState<GSFStageMode::Lobby>(inMode) {}
	virtual ~GSFStageLobbyBase() {}
	virtual void Enter() override {};
	virtual void Exit() override {};
	virtual void Update() override {};

	virtual void OnReconectionStart() {};
	virtual void OnReconectionEnd() {};
};
