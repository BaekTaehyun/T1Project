#pragma once
#include "GsStageLobbyBase.h"

//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class FGsStageLobby_CharecterPreview: public FGsStageLobbyBase
{
public:
	FGsStageLobby_CharecterPreview();
	virtual ~FGsStageLobby_CharecterPreview();
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Update() override;
};

