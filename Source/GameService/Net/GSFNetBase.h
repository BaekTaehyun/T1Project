#pragma once
#include "GSFNet.h"
#include "../Class/GSTState.h"
#include "../Class/GSTMessageHandler.h"

#include "./Network/Session.h"

//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class GSFNetModeBase : public GSTState<GSFNet::Mode>
{
	std::shared_ptr<Session> session_;  // ���� ������ ����
public:
	GSFNetModeBase() : GSTState<GSFNet::Mode>(GSFNet::Mode::MAX) {}
	GSFNetModeBase(GSFNet::Mode inMode) : GSTState<GSFNet::Mode>(inMode) {}
	virtual ~GSFNetModeBase() {}
	virtual void Enter() override {};
	virtual void Exit() override {};
	virtual void Update() override {};
};