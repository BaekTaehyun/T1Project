#pragma once

#include "../../../Class/GsState.h"
#include "../../../Class/GsStateMng.h"
#include "../GsStageMode.h"
#include "GsStageLobbyBase.h"
//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �ν��Ͻ� �Ҵ������(���� �ν��Ͻ��� �ƴ� ��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class FGsStageLobbyAllocator : TGsMapAllocator<FGsStageMode::Lobby, FGsStageLobbyBase>
{
public:
	FGsStageLobbyAllocator() {}
	virtual ~FGsStageLobbyAllocator() {}
	virtual FGsStageLobbyBase* Alloc(FGsStageMode::Lobby inMode) override;
};

//------------------------------------------------------------------------------
// ���Ӹ�带 ����
//------------------------------------------------------------------------------
class FGsStageManagerLobby : public TGsStateMng<FGsStageMode::Lobby, FGsStageLobbyBase, FGsStageLobbyAllocator>
{
	typedef TGsStateMng<FGsStageMode::Lobby, FGsStageLobbyBase, FGsStageLobbyAllocator> Super;
public:
	virtual ~FGsStageManagerLobby() {};
	virtual void RemoveAll() override;
	virtual void InitState() override;
};
