#pragma once

#include "../../../Class/GSTState.h"
#include "../../../Class/GSTStateMng.h"
#include "../GSFStageMode.h"
#include "GSFStageLobbyBase.h"

//------------------------------------------------------------------------------
// 게임모드를 관리할때 필요한 인스턴스 할당관리자(동일 인스턴스가 아닌 상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
class GSFStageLobbyAllocator : GSTMapAllocator<GSFStageMode::Lobby, GSFStageLobbyBase>
{
public:
	GSFStageLobbyAllocator() {}
	virtual ~GSFStageLobbyAllocator() {}
	virtual GSFStageLobbyBase* Alloc(GSFStageMode::Lobby inMode) override;
};

//------------------------------------------------------------------------------
// 게임모드를 관리
//------------------------------------------------------------------------------
class GSStageManagerLobby : public GSTStateMng<GSFStageMode::Lobby, GSFStageLobbyBase, GSFStageLobbyAllocator>
{
public:
	virtual ~GSStageManagerLobby() {};
	virtual void RemoveAll() override;
	virtual void InitState() override;
};
