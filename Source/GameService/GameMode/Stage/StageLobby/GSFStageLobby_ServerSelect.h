#pragma once
#include "GSFStageLobbyBase.h"

//------------------------------------------------------------------------------
// ���������� �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class GSFStageLobby_ServerSelect : public GSFStageLobbyBase
{
public:
	GSFStageLobby_ServerSelect();
	virtual ~GSFStageLobby_ServerSelect();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;
};

