#pragma once
#include "GSFStageLobbyBase.h"

//------------------------------------------------------------------------------
// �������� �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class GSFStageLobby_AssetDownLoad : public GSFStageLobbyBase
{
public:
	GSFStageLobby_AssetDownLoad();
	virtual ~GSFStageLobby_AssetDownLoad();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;
};

