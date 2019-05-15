#pragma once

#include "GsGameFlowBase.h"

class FGsStageManagerLobby;
//------------------------------------------------------------------------------
// 게임모드를 관리할때 필요한 메모리 할당관리자(상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
class FGsGameFlowLobby : public FGsGameFlowBase
{
	TUniquePtr<FGsStageManagerLobby>	_stageManager;
public:
	FGsGameFlowLobby();
	virtual ~FGsGameFlowLobby();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;

	virtual void OnReconnectionStart() override;
	virtual void OnReconnectionEnd() override;
};

