#pragma once

#include "GsGameFlow.h"
#include "../Class/GsState.h"

//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
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