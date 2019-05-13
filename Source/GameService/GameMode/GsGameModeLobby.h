#pragma once

#include "GsGameModeBase.h"

class FGsStageManagerLobby;
//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class FGsGameModeLobby : public FGsGameModeBase
{
	TUniquePtr<FGsStageManagerLobby>	_stageManager;
public:
	FGsGameModeLobby();
	virtual ~FGsGameModeLobby();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;

	virtual void OnReconnectionStart() override;
	virtual void OnReconnectionEnd() override;
};

