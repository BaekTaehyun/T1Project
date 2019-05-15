#pragma once

#include "../GsStageMode.h"
#include "../../../Class/GsState.h"

//------------------------------------------------------------------------------
// 로비 스테이지를 관리할때 필요한 메모리 할당관리자(상속구조도 지원하기 위함)
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
