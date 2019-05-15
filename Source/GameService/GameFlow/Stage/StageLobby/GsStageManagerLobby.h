#pragma once

#include "../../../Class/GsState.h"
#include "../../../Class/GsStateMng.h"
#include "../GsStageMode.h"
#include "GsStageLobbyBase.h"
//------------------------------------------------------------------------------
// 게임모드를 관리할때 필요한 인스턴스 할당관리자(동일 인스턴스가 아닌 상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
class FGsStageLobbyAllocator : TGsMapAllocator<FGsStageMode::Lobby, FGsStageLobbyBase>
{
public:
	FGsStageLobbyAllocator() {}
	virtual ~FGsStageLobbyAllocator() {}
	virtual FGsStageLobbyBase* Alloc(FGsStageMode::Lobby inMode) override;
};

//------------------------------------------------------------------------------
// 게임모드를 관리
//------------------------------------------------------------------------------
class FGsStageManagerLobby : public TGsStateMng<FGsStageMode::Lobby, FGsStageLobbyBase, FGsStageLobbyAllocator>
{
	typedef TGsStateMng<FGsStageMode::Lobby, FGsStageLobbyBase, FGsStageLobbyAllocator> Super;
public:
	virtual ~FGsStageManagerLobby() {};
	virtual void RemoveAll() override;
	virtual void InitState() override;
};
