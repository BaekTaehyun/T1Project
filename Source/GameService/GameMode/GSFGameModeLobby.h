#pragma once

#include "GSFGameModeBase.h"
#include "./Stage/GSFStageManagerLobby.h"

//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class GSFGameModeLobby : public GSFGameModeBase
{
	TUniquePtr<GSStageManagerLobby>	_stageManager;
public:
	GSFGameModeLobby();
	virtual ~GSFGameModeLobby();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;
};

