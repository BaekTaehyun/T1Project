#pragma once
#include "GsStageLobbyBase.h"

//------------------------------------------------------------------------------
// ���������� �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class FGsStageLobby_ServerSelect : public FGsStageLobbyBase
{
public:
	FGsStageLobby_ServerSelect();
	virtual ~FGsStageLobby_ServerSelect();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;
};

