#pragma once
#include "GsStageLobbyBase.h"

//------------------------------------------------------------------------------
// �������� �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class FGsStageLobby_CharacterCreate : public FGsStageLobbyBase
{
public:
	FGsStageLobby_CharacterCreate();
	virtual ~FGsStageLobby_CharacterCreate();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;
};

