#pragma once
#include "GsStageLobbyBase.h"

//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class FGsStageLobby_Intro : public FGsStageLobbyBase
{
public:
	FGsStageLobby_Intro();
	virtual ~FGsStageLobby_Intro();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;
};

