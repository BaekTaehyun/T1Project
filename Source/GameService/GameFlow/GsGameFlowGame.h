#pragma once

#include "GsGameFlowBase.h"


class FGsStageManagerGame;
//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class FGsGameFlowGame : public FGsGameFlowBase
{
	TUniquePtr<FGsStageManagerGame>	_stageManager;
public:
	FGsGameFlowGame();
	virtual ~FGsGameFlowGame();

	virtual void Init() override;
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;

	virtual void OnReconnectionStart() override;
	virtual void OnReconnectionEnd() override;
};



