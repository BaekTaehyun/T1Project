#pragma once

#include "GsGameMode.h"
#include "../Class/GsState.h"

//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
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