#pragma once
#include "GSFStageLobbyBase.h"

//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class GSFStageLobby_Intro : public GSFStageLobbyBase
{
public:
	GSFStageLobby_Intro();
	virtual ~GSFStageLobby_Intro();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;
};

