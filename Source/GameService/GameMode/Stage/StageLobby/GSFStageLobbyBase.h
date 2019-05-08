#pragma once

#include "../GSFStageMode.h"
#include "../../../Class/GSTState.h"

//------------------------------------------------------------------------------
// 로비 스테이지를 관리할때 필요한 메모리 할당관리자(상속구조도 지원하기 위함)
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
