#pragma once
#include "GSFStageLobbyBase.h"

//------------------------------------------------------------------------------
// 스테이지를 관리할때 필요한 메모리 할당관리자(상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
class GSFStageLobby_ServerSelect : public GSFStageLobbyBase
{
public:
	GSFStageLobby_ServerSelect();
	virtual ~GSFStageLobby_ServerSelect();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;
};

