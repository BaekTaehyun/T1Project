#pragma once
#include "GSFStageLobbyBase.h"

//------------------------------------------------------------------------------
// �������� �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class GSFStageLobby_CharacterSelect : public GSFStageLobbyBase
{
public:
	GSFStageLobby_CharacterSelect();
	virtual ~GSFStageLobby_CharacterSelect();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;
};

