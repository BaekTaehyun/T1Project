#pragma once

#include "GsGameMode.h"
#include "../Class/GsState.h"

//------------------------------------------------------------------------------
// 게임모드를 관리할때 필요한 메모리 할당관리자(상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
class FGsGameModeBase : public TGsState<FGsGameMode::Mode>
{
public:
	FGsGameModeBase() : TGsState<FGsGameMode::Mode>(FGsGameMode::Mode::MAX) {}
	FGsGameModeBase(FGsGameMode::Mode inMode) : TGsState<FGsGameMode::Mode>(inMode) {}
	virtual ~FGsGameModeBase() {}
	virtual void Enter() override{};
	virtual void Exit() override {};
	virtual void Update() override {};

	virtual void OnReconnectionStart() {};
	virtual void OnReconnectionEnd() {};
};