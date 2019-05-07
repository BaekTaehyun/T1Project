#pragma once
#include "GSFStageLobbyBase.h"

//------------------------------------------------------------------------------
// 게임모드를 관리할때 필요한 메모리 할당관리자(상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
class GSFStageLobby_Intro : public GSFStageLobbyBase
{
public:
	GSFStageLobby_Intro();
	virtual ~GSFStageLobby_Intro();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;
};

