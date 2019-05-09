#pragma once

#include "GSFGameModeBase.h"


class GSStageManagerGame;
//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class GSFGameModeGame : public GSFGameModeBase
{
	TUniquePtr<GSStageManagerGame>	_stageManager;
public:
	GSFGameModeGame();
	virtual ~GSFGameModeGame();	

	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;

	virtual void OnReconnectionStart() override;
	virtual void OnReconnectionEnd() override;
};



