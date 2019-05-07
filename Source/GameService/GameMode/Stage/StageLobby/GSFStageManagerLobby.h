#pragma once

#include "../../../Class/GSTState.h"
#include "../../../Class/GSTStateMng.h"
#include "../GSFStageMode.h"
#include "GSFStageLobbyBase.h"

//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �ν��Ͻ� �Ҵ������(���� �ν��Ͻ��� �ƴ� ��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class GSFStageLobbyAllocator : GSTMapAllocator<GSFStageMode::Lobby, GSFStageLobbyBase>
{
public:
	GSFStageLobbyAllocator() {}
	virtual ~GSFStageLobbyAllocator() {}
	virtual GSFStageLobbyBase* Alloc(GSFStageMode::Lobby inMode) override;
};

//------------------------------------------------------------------------------
// ���Ӹ�带 ����
//------------------------------------------------------------------------------
class GSStageManagerLobby : public GSTStateMng<GSFStageMode::Lobby, GSFStageLobbyBase, GSFStageLobbyAllocator>
{
public:
	virtual ~GSStageManagerLobby() {};
	virtual void RemoveAll() override;
	virtual void InitState() override;
};
