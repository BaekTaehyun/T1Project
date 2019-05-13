#pragma once

#include "GsGameModeBase.h"


class FGsStageManagerGame;
//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class FGsGameModeGame : public FGsGameModeBase
{
	TUniquePtr<FGsStageManagerGame>	_stageManager;
public:
	FGsGameModeGame();
	virtual ~FGsGameModeGame();

	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;

	virtual void OnReconnectionStart() override;
	virtual void OnReconnectionEnd() override;
};



