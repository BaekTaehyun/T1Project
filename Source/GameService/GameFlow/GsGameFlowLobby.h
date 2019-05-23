#pragma once

#include "GsGameFlowBase.h"

class FGsMessageNone;
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

protected:
	/** �޽��� �ڵ鷯 ��� */
	void InitMessageHandler();

	/** MessageLobby::Stage �޽��� �ڵ鷯 �Լ� ���� */
	void OnIntroComplete();
	void OnAssetDownloadComplete();
	void OnLoginComplete(); // Hive �α��� or Dev ��� �������� �α��� �Ϸ�
	void OnServerSelectComplete();
	void OnBackToServerSelect();
	void OnEnterIngame();
	void OnIngameLoadComplete();

	void OnGVSDownloadComplete();
	void OnHiveLoginComplete(); // ���� ��忡�� Hive �α��� �Ϸ�
};

