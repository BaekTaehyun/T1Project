#pragma once
#include "GsStageLobbyBase.h"

//------------------------------------------------------------------------------
// �������� �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class FGsStageLobby_CharacterSelect : public FGsStageLobbyBase
{
public:
	FGsStageLobby_CharacterSelect();
	virtual ~FGsStageLobby_CharacterSelect();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;
};

