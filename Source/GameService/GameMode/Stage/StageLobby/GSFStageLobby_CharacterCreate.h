#pragma once
#include "GSFStageLobbyBase.h"

//------------------------------------------------------------------------------
// �������� �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class GSFStageLobby_CharacterCreate : public GSFStageLobbyBase
{
public:
	GSFStageLobby_CharacterCreate();
	virtual ~GSFStageLobby_CharacterCreate();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;
};

