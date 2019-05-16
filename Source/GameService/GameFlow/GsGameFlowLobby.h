#pragma once

#include "GsGameFlowBase.h"

class FGsStageManagerLobby;
//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class FGsGameFlowLobby : public FGsGameFlowBase
{
	TUniquePtr<FGsStageManagerLobby>	_stageManager;
public:
	FGsGameFlowLobby();
	virtual ~FGsGameFlowLobby();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;

	virtual void OnReconnectionStart() override;
	virtual void OnReconnectionEnd() override;
};

