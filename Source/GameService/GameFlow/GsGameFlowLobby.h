#pragma once

#include "GsGameFlowBase.h"

class FGsMessageNone;
class FGsStageManagerLobby;
//------------------------------------------------------------------------------
// 게임모드를 관리할때 필요한 메모리 할당관리자(상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
class FGsGameFlowLobby : public FGsGameFlowBase
{
	TUniquePtr<FGsStageManagerLobby>	_stageManager;
public:
	FGsGameFlowLobby();
	virtual ~FGsGameFlowLobby();
	virtual void Init() override;
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;

	virtual void OnReconnectionStart() override;
	virtual void OnReconnectionEnd() override;

protected:
	/** 메시지 핸들러 등록 */
	void InitMessageHandler();

	/** MessageLobby::Stage 메시지 핸들러 함수 정의 */
	void OnIntroComplete();
	void OnAssetDownloadComplete();	
	void OnServerSelectComplete();
	void OnBackToServerSelect();
	void OnIngameLoadComplete();

	void OnGVSDownloadComplete();
	void OnHiveLoginComplete(); // 서비스 모드에서 Hive 로그인 완료
};

