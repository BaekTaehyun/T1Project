#pragma once

#include "GsGameFlow.h"
#include "../Class/GsState.h"

//------------------------------------------------------------------------------
// 게임모드를 관리할때 필요한 메모리 할당관리자(상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
class FGsGameFlowBase : public TGsState<FGsGameFlow::Mode>
{
public:
	FGsGameFlowBase() : TGsState<FGsGameFlow::Mode>(FGsGameFlow::Mode::MAX) {}
	FGsGameFlowBase(FGsGameFlow::Mode inMode) : TGsState<FGsGameFlow::Mode>(inMode) {}
	virtual ~FGsGameFlowBase() {}
	virtual void Enter() override{};
	virtual void Exit() override {};
	virtual void Update() override {};

	virtual void OnReconnectionStart() {};
	virtual void OnReconnectionEnd() {};
};