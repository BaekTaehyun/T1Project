#pragma once

#include "GSFGameModeBase.h"

//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------

class GSFGameModeGame : public GSFGameModeBase
{
public:
	GSFGameModeGame();
	virtual ~GSFGameModeGame();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;
};



